/**
 * MainWindow Class
 *
 * --------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/aspicat/shaderide.
 * --------------------------------------------------------------------------
 *
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <QDebug>
#include <QSurfaceFormat>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStandardPaths>
#include <QJsonDocument>
#include "MainWindow.hpp"
#include "StyleSheets.hpp"
#include "src/GL/GLDefaults.hpp"
#include "src/Core/GeneralException.hpp"
#include "src/Project/ProjectException.hpp"
#include "src/Core/Memory.hpp"
#include "src/Core/ApplicationDefaults.hpp"

using namespace ShaderIDE::GUI;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow                       (parent),
      numSamples                        (SHADERIDE_SURFACEFORMAT_NUM_SAMPLES),
      centralWidget                     (nullptr),
      verticalLayout                    (nullptr),
      verticalSplitter                  (nullptr),
      mainSplitter                      (nullptr),
      menuBar                           (nullptr),
      openGLWidget                      (nullptr),
      fileTabWidget                     (nullptr),
      logOutputWidget                   (nullptr),
      settingsDialog                    (nullptr),
      aboutDialog                       (nullptr),
      statusBar                         (nullptr),
      fileMenu                          (nullptr),
      newProjectAction                  (nullptr),
      openProjectAction                 (nullptr),
      saveProjectAction                 (nullptr),
      saveProjectAsAction               (nullptr),
      exportShadersAction               (nullptr),
      settingsAction                    (nullptr),
      exitAction                        (nullptr),
      viewMenu                          (nullptr),
      swapLayoutAction                  (nullptr),
      resetLayoutAction                 (nullptr),
      saveLayoutAction                  (nullptr),
      toggleLogAction                   (nullptr),
      codeMenu                          (nullptr),
      compileCodeAction                 (nullptr),
      toggleRealtimeCompilationAction   (nullptr),
      toggleWordWrapAction              (nullptr),
      helpMenu                          (nullptr),
      aboutAction                       (nullptr),
      shaderProject                     (nullptr)
{
    LoadSettings();

    // Apply default surface format for Anti-Aliasing.
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(numSamples);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    InitLayout();
    InitMenuBar();
    InitOpenGLWidget();
    InitFileTabWidget();
    InitLogOutputWidget();
    InitSettingsDialog();
    InitAboutDialog();
    InitStatusBar();
    InitShaderProject();

    UpdateWindowTitle();

    // Load Layout Config
    LoadLayoutConfig();

    sl_UpdateStatusBarMessage("Ready.");
}

MainWindow::~MainWindow() {
    DestroyShaderProject();
    DestroyMenuBar();
    DestroyLayout();
}

void MainWindow::sl_VSCodeChanged(const QString &code) {
    openGLWidget->SetVertexShaderSource(code);
}

void MainWindow::sl_FSCodeChanged(const QString &code) {
    openGLWidget->SetFragmentShaderSource(code);
}

void MainWindow::sl_TextureBrowserImageChanged(TextureBrowserImage *image) {
    auto slot = OpenGLWidget::FindSlotByName(image->Name());
    openGLWidget->ApplyTextureToSlot(image->ImageHQ(), slot);
    sl_UpdateStatusBarMessage(QString("Texture \"") + image->Name() + "\" changed.");
}

void MainWindow::sl_TextureBrowserImageCleared(TextureBrowserImage *image) {
    auto slot = OpenGLWidget::FindSlotByName(image->Name());
    openGLWidget->ClearTextureSlot(slot);
    shaderProject->ClearTexturePath(image->Name().toStdString());
    sl_UpdateStatusBarMessage(QString("Texture \"") + image->Name() + "\" cleared.");
}

void MainWindow::sl_TextureBrowserImagePathChanged(TextureBrowserImage *image) {
    shaderProject->SetTexturePath(image->Name().toStdString(), image->Path().toStdString());
}

void MainWindow::sl_GLInitialized() {

    // Fetch all textures from File Tab Widget's texture browser
    // and apply these to the OpenGLWidget texture slots.
    ApplyTextureSlots();

    // Apply default code snippets for shaders,
    // after GL is initialized in OpenGLWidget.
    // If we apply the sources earlier, we might
    // have a segmentation fault for glShaderSource(...),
    // since the shader was not yet initialized.
    if (fileTabWidget->CodeEditorsReady()) {
        fileTabWidget->SetVertexShaderSource(GLSL_DEFAULT_VS_SOURCE);
        fileTabWidget->SetFragmentShaderSource(GLSL_DEFAULT_FS_SOURCE);

        // Only compile at first initialization if realtime compilation
        // is not enabled, otherwise this step is redundant.
        if (!openGLWidget->RealtimeCompilation()) {
            openGLWidget->sl_CompileShaders();
        }
    }
}

void MainWindow::sl_CompileSuccess(const QString &message) {
    fileTabWidget->GetVSCodeEditor()->ResetErrorLines();
    fileTabWidget->GetFSCodeEditor()->ResetErrorLines();
    logOutputWidget->LogSuccessMessage(message);
    sl_UpdateStatusBarMessage(message);
}

void MainWindow::sl_CompileError(GLSLCompileError &error) {
    auto errorMessage = logOutputWidget->LogGLSLError(error);
    sl_UpdateStatusBarMessage(errorMessage);

    // TODO Stylesheet.
    auto highlightColor = QColor("#7A1F2F");

    // TODO Instead of comparing a string here, define an enum class
    //      for shader types.
    if (error.File() == "VS") {
        auto *vsCodeEditor = fileTabWidget->GetVSCodeEditor();
        vsCodeEditor->HighlightErrorLine(error.Line(), highlightColor);
        fileTabWidget->setCurrentWidget(vsCodeEditor);
    }

    if (error.File() == "FS") {
        auto *fsCodeEditor = fileTabWidget->GetFSCodeEditor();
        fsCodeEditor->HighlightErrorLine(error.Line(), highlightColor);
        fileTabWidget->setCurrentWidget(fsCodeEditor);
    }
}

void MainWindow::sl_GeneralError(const GeneralException &e) {
    logOutputWidget->LogErrorMessage(e.what());
    sl_UpdateStatusBarMessage(e.what());
}

void MainWindow::sl_UpdateStatusBarMessage(const QString &message) {
    statusBar->showMessage(message, SHADERIDE_STATUSBAR_TIMEOUT);
}

void MainWindow::sl_Menu_File_NewProject() {
    auto mbButton = QMessageBox::question(
            nullptr,
            "New Project",
            "All unsaved changes will be lost.\n\nCreate new project?"
    );

    if (mbButton == QMessageBox::Button::Yes) {
        ResetProject();
        sl_UpdateStatusBarMessage("New project created.");
    }
}

void MainWindow::sl_Menu_File_OpenProject() {
    QString path = QFileDialog::getOpenFileName(
            this,
            "Open Project...",
            QString(),
            QString("Shader IDE Files (") + SHADERIDE_PROJECT_EXTENSION + ")"
    );

    try {
        shaderProject = ShaderProject::LoadFromJSON(path.toStdString());
        shaderProject->SetPath(path.toStdString());

        if (ApplyUIFromProject()) {
            sl_UpdateStatusBarMessage(QString("Shader project loaded: ") + path);
        }

    } catch (GeneralException &e) {
        sl_GeneralError(e);
        return;
    }
}

void MainWindow::sl_Menu_File_SaveProject() {
    try {
        ApplyProjectFromUI();
        shaderProject->SaveAsJSON();
        sl_Menu_Code_Compile();
        sl_UpdateStatusBarMessage("Shader project saved.");

    } catch (ProjectException &e) {
        if (e.Code() == ProjectException::ExceptionCode::PATH_EMPTY) {
            sl_Menu_File_SaveProjectAs();
            return;
        }

        logOutputWidget->LogErrorMessage(e.what());
    }
}

void MainWindow::sl_Menu_File_SaveProjectAs() {
    QString path = QFileDialog::getSaveFileName(
            this,
            "Save Project as...",
            QString(),
            QString("Shader IDE Files (") + SHADERIDE_PROJECT_EXTENSION + ")"
    );

    if (path.isEmpty()) {
        return;
    }

    try {
        ApplyProjectFromUI();
        shaderProject->SetPath(path.toStdString());
        shaderProject->SaveAsJSON();
        ApplyUIFromProject();

        sl_Menu_Code_Compile();
        sl_UpdateStatusBarMessage(QString("Shader project saved to ") + path);

    } catch (ProjectException &e) {
        logOutputWidget->LogErrorMessage(e.what());
    }
}

void MainWindow::sl_Menu_File_ExportShaders() {
    auto directoryURL = QFileDialog::getExistingDirectoryUrl();

    if (!directoryURL.isEmpty()) {
        ExportShaders(directoryURL);
    }
}

void MainWindow::sl_Menu_File_Settings() {
    settingsDialog->show();
}

void MainWindow::sl_Menu_File_Exit() {
    auto mbButton = QMessageBox::question(
            nullptr,
            "Exit",
            "All unsaved changes will be lost.\n\nContinue?"
    );

    if (mbButton != QMessageBox::Yes) {
        return;
    }

    // Disable realtime compilation before closing, to
    // avoid another compilation step after the code editors
    // were cleared by Qt. This caused an error in the
    // log output widget, since the editors trigger the
    // compilation process with some log output, but the log
    // widget is destroyed first (see Destructor).
    openGLWidget->SetRealtimeCompilationEnabled(false);

    QApplication::instance()->quit();
}

void MainWindow::sl_Menu_View_SwapLayout() {
    SwapLayout();
}

void MainWindow::sl_Menu_View_ResetLayout() {
    ResetLayout();
}

void MainWindow::sl_Menu_View_SaveLayout() {
    SaveLayoutConfig();
}

void MainWindow::sl_Menu_View_ToggleLog() {
    logOutputWidget->setVisible(!logOutputWidget->isVisible());
    toggleLogAction->setIconVisibleInMenu(logOutputWidget->isVisible());
    SaveLayoutConfig();
}

void MainWindow::sl_Menu_Code_Compile() {
    fileTabWidget->GetVSCodeEditor()->ResetErrorLines();
    fileTabWidget->GetFSCodeEditor()->ResetErrorLines();
    openGLWidget->sl_CompileShaders();
}

void MainWindow::sl_Menu_Code_ToggleRealtimeCompilation() {
    openGLWidget->ToggleRealtimeCompilation();
    toggleRealtimeCompilationAction->setIconVisibleInMenu(openGLWidget->RealtimeCompilation());
    SaveLayoutConfig();
}

void MainWindow::sl_Menu_Code_ToggleWordWrap() {
    fileTabWidget->ToggleWordWrap();
    toggleWordWrapAction->setIconVisibleInMenu(fileTabWidget->WordWrap());
    SaveLayoutConfig();
}

void MainWindow::sl_Menu_Help_About() {
    aboutDialog->show();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    sl_Menu_File_Exit();
}

QString MainWindow::GetConfigPath() {

    // Linux / UNIX
#ifndef WIN32
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/ShaderIDE";
#endif

    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}

bool MainWindow::AppConfigDirExists() {
    return QDir(GetConfigPath()).exists();
}

void MainWindow::InitLayout() {
    resize(STYLE_MAINWINDOW_WIDTH, STYLE_MAINWINDOW_HEIGHT);
    setStyleSheet(STYLE_MAINWINDOW);

    // Central Widget
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Vertical Layout
    verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    verticalLayout->setSpacing(0);
    centralWidget->setLayout(verticalLayout);

    // Vertical Splitter
    verticalSplitter = new QSplitter();
    verticalSplitter->setOrientation(Qt::Orientation::Vertical);
    verticalSplitter->setContentsMargins(0, 0, 0, 0);
    verticalLayout->addWidget(verticalSplitter);

    // Main Splitter
    mainSplitter = new QSplitter();
    mainSplitter->setContentsMargins(0, 0, 0, 0);
    verticalSplitter->addWidget(mainSplitter);
}

void MainWindow::InitMenuBar() {
    menuBar = new QMenuBar(this);
    menuBar->setStyleSheet(STYLE_MENUBAR);
    setMenuBar(menuBar);

    InitMenuFile();
    InitMenuView();
    InitMenuCode();
    InitMenuHelp();
}

void MainWindow::InitMenuFile() {
    fileMenu = new QMenu("&File");
    menuBar->addMenu(fileMenu);

    newProjectAction     = new QAction("New Project");
    openProjectAction    = new QAction("Open Project...");
    saveProjectAction    = new QAction("Save Project");
    saveProjectAsAction  = new QAction("Save Project as...");
    exportShadersAction  = new QAction("Export Shaders...");
    settingsAction       = new QAction("Settings...");
    exitAction           = new QAction("Exit");

    saveProjectAction->setShortcut(QKeySequence::Save);

    fileMenu->addAction(newProjectAction);
    fileMenu->addAction(openProjectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(saveProjectAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportShadersAction);
    fileMenu->addSeparator();
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(newProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_NewProject()));

    connect(openProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_OpenProject()));

    connect(saveProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_SaveProject()));

    connect(saveProjectAsAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_SaveProjectAs()));

    connect(exportShadersAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_ExportShaders()));

    connect(settingsAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_Settings()));

    connect(exitAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_File_Exit()));
}

void MainWindow::InitMenuView() {
    viewMenu = new QMenu("&View");
    menuBar->addMenu(viewMenu);

    swapLayoutAction    = new QAction("Swap Layout");
    resetLayoutAction   = new QAction("Reset Layout");
    saveLayoutAction    = new QAction("Save Layout");

    toggleLogAction = new QAction("Toggle Log");
    toggleLogAction->setIcon(QIcon(":/icons/icon-menu-check.png"));
    toggleLogAction->setIconVisibleInMenu(false);
    toggleLogAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));

    viewMenu->addAction(swapLayoutAction);
    viewMenu->addAction(resetLayoutAction);
    viewMenu->addAction(saveLayoutAction);
    viewMenu->addSeparator();
    viewMenu->addAction(toggleLogAction);

    connect(swapLayoutAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_View_SwapLayout()));

    connect(resetLayoutAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_View_ResetLayout()));

    connect(saveLayoutAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_View_SaveLayout()));

    connect(toggleLogAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_View_ToggleLog()));
}

void MainWindow::InitMenuCode() {
    codeMenu = new QMenu("&Code");
    menuBar->addMenu(codeMenu);

    // Compile
    compileCodeAction = new QAction("Compile");
    compileCodeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

    // Realtime Compilation
    toggleRealtimeCompilationAction = new QAction("Toggle Realtime Compilation");
    toggleRealtimeCompilationAction->setIcon(QIcon(":/icons/icon-menu-check.png"));
    toggleRealtimeCompilationAction->setIconVisibleInMenu(false);
    toggleRealtimeCompilationAction->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_R));

    // Word Wrap
    toggleWordWrapAction = new QAction("Toggle Word Wrap");
    toggleWordWrapAction->setIcon(QIcon(":/icons/icon-menu-check.png"));
    toggleWordWrapAction->setIconVisibleInMenu(false);

    // Add Actions
    codeMenu->addAction(compileCodeAction);
    codeMenu->addSeparator();
    codeMenu->addAction(toggleRealtimeCompilationAction);
    codeMenu->addSeparator();
    codeMenu->addAction(toggleWordWrapAction);

    // Signals & Slots
    connect(compileCodeAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_Code_Compile()));

    connect(toggleRealtimeCompilationAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_Code_ToggleRealtimeCompilation()));

    connect(toggleWordWrapAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_Code_ToggleWordWrap()));
}

void MainWindow::InitMenuHelp() {
    helpMenu = new QMenu("&Help");
    menuBar->addMenu(helpMenu);

    aboutAction = new QAction("About");
    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_Menu_Help_About()));
}

void MainWindow::InitOpenGLWidget() {
    openGLWidget = new OpenGLWidget();
    openGLWidget->setMinimumWidth(400);
    openGLWidget->resize(800, openGLWidget->height());
    mainSplitter->addWidget(openGLWidget);
    mainSplitter->setCollapsible(mainSplitter->indexOf(openGLWidget), false);

    connect(openGLWidget, SIGNAL(si_GLInitialized()),
            this, SLOT(sl_GLInitialized()));

    connect(openGLWidget, SIGNAL(si_CompileSuccess(const QString&)),
            this, SLOT(sl_CompileSuccess(const QString&)));

    connect(openGLWidget, SIGNAL(si_CompileError(GLSLCompileError&)),
            this, SLOT(sl_CompileError(GLSLCompileError&)));

    connect(openGLWidget, SIGNAL(si_StateUpdated(const QString&)),
            this, SLOT(sl_UpdateStatusBarMessage(const QString&)));

    connect(openGLWidget, SIGNAL(si_GeneralError(const GeneralException&)),
            this, SLOT(sl_GeneralError(const GeneralException&)));
}

void MainWindow::InitFileTabWidget() {

    // File Tab Widget
    fileTabWidget = new FileTabWidget();
    mainSplitter->addWidget(fileTabWidget);

    // Disable Collapsing
    auto ftwIndex = mainSplitter->indexOf(fileTabWidget);
    mainSplitter->setCollapsible(ftwIndex, false);
    verticalSplitter->setStretchFactor(ftwIndex, 1);

    connect(fileTabWidget, SIGNAL(si_VSCodeChanged(const QString&)),
            this, SLOT(sl_VSCodeChanged(const QString&)));

    connect(fileTabWidget, SIGNAL(si_FSCodeChanged(const QString&)),
            this, SLOT(sl_FSCodeChanged(const QString&)));

    connect(fileTabWidget->GetTextureBrowser(),
            SIGNAL(si_ImageChanged(TextureBrowserImage*)),
            this,
            SLOT(sl_TextureBrowserImageChanged(TextureBrowserImage*)));

    connect(fileTabWidget->GetTextureBrowser(),
            SIGNAL(si_ImageCleared(TextureBrowserImage*)),
            this,
            SLOT(sl_TextureBrowserImageCleared(TextureBrowserImage*)));

    connect(fileTabWidget->GetTextureBrowser(),
            SIGNAL(si_ImagePathChanged(TextureBrowserImage*)),
            this,
            SLOT(sl_TextureBrowserImagePathChanged(TextureBrowserImage*)));
}

void MainWindow::InitLogOutputWidget() {
    logOutputWidget = new LogOutputWidget();
    verticalSplitter->addWidget(logOutputWidget);
    verticalSplitter->setCollapsible(verticalSplitter->indexOf(logOutputWidget), false);
    verticalSplitter->setSizes(QList<int>({ height(), 120 }));
    toggleLogAction->setIconVisibleInMenu(logOutputWidget->isVisible());
}

void MainWindow::InitSettingsDialog() {
    settingsDialog = new SettingsDialog(this);
}

void MainWindow::InitAboutDialog() {
    aboutDialog = new AboutDialog();
}

void MainWindow::InitStatusBar() {
    statusBar = new QStatusBar();
    statusBar->setStyleSheet(STYLE_STATUSBAR);
    verticalLayout->addWidget(statusBar);
}

void MainWindow::InitShaderProject() {
    shaderProject = new ShaderProject("");
    shaderProject->SetTexturePath(GLSL_TEXTURE_SLOT_0_NAME, "");
    shaderProject->SetTexturePath(GLSL_TEXTURE_SLOT_1_NAME, "");
    shaderProject->SetTexturePath(GLSL_TEXTURE_SLOT_2_NAME, "");
    shaderProject->SetTexturePath(GLSL_TEXTURE_SLOT_3_NAME, "");
}

void MainWindow::DestroyShaderProject() {
    Memory::Release(shaderProject);
}

void MainWindow::DestroyMenuBar() {

    // Help Menu
    Memory::Release(aboutAction);
    Memory::Release(helpMenu);

    // Code Menu
    Memory::Release(toggleWordWrapAction);
    Memory::Release(toggleRealtimeCompilationAction);
    Memory::Release(compileCodeAction);
    Memory::Release(codeMenu);

    // View Menu
    Memory::Release(toggleLogAction);
    Memory::Release(saveLayoutAction);
    Memory::Release(resetLayoutAction);
    Memory::Release(swapLayoutAction);
    Memory::Release(viewMenu);

    // File Menu
    Memory::Release(exitAction);
    Memory::Release(settingsAction);
    Memory::Release(exportShadersAction);
    Memory::Release(saveProjectAsAction);
    Memory::Release(saveProjectAction);
    Memory::Release(openProjectAction);
    Memory::Release(newProjectAction);
    Memory::Release(fileMenu);
}

void MainWindow::DestroyLayout() {
    Memory::Release(statusBar);
    Memory::Release(logOutputWidget);
    Memory::Release(fileTabWidget);
    Memory::Release(openGLWidget);
    Memory::Release(menuBar);
    Memory::Release(mainSplitter);
    Memory::Release(verticalSplitter);
    Memory::Release(verticalLayout);
    Memory::Release(centralWidget);

    aboutDialog->deleteLater();
    settingsDialog->deleteLater();
}

void MainWindow::SwapLayout() {

    // Hide (remove) widgets from splitter.
    fileTabWidget->hide();
    openGLWidget->hide();

    // We use bit negation to swap the index of 0 and 1.
    // Not a perfect solution right here, but we cannot simply
    // swap widgets inside a splitter. If we have more than two
    // widgets in future versions (i.e. a sidebar), we need a
    // more complex solution of course.
    auto openGLWidgetIndex = mainSplitter->indexOf(openGLWidget);
    mainSplitter->insertWidget(~openGLWidgetIndex, openGLWidget);
    mainSplitter->insertWidget(openGLWidgetIndex, fileTabWidget);

    fileTabWidget->show();
    openGLWidget->show();
}

void MainWindow::ResetLayout() {

    // Reset Size
    QList<int> sizes;
    sizes << 1 << 1;
    mainSplitter->setSizes(sizes);

    // This is just a quick solution, since there are only
    // two widgets inside the splitter. See SwapLayout().
    if (mainSplitter->indexOf(openGLWidget) == 1) {
        SwapLayout();
    }

    logOutputWidget->setVisible(true);
    toggleLogAction->setIconVisibleInMenu(logOutputWidget->isVisible());
}

void MainWindow::UpdateWindowTitle() {
    QString windowTitle = SHADERIDE_APPLICATION_NAME;

    // TODO -> Future Versions -> Project Name
    if (shaderProject != nullptr) {
        if (!shaderProject->Path().empty()) {
            windowTitle.append(" - ")
            .append(QString::fromStdString(shaderProject->Path()));
            // TODO .append("*"); --> if unsaved changes are available.
        }
    }

    setWindowTitle(windowTitle);
}

void MainWindow::ApplyTextureSlots() {
    auto textures = fileTabWidget->GetTextureBrowser()->Images();

    // Find Textures
    auto slotIt0 = textures.find(GLSL_TEXTURE_SLOT_0_NAME);
    auto slotIt1 = textures.find(GLSL_TEXTURE_SLOT_1_NAME);
    auto slotIt2 = textures.find(GLSL_TEXTURE_SLOT_2_NAME);
    auto slotIt3 = textures.find(GLSL_TEXTURE_SLOT_3_NAME);

    const auto errorFmt = "Slot texture \"%s\" not available in texture browser. Ignored.";

    if (slotIt0 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt0.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_0);
    } else {
        LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_0_NAME));
    }

    if (slotIt1 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt1.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_1);
    } else {
        LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_1_NAME));
    }

    if (slotIt2 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt2.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_2);
    } else {
        LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_2_NAME));
    }

    if (slotIt3 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt3.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_3);
    } else {
        LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_3_NAME));
    }
}

void MainWindow::LogMessage(const QString &message) {
    logOutputWidget->LogMessage(message);
    sl_UpdateStatusBarMessage(message);
}

void MainWindow::ResetProject() {
    Memory::Release(shaderProject);

    shaderProject = new ShaderProject("");
    shaderProject->SetVertexShaderSource(GLSL_DEFAULT_VS_SOURCE);
    shaderProject->SetFragmentShaderSource(GLSL_DEFAULT_FS_SOURCE);

    openGLWidget->ResetUI();
    fileTabWidget->ResetUI();

    ApplyUIFromProject();
}

bool MainWindow::ApplyProjectFromUI() {
    shaderProject->SetVertexShaderSource(
            fileTabWidget->VertexShaderSource().toStdString());

    shaderProject->SetFragmentShaderSource(
            fileTabWidget->FragmentShaderSource().toStdString());

    // Ignore "Plane" mesh, which is only used by "Plane 2D" mode.
    auto meshName = openGLWidget->SelectedMeshName().toStdString();

    if (meshName != "Plane") {
        shaderProject->SetMeshName(meshName);
    }

    for (const auto &texture : fileTabWidget->GetTextureBrowser()->Images()) {
        shaderProject->SetTexturePath(
                texture->Name().toStdString(),
                texture->Path().toStdString()
        );
    }

    shaderProject->SetRealtime(openGLWidget->Realtime());
    shaderProject->SetPlane2D(openGLWidget->Plane2D());
    shaderProject->SetModelRotation(openGLWidget->ModelRotation());
    shaderProject->SetCameraPosition(openGLWidget->CameraPosition());

    return true;
}

bool MainWindow::ApplyUIFromProject() {
    if (shaderProject == nullptr) {
        auto msg = "Could not load project.";
        logOutputWidget->LogErrorMessage(msg);
        sl_UpdateStatusBarMessage(msg);
        return false;
    }

    fileTabWidget->SetVertexShaderSource(shaderProject->VertexShaderSource().c_str());
    fileTabWidget->SetFragmentShaderSource(shaderProject->FragmentShaderSource().c_str());

    if (shaderProject->Plane2D()) {
        openGLWidget->EnablePlane2DAfterModelLoaded();
    }

    openGLWidget->sl_CompileShaders();
    openGLWidget->SelectMesh(QString::fromStdString(shaderProject->MeshName()));

    for (auto &texture : shaderProject->TexturePaths()) {
        auto path = QString::fromStdString(texture.second);

        if (path.isEmpty()) {
            continue;
        }

        auto slotName = QString::fromStdString(texture.first);
        auto slot     = OpenGLWidget::FindSlotByName(slotName);
        auto image    = QImage(path);

        openGLWidget->ApplyTextureToSlot(image, slot);
        fileTabWidget->GetTextureBrowser()->GetImage(slotName)->UpdateImage(image);
        fileTabWidget->GetTextureBrowser()->GetImage(slotName)->SetPath(path);
    }

    openGLWidget->CheckRealtime(shaderProject->Realtime());
    openGLWidget->RotateModel(shaderProject->ModelRotation());
    openGLWidget->MoveCamera(shaderProject->CameraPosition());

    UpdateWindowTitle();

    return true;
}

void MainWindow::ExportShaders(const QUrl &directory) {
    auto vsFilePath = directory.toLocalFile().append("/vs.glsl");
    auto fsFilePath = directory.toLocalFile().append("/fs.glsl");
    auto errorFmt = "Could not create shader file \"%s\".";

    // Vertex Shader File
    QFile vsFile(vsFilePath);
    vsFile.open(QIODevice::OpenModeFlag::WriteOnly);

    if (vsFile.error() != QFileDevice::NoError) {
        logOutputWidget->LogErrorMessage(
                QString::asprintf(errorFmt, vsFilePath.toStdString().c_str())
        );
    }

    if (vsFile.isOpen()) {
        vsFile.write(fileTabWidget->VertexShaderSource().toUtf8());
        vsFile.close();
    }

    // Fragment Shader File
    QFile fsFile(fsFilePath);
    fsFile.open(QIODevice::OpenModeFlag::WriteOnly);

    if (fsFile.error() != QFileDevice::NoError) {
        logOutputWidget->LogErrorMessage(
                QString::asprintf(errorFmt, fsFilePath.toStdString().c_str())
        );
    }

    if (fsFile.isOpen()) {
        fsFile.write(fileTabWidget->FragmentShaderSource().toUtf8());
        fsFile.close();
    }
}

void MainWindow::CreateAppConfigDirectory() {
    if (AppConfigDirExists()) {
        return;
    }

    QDir().mkpath(GetConfigPath());

    if (!AppConfigDirExists()) {
        auto msg = "Could not create config directory \"" + GetConfigPath() + "\".";
        LogMessage(msg);
    }
}

void MainWindow::SaveLayoutConfig() {
    CreateAppConfigDirectory();

    if (!AppConfigDirExists()) {
        auto msg = "Could not save layout, app config directory not found.";
        LogMessage(msg);
        return;
    }

    // Create layout config file.
    QFile file(GetConfigPath() + "/" + CONFIG_FILE_NAME);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        auto msg = "Could not store application config file.";
        LogMessage(msg);
        return;
    }

    // Collect Variables
    config = QJsonObject();

    // Layout
    QJsonObject config_layout;
    config_layout["swapped"]     = mainSplitter->indexOf(openGLWidget) == 1; // See SwapLayout();
    config_layout["width_left"]  = mainSplitter->widget(0)->width(); // See SwapLayout();
    config_layout["width_right"] = mainSplitter->widget(1)->width(); // See SwapLayout();
    config_layout["show_log"]    = logOutputWidget->isVisible();
    config["layout"] = config_layout;

    // Window
    QJsonObject config_window;
    config_window["maximized"]  = isMaximized();
    config_window["width"]      = width();
    config_window["height"]     = height();
    config["window"] = config_window;

    // Code
    QJsonObject config_code;
    config_code["word_wrap"] = fileTabWidget->WordWrapMode();
    config_code["realtime_compilation"] = openGLWidget->RealtimeCompilation();
    config["code"] = config_code;

    QJsonDocument jsonDocument(config);
    file.write(jsonDocument.toJson());
}

void MainWindow::LoadLayoutConfig() {
    if (!AppConfigDirExists()) {
        return;
    }

    QFile file(GetConfigPath() + "/" + CONFIG_FILE_NAME);

    if (!file.open(QIODevice::ReadOnly)) {
        auto msg = "Could not load layout config file.";
        LogMessage(msg);
        return;
    }

    auto content = file.readAll();
    auto jsonDocument = QJsonDocument::fromJson(content);
    config = jsonDocument.object();

    if (config.isEmpty()) {
        return;
    }

    // Layout
    if (config.find("layout") != config.end()) {
        auto config_layout = config.find("layout")->toObject();

        // Swapped
        if (config_layout.contains("swapped") && config_layout["swapped"].toBool()) {
            SwapLayout();
        }

        // Width Left
        if (config_layout.contains("width_left")) {
            mainSplitter->widget(0)->resize(
                    config_layout["width_left"].toInt(),
                    mainSplitter->height()
            );
        }

        // Width Right
        if (config_layout.contains("width_right")) {
            mainSplitter->widget(1)->resize(
                    config_layout["width_right"].toInt(),
                    mainSplitter->height()
            );
        }

        // Show Log
        if (config_layout.contains("show_log")) {
            auto show_log = config_layout["show_log"].toBool();
            logOutputWidget->setVisible(show_log);
            toggleLogAction->setIconVisibleInMenu(show_log);
        }
    }

    // Window
    if (config.find("window") != config.end()) {
        auto config_window = config.find("window")->toObject();

        // Maximized?
        if (config_window.contains("maximized") &&
            config_window["maximized"].toBool()) {
            showMaximized();
        }

        // Width
        if (config_window.contains("width")) {
            resize(config_window["width"].toInt(), height());
        }

        // Height
        if (config_window.contains("height")) {
            resize(width(), config_window["height"].toInt());
        }
    }

    // Code
    if (config.find("code") != config.end()) {
        auto config_code = config.find("code")->toObject();

        // Realtime Compilation
        if (config_code.contains("realtime_compilation")) {
            auto rtcEnabled = config_code["realtime_compilation"].toBool();
            openGLWidget->SetRealtimeCompilationEnabled(rtcEnabled);
            toggleRealtimeCompilationAction->setIconVisibleInMenu(rtcEnabled);
        }

        // Word Wrap
        if (config_code.contains("word_wrap")) {
            auto mode = static_cast<QTextOption::WrapMode>(config_code["word_wrap"].toInt());
            fileTabWidget->SetWordWrapMode(mode);
            toggleWordWrapAction->setIconVisibleInMenu(mode != 0);
        }
    }
}

void MainWindow::SaveSettings() {
    CreateAppConfigDirectory();

    if (!AppConfigDirExists()) {
        auto msg = "Could not save settings, app config directory not found.";
        LogMessage(msg);
        return;
    }

    // Create settings file.
    QFile file(GetConfigPath() + "/" + SETTINGS_FILE_NAME);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
        auto msg = "Could not save settings.";
        LogMessage(msg);
        return;
    }

    // Collect Settings
    settings = QJsonObject();

    // Viewport
    QJsonObject settings_viewport;
    settings_viewport["multisampling"] = numSamples;
    settings["viewport"] = settings_viewport;

    QJsonDocument jsonDocument(settings);
    file.write(jsonDocument.toJson());
}

void MainWindow::LoadSettings() {
    if (!AppConfigDirExists()) {
        return;
    }

    // Try to load an existing settings file.
    // If not found, ignore and use previously defined default settings.
    QFile file(GetConfigPath() + "/" + SETTINGS_FILE_NAME);

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    // Read settings file content.
    auto content = file.readAll();
    auto jsonDocument = QJsonDocument::fromJson(content);
    settings = jsonDocument.object();

    if (settings.isEmpty()) {
        return;
    }

    // Viewport
    if (settings.find("viewport") != settings.end()) {
        auto settings_viewport = settings.find("viewport")->toObject();

        if (settings_viewport.contains("multisampling")) {
            numSamples = settings_viewport["multisampling"].toInt();
        }
    }
}
