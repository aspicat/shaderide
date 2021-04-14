/**
 * MainWindow Class
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2019 - 2021 Florian Roth (The Damn Coder)
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
#include <QDragEnterEvent>
#include <QMimeData>
#include <QStandardPaths>
#include <QJsonDocument>
#include "MainWindow.hpp"
#include "StyleSheets.hpp"
#include "src/Core/Memory.hpp"
#include <src/Core/QtUtility.hpp>
#include "src/Project/ProjectException.hpp"

#ifdef SHADERIDE_SPIRV_EXPORT
#include "src/Core/Shell.hpp"
#endif

using namespace ShaderIDE::GUI;

MainWindow::MainWindow(QWidget* parent)
        : QMainWindow(parent)
{
    setAcceptDrops(true);
    LoadApplicationSettings();

    // Apply default surface format for Anti-Aliasing.
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(applicationSettings.numSamples);

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

    // Load Config
    LoadConfig();

    OnUpdateStatusBarMessage("Ready.");
}

MainWindow::~MainWindow()
{
    // Project
    Memory::Release(shaderProject);

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
    Memory::Release(resetLayoutAction);
    Memory::Release(swapLayoutAction);
    Memory::Release(viewMenu);

    // File Menu
    Memory::Release(exitAction);
    Memory::Release(settingsAction);
    Memory::Release(exportSPIRVAction);
    Memory::Release(exportShadersAction);
    Memory::Release(saveProjectAsAction);
    Memory::Release(saveProjectAction);
    Memory::Release(openProjectAction);
    Memory::Release(newProjectAction);
    Memory::Release(fileMenu);

    // Widgets
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

ApplicationSettings MainWindow::GetApplicationSettings()
{
    return applicationSettings;
}

void MainWindow::OnVSCodeChanged(const QString& code)
{
    openGLWidget->SetVertexShaderSource(code);
    shaderProject->SetVertexShaderSource(code);
}

void MainWindow::OnFSCodeChanged(const QString& code)
{
    openGLWidget->SetFragmentShaderSource(code);
    shaderProject->SetFragmentShaderSource(code);
}

void MainWindow::OnTextureBrowserImageChanged(TextureBrowserImage* image)
{
    auto slot = OpenGLWidget::FindSlotByName(image->Name());
    openGLWidget->ApplyTextureToSlot(image->ImageHQ(), slot);

    const auto data = QtUtility::MakeBase64FromImage(image->ImageHQ());
    shaderProject->SetTextureData(image->Name(), data);

    OnUpdateStatusBarMessage(QString("Texture \"") + image->Name() + "\" changed.");
}

void MainWindow::OnTextureBrowserImageCleared(TextureBrowserImage* image)
{
    auto slot = OpenGLWidget::FindSlotByName(image->Name());
    openGLWidget->ClearTextureSlot(slot);
    shaderProject->ClearTextureData(image->Name());
    OnUpdateStatusBarMessage(QString("Texture \"") + image->Name() + "\" cleared.");
}

void MainWindow::OnGLInitialized()
{
    // Fetch all textures from File Tab Widget's texture browser
    // and apply these to the OpenGLWidget texture slots.
    ApplyTextureSlots();

    // Apply default code snippets for shaders,
    // after GL is initialized in OpenGLWidget.
    // If we apply the sources earlier, we might
    // have a segmentation fault for glShaderSource(...),
    // since the shader was not yet initialized.
    if (fileTabWidget->CodeEditorsReady())
    {
        fileTabWidget->SetVertexShaderSource(GLSL_DEFAULT_VS_SOURCE);
        fileTabWidget->SetFragmentShaderSource(GLSL_DEFAULT_FS_SOURCE);

        // Only compile at first initialization if realtime compilation
        // is not enabled, otherwise this step is redundant.
        if (!openGLWidget->RealtimeCompilation()) {
            openGLWidget->OnCompileShaders();
        }
    }
}

void MainWindow::OnCompileSuccess(const QString& message)
{
    fileTabWidget->GetVSCodeEditor()->ResetErrorLines();
    fileTabWidget->GetFSCodeEditor()->ResetErrorLines();
    logOutputWidget->LogSuccessMessage(message);
    OnUpdateStatusBarMessage(message);
}

void MainWindow::OnCompileError(GLSLCompileError& error)
{
    auto errorMessage = logOutputWidget->LogGLSLError(error);
    OnUpdateStatusBarMessage(errorMessage);

    // TODO Stylesheet.
    auto highlightColor = QColor("#7A1F2F");

    if (error.GetShaderType() == ShaderType::VertexShader)
    {
        auto* vsCodeEditor = fileTabWidget->GetVSCodeEditor();
        vsCodeEditor->HighlightErrorLine(error.Line(), highlightColor);
        fileTabWidget->setCurrentWidget(vsCodeEditor);
    }

    if (error.GetShaderType() == ShaderType::FragmentShader)
    {
        auto* fsCodeEditor = fileTabWidget->GetFSCodeEditor();
        fsCodeEditor->HighlightErrorLine(error.Line(), highlightColor);
        fileTabWidget->setCurrentWidget(fsCodeEditor);
    }
}

void MainWindow::OnGeneralError(const QString& error)
{
    logOutputWidget->LogErrorMessage(error);
    OnUpdateStatusBarMessage(error);
}

void MainWindow::OnGeneralError(const GeneralException& e)
{
    logOutputWidget->LogErrorMessage(e.what());
    OnUpdateStatusBarMessage(e.what());
}

void MainWindow::OnUpdateStatusBarMessage(const QString& message)
{
    statusBar->showMessage(message, SHADERIDE_STATUSBAR_TIMEOUT);
}

void MainWindow::OnMenuFileNewProject()
{
    auto mbButton = QMessageBox::question(
            nullptr,
            "New Project",
            "All unsaved changes will be lost.\n\nCreate new project?"
    );

    if (mbButton == QMessageBox::Button::Yes)
    {
        ResetProject();
        OnUpdateStatusBarMessage("New project created.");
    }
}

void MainWindow::OnMenuFileOpenProject()
{
    QString path = QFileDialog::getOpenFileName(
            this,
            "Open Project...",
            lastShaderProjectOpenPath,
            QString("Shader IDE Files (") + SHADERIDE_PROJECT_EXTENSION + ")"
    );

    OpenProject(path);

    // Store last project path to cache.
    if (!path.isEmpty()) {
        lastShaderProjectOpenPath = QFileInfo(path).absolutePath();
    }
}

void MainWindow::OnMenuFileSaveProject()
{
    try
    {
        shaderProject->Save();
        OnMenuCodeCompile();
        OnUpdateStatusBarMessage("Shader project saved.");
    }
    catch (ProjectException& e)
    {
        if (e.Code() == ProjectException::ExceptionCode::PATH_EMPTY)
        {
            OnMenuFileSaveProjectAs();
            return;
        }

        logOutputWidget->LogErrorMessage(QString(e.what()));
    }
}

void MainWindow::OnMenuFileSaveProjectAs()
{
    QString path = QFileDialog::getSaveFileName(
            this,
            "Save Project as...",
            QString(),
            QString("Shader IDE Files (") + SHADERIDE_PROJECT_EXTENSION + ")"
    );

    if (path.isEmpty()) {
        return;
    }

    try
    {
        shaderProject->SetPath(path);
        shaderProject->Save();
        ApplyUIFromProject();

        OnMenuCodeCompile();
        OnUpdateStatusBarMessage(QString("Shader project saved to ") + path);
    }
    catch (ProjectException& e) {
        logOutputWidget->LogErrorMessage(e.what());
    }
}

void MainWindow::OnMenuFileExportShaders()
{
    auto directoryURL = QFileDialog::getExistingDirectoryUrl();

    if (!directoryURL.isEmpty()) {
        ExportShaders(directoryURL);
    }
}

void MainWindow::OnMenuFileExportSPIRV()
{
    auto directoryURL = QFileDialog::getExistingDirectoryUrl();

    if (!directoryURL.isEmpty()) {
        ExportSPIRV(directoryURL);
    }
}

void MainWindow::OnMenuFileSettings()
{
    settingsDialog->show();
}

void MainWindow::OnMenuFileExit()
{
    // Project changes unsaved.
    if (!shaderProject->Saved())
    {
        auto mbButton = QMessageBox::question(
                nullptr,
                "Exit",
                "All unsaved changes will be lost.\n\nSave changes?",
                QMessageBox::StandardButtons(
                        QMessageBox::Yes |
                        QMessageBox::No |
                        QMessageBox::Cancel
                )
        );

        if (mbButton == QMessageBox::Yes) {
            OnMenuFileSaveProject();
        }

        if (mbButton == QMessageBox::Cancel) {
            return;
        }
    }

    // No changes. Wanna quit?
    else
    {
        auto mbButton = QMessageBox::question(
                nullptr,
                "Exit",
                "Do you really want to quit?",
                QMessageBox::StandardButtons(
                        QMessageBox::Yes |
                        QMessageBox::No
                )
        );

        if (mbButton == QMessageBox::No) {
            return;
        }
    }

    // No:

    SaveConfig();

    // Disable realtime compilation before closing, to
    // avoid another compilation step after the code editors
    // were cleared by Qt. This caused an error in the
    // log output widget, since the editors trigger the
    // compilation process with some log output, but the log
    // widget is destroyed first (see Destructor).
    openGLWidget->SetRealtimeCompilationEnabled(false);

    exit(0);
}

void MainWindow::OnMenuViewSwapLayout()
{
    SwapLayout();
}

void MainWindow::OnMenuViewResetLayout()
{
    ResetLayout();
}

void MainWindow::OnMenuViewToggleLog()
{
    logOutputWidget->setVisible(!logOutputWidget->isVisible());
    toggleLogAction->setIconVisibleInMenu(logOutputWidget->isVisible());
}

void MainWindow::OnMenuCodeCompile()
{
    fileTabWidget->GetVSCodeEditor()->ResetErrorLines();
    fileTabWidget->GetFSCodeEditor()->ResetErrorLines();
    openGLWidget->OnCompileShaders();
}

void MainWindow::OnMenuCodeToggleRealtimeCompilation()
{
    openGLWidget->ToggleRealtimeCompilation();
    toggleRealtimeCompilationAction->setIconVisibleInMenu(openGLWidget->RealtimeCompilation());
}

void MainWindow::OnMenuCodeToggleWordWrap()
{
    fileTabWidget->ToggleWordWrap();
    toggleWordWrapAction->setIconVisibleInMenu(fileTabWidget->WordWrap());
}

void MainWindow::OnMenuHelpAbout()
{
    aboutDialog->show();
}

void MainWindow::OnOpenGLWidgetMeshSelected(const QString& meshName)
{
    // Ignore "Plane" mesh, which is only used by "Plane 2D" mode.
    if (meshName != "Plane") {
        shaderProject->SetMeshName(meshName);
    }
}

void MainWindow::OnOpenGLWidgetRealtimeToggled(const bool& realtimeActive)
{
    shaderProject->SetRealtime(realtimeActive);
}

void MainWindow::OnOpenGLWidgetPlane2DToggled(const bool& plane2DActive)
{
    shaderProject->SetPlane2D(plane2DActive);
}

void MainWindow::OnOpenGLWidgetModelRotationChanged(const glm::vec3& modelRotation)
{
    shaderProject->SetModelRotation(modelRotation);
}

void MainWindow::OnOpenGLWidgetCameraPositionChanged(const glm::vec3& cameraPosition)
{
    shaderProject->SetCameraPosition(cameraPosition);
}

void MainWindow::OnShaderProjectMarkSaved()
{
    UpdateWindowTitle();
}

void MainWindow::OnShaderProjectMarkUnsaved()
{
    UpdateWindowTitle();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QList<QUrl> urls = mimeData->urls();

        // Only load one project file.
        if (!urls.empty()) {
            OpenProject(urls.at(0).toLocalFile());
        }
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    event->ignore();
    OnMenuFileExit();
}

QString MainWindow::GetConfigPath()
{
    // Linux / UNIX
#ifndef WIN32
    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/ShaderIDE";
#endif

    return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
}

bool MainWindow::AppConfigDirExists()
{
    return QDir(GetConfigPath()).exists();
}

void MainWindow::InitLayout()
{
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

void MainWindow::InitMenuBar()
{
    menuBar = new QMenuBar(this);
    menuBar->setStyleSheet(STYLE_MENUBAR);
    setMenuBar(menuBar);

    InitMenuFile();
    InitMenuView();
    InitMenuCode();
    InitMenuHelp();
}

void MainWindow::InitMenuFile()
{
    fileMenu = new QMenu("&File");
    menuBar->addMenu(fileMenu);

    newProjectAction = new QAction("New Project");
    openProjectAction = new QAction("Open Project...");
    saveProjectAction = new QAction("Save Project");
    saveProjectAsAction = new QAction("Save Project as...");
    exportShadersAction = new QAction("Export Shaders...");

#ifdef SHADERIDE_SPIRV_EXPORT
    exportSPIRVAction   = new QAction("Export SPIR-V...");
#endif

    settingsAction = new QAction("Settings...");
    exitAction = new QAction("Exit");

    saveProjectAction->setShortcut(QKeySequence::Save);

    fileMenu->addAction(newProjectAction);
    fileMenu->addAction(openProjectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(saveProjectAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportShadersAction);

#ifdef SHADERIDE_SPIRV_EXPORT
    fileMenu->addAction(exportSPIRVAction);
#endif

    fileMenu->addSeparator();
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    connect(newProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileNewProject()));

    connect(openProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileOpenProject()));

    connect(saveProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileSaveProject()));

    connect(saveProjectAsAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileSaveProjectAs()));

    connect(exportShadersAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileExportShaders()));

#ifdef SHADERIDE_SPIRV_EXPORT
    connect(exportSPIRVAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileExportSPIRV()));
#endif

    connect(settingsAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileSettings()));

    connect(exitAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuFileExit()));
}

void MainWindow::InitMenuView()
{
    viewMenu = new QMenu("&View");
    menuBar->addMenu(viewMenu);

    swapLayoutAction = new QAction("Swap Layout");
    resetLayoutAction = new QAction("Reset Layout");

    toggleLogAction = new QAction("Toggle Log");
    toggleLogAction->setIcon(QIcon(":/icons/icon-menu-check.png"));
    toggleLogAction->setIconVisibleInMenu(false);
    toggleLogAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));

    viewMenu->addAction(swapLayoutAction);
    viewMenu->addAction(resetLayoutAction);
    viewMenu->addSeparator();
    viewMenu->addAction(toggleLogAction);

    connect(swapLayoutAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuViewSwapLayout()));

    connect(resetLayoutAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuViewResetLayout()));

    connect(toggleLogAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuViewToggleLog()));
}

void MainWindow::InitMenuCode()
{
    codeMenu = new QMenu("&Code");
    menuBar->addMenu(codeMenu);

    // Compile
    compileCodeAction = new QAction("Compile");
    compileCodeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));

    // Realtime Compilation
    toggleRealtimeCompilationAction = new QAction("Toggle Realtime Compilation");
    toggleRealtimeCompilationAction->setIcon(QIcon(":/icons/icon-menu-check.png"));
    toggleRealtimeCompilationAction->setIconVisibleInMenu(false);
    toggleRealtimeCompilationAction->setShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_R));

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
            this, SLOT(OnMenuCodeCompile()));

    connect(toggleRealtimeCompilationAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuCodeToggleRealtimeCompilation()));

    connect(toggleWordWrapAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuCodeToggleWordWrap()));
}

void MainWindow::InitMenuHelp()
{
    helpMenu = new QMenu("&Help");
    menuBar->addMenu(helpMenu);

    aboutAction = new QAction("About");
    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered(bool)),
            this, SLOT(OnMenuHelpAbout()));
}

void MainWindow::InitOpenGLWidget()
{
    openGLWidget = new OpenGLWidget(mainSplitter);
    openGLWidget->setMinimumWidth(400);
    openGLWidget->resize(800, openGLWidget->height());
    mainSplitter->addWidget(openGLWidget);
    mainSplitter->setCollapsible(mainSplitter->indexOf(openGLWidget), false);

    connect(openGLWidget, SIGNAL(NotifyGLInitialized()),
            this, SLOT(OnGLInitialized()));

    connect(openGLWidget, SIGNAL(NotifyCompileSuccess(const QString&)),
            this, SLOT(OnCompileSuccess(const QString&)));

    connect(openGLWidget, SIGNAL(NotifyCompileError(GLSLCompileError & )),
            this, SLOT(OnCompileError(GLSLCompileError & )));

    connect(openGLWidget, SIGNAL(NotifyStateUpdated(const QString&)),
            this, SLOT(OnUpdateStatusBarMessage(const QString&)));

    connect(openGLWidget, SIGNAL(NotifyGeneralError(const GeneralException&)),
            this, SLOT(OnGeneralError(const GeneralException&)));

    connect(openGLWidget, SIGNAL(NotifyMeshSelected(const QString&)),
            this, SLOT(OnOpenGLWidgetMeshSelected(const QString&)));

    connect(openGLWidget, SIGNAL(NotifyRealtimeToggled(const bool&)),
            this, SLOT(OnOpenGLWidgetRealtimeToggled(const bool&)));

    connect(openGLWidget, SIGNAL(NotifyPlane2DToggled(const bool&)),
            this, SLOT(OnOpenGLWidgetPlane2DToggled(const bool&)));

    connect(openGLWidget, SIGNAL(NotifyModelRotationChanged(const glm::vec3&)),
            this, SLOT(OnOpenGLWidgetModelRotationChanged(const glm::vec3&)));

    connect(openGLWidget, SIGNAL(NotifyCameraPositionChanged(const glm::vec3&)),
            this, SLOT(OnOpenGLWidgetCameraPositionChanged(const glm::vec3&)));
}

void MainWindow::InitFileTabWidget()
{
    // File Tab Widget
    fileTabWidget = new FileTabWidget();
    mainSplitter->addWidget(fileTabWidget);

    // Disable Collapsing
    auto ftwIndex = mainSplitter->indexOf(fileTabWidget);
    mainSplitter->setCollapsible(ftwIndex, false);
    verticalSplitter->setStretchFactor(ftwIndex, 1);

    connect(fileTabWidget, SIGNAL(NotifyVSCodeChanged(const QString&)),
            this, SLOT(OnVSCodeChanged(const QString&)));

    connect(fileTabWidget, SIGNAL(NotifyFSCodeChanged(const QString&)),
            this, SLOT(OnFSCodeChanged(const QString&)));

    connect(fileTabWidget->GetTextureBrowser(), SIGNAL(NotifyImageChanged(TextureBrowserImage*)),
            this, SLOT(OnTextureBrowserImageChanged(TextureBrowserImage *)));

    connect(fileTabWidget->GetTextureBrowser(), SIGNAL(NotifyImageCleared(TextureBrowserImage*)),
            this, SLOT(OnTextureBrowserImageCleared(TextureBrowserImage *)));
}

void MainWindow::InitLogOutputWidget()
{
    logOutputWidget = new LogOutputWidget();
    verticalSplitter->addWidget(logOutputWidget);
    verticalSplitter->setCollapsible(verticalSplitter->indexOf(logOutputWidget), false);
    verticalSplitter->setSizes(QList<int>({ height(), 120 }));
    toggleLogAction->setIconVisibleInMenu(logOutputWidget->isVisible());
}

void MainWindow::InitSettingsDialog()
{
    settingsDialog = new SettingsDialog(this);
}

void MainWindow::InitAboutDialog()
{
    aboutDialog = new AboutDialog();
}

void MainWindow::InitStatusBar()
{
    statusBar = new QStatusBar();
    statusBar->setStyleSheet(STYLE_STATUSBAR);
    verticalLayout->addWidget(statusBar);
}

void MainWindow::InitShaderProject()
{
    shaderProject = new ShaderProject("");
    ConnectShaderProjectSignals();

    shaderProject->SetTextureData(GLSL_TEXTURE_SLOT_0_NAME, "");
    shaderProject->SetTextureData(GLSL_TEXTURE_SLOT_1_NAME, "");
    shaderProject->SetTextureData(GLSL_TEXTURE_SLOT_2_NAME, "");
    shaderProject->SetTextureData(GLSL_TEXTURE_SLOT_3_NAME, "");
}

void MainWindow::SwapLayout()
{
    // Hide (remove) widgets from splitter.
    fileTabWidget->hide();
    openGLWidget->hide();

    // Store Width
    auto w0_width = mainSplitter->widget(0)->width();
    auto w1_width = mainSplitter->widget(1)->width();

    // We use bit negation to swap the index of 0 and 1.
    // Not a perfect solution right here, but we cannot simply
    // swap widgets inside a splitter. If we have more than two
    // widgets in future versions (i.e. a sidebar), we need a
    // more complex solution of course.
    auto openGLWidgetIndex = mainSplitter->indexOf(openGLWidget);
    mainSplitter->insertWidget(~openGLWidgetIndex, openGLWidget);
    mainSplitter->insertWidget(openGLWidgetIndex, fileTabWidget);

    // Swap width values for widgets from stored sizes.
    // For some reason after swapping the widgets, the size
    // is reset to either 400.
    auto* w0 = mainSplitter->widget(0);
    auto* w1 = mainSplitter->widget(1);
    w0->resize(w1_width, w0->height());
    w1->resize(w0_width, w1->height());

    fileTabWidget->show();
    openGLWidget->show();
}

void MainWindow::ResetLayout()
{
    // Reset Size
    mainSplitter->setSizes(QList<int>() << 1 << 1);

    // This is just a quick solution, since there are only
    // two widgets inside the splitter. See SwapLayout().
    if (mainSplitter->indexOf(openGLWidget) == 1) {
        SwapLayout();
    }

    logOutputWidget->setVisible(true);
    toggleLogAction->setIconVisibleInMenu(logOutputWidget->isVisible());
}

void MainWindow::UpdateWindowTitle()
{
    QString windowTitle = SHADERIDE_APPLICATION_NAME;

    // TODO -> Future Versions -> Project Name
    if (shaderProject != nullptr)
    {
        if (!shaderProject->Path().isEmpty())
        {
            windowTitle.append(" - ").append(shaderProject->Path());

            if (!shaderProject->Saved()) {
                windowTitle.append(" *");
            }
        }
    }

    setWindowTitle(windowTitle);
}

void MainWindow::ApplyTextureSlots()
{
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

void MainWindow::LogMessage(const QString& message)
{
    logOutputWidget->LogMessage(message);
    OnUpdateStatusBarMessage(message);
}

void MainWindow::ResetProject()
{
    Memory::Release(shaderProject);

    shaderProject = new ShaderProject("");
    ConnectShaderProjectSignals();

    shaderProject->SetVertexShaderSource(GLSL_DEFAULT_VS_SOURCE);
    shaderProject->SetFragmentShaderSource(GLSL_DEFAULT_FS_SOURCE);

    openGLWidget->ResetUI();
    fileTabWidget->ResetUI();

    ApplyUIFromProject();
}

void MainWindow::OpenProject(const QString& path)
{
    if (path.isEmpty()) {
        return;
    }

    fileTabWidget->GetTextureBrowser()->ClearImages();

    try
    {
        shaderProject = ShaderProject::Load(path);
        ConnectShaderProjectSignals();

        if (ApplyUIFromProject()) {
            OnUpdateStatusBarMessage(QString("Shader project loaded: ") + path);
        }

        MarkProjectSavedDelayed();
    }
    catch (GeneralException& e)
    {
        OnGeneralError(e);
        return;
    }
}

void MainWindow::MarkProjectSavedDelayed()
{
    // TODO Cheapfix: Mark project as saved when loaded.
    // Check roughly after one second when all signals were fired
    // by the code editors etc., which modify the project again by
    // invalidating the save state flag from setters.
    QTimer::singleShot(1000, [&] () {
        shaderProject->MarkSaved();
    });
}

bool MainWindow::ApplyUIFromProject()
{
    if (shaderProject == nullptr)
    {
        auto msg = "Invalid shader project pointer. Could not apply UI from project data.";
        logOutputWidget->LogErrorMessage(msg);
        OnUpdateStatusBarMessage(msg);
        return false;
    }

    fileTabWidget->SetVertexShaderSource(shaderProject->VertexShaderSource());
    fileTabWidget->SetFragmentShaderSource(shaderProject->FragmentShaderSource());

    openGLWidget->CheckPlane2D(shaderProject->Plane2D());

    openGLWidget->OnCompileShaders();
    openGLWidget->SelectMesh(shaderProject->MeshName());

    for (auto& texture : shaderProject->TextureData())
    {
        auto data = texture.second;

        if (data.isEmpty()) {
            continue;
        }

        const auto slotName = texture.first;
        const auto slot = OpenGLWidget::FindSlotByName(slotName);
        const auto image = QtUtility::MakeQImageFromBase64(data);

        openGLWidget->ApplyTextureToSlot(image, slot);
        fileTabWidget->GetTextureBrowser()->GetImage(slotName)->UpdateImage(image);
    }

    openGLWidget->CheckRealtime(shaderProject->Realtime());
    openGLWidget->RotateModel(shaderProject->ModelRotation());
    openGLWidget->MoveCamera(shaderProject->CameraPosition());
    openGLWidget->repaint();

    shaderProject->MarkSaved();

    UpdateWindowTitle();

    return true;
}

void MainWindow::ExportShaders(const QUrl& directory)
{
    auto vsFilePath = MakeVSPath(directory);
    auto fsFilePath = MakeFSPath(directory);
    auto errorFmt = "Could not create shader file \"%s\".";

    // Vertex Shader File
    QFile vsFile(vsFilePath);
    vsFile.open(QIODevice::OpenModeFlag::WriteOnly);

    if (vsFile.error() != QFileDevice::NoError) {
        logOutputWidget->LogErrorMessage(QString::asprintf(errorFmt, vsFilePath.toStdString().c_str()));
    }

    if (vsFile.isOpen())
    {
        vsFile.write(fileTabWidget->VertexShaderSource().toUtf8());
        vsFile.close();
    }

    // Fragment Shader File
    QFile fsFile(fsFilePath);
    fsFile.open(QIODevice::OpenModeFlag::WriteOnly);

    if (fsFile.error() != QFileDevice::NoError) {
        logOutputWidget->LogErrorMessage(QString::asprintf(errorFmt, fsFilePath.toStdString().c_str()));
    }

    if (fsFile.isOpen())
    {
        fsFile.write(fileTabWidget->FragmentShaderSource().toUtf8());
        fsFile.close();
    }
}

void MainWindow::ExportSPIRV(const QUrl& directory)
{
#ifdef SHADERIDE_SPIRV_EXPORT
    // Export shader files first.
    ExportShaders(directory);

    auto vsFilePath = MakeVSPath(directory).toStdString();
    auto fsFilePath = MakeFSPath(directory).toStdString();

    // TODO Move "glslangValidator" path to settings.
    const auto cmd = "glslangValidator --auto-map-bindings --auto-map-locations -V ";
    const auto vsOutput = Shell::Exec(QString(cmd) + vsFilePath);
    const auto fsOutput = Shell::Exec(QString(cmd) + fsFilePath);

    logOutputWidget->LogMessage(vsOutput.c_str());
    logOutputWidget->LogMessage(fsOutput.c_str());

    remove(vsFilePath.c_str());
    remove(fsFilePath.c_str());

    const auto errorTag = "ERROR:";

    if (vsOutput.find(errorTag) != QString::npos ||
        fsOutput.find(errorTag) != QString::npos) {
        OnGeneralError("Could not export SPIR-V shader binaries. See log above.");
    }
#endif
}

void MainWindow::ConnectShaderProjectSignals()
{
    connect(shaderProject, SIGNAL(NotifyMarkSaved()),
            this, SLOT(OnShaderProjectMarkSaved()));

    connect(shaderProject, SIGNAL(NotifyMarkUnsaved()),
            this, SLOT(OnShaderProjectMarkUnsaved()));
}

QString MainWindow::MakeVSPath(const QUrl& directory)
{
    // TODO Use [PROJECT_ID].vert as shader file name.
    return directory.toLocalFile().append("/shader.vert");
}

QString MainWindow::MakeFSPath(const QUrl& directory)
{
    // TODO Use [PROJECT_ID].frag as shader file name.
    return directory.toLocalFile().append("/shader.frag");
}

void MainWindow::CreateAppConfigDirectory()
{
    if (AppConfigDirExists()) {
        return;
    }

    QDir().mkpath(GetConfigPath());

    if (!AppConfigDirExists())
    {
        auto msg = "Could not create config directory \"" + GetConfigPath() + "\".";
        LogMessage(msg);
    }
}

void MainWindow::SaveConfig()
{
    CreateAppConfigDirectory();

    if (!AppConfigDirExists())
    {
        auto msg = "Could not save config, application config directory not found.";
        LogMessage(msg);
        return;
    }

    // Create layout config file.
    QFile file(GetConfigPath() + "/" + CONFIG_FILE_NAME);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        auto msg = "Could not save config file.";
        LogMessage(msg);
        return;
    }

    // Collect Variables
    config = QJsonObject();

    // Layout
    QJsonObject config_layout;
    config_layout["swapped"] = mainSplitter->indexOf(openGLWidget) == 1; // See SwapLayout();
    config_layout["width_left"] = mainSplitter->widget(0)->width(); // See SwapLayout();
    config_layout["width_right"] = mainSplitter->widget(1)->width(); // See SwapLayout();
    config_layout["show_log"] = logOutputWidget->isVisible();
    config["layout"] = config_layout;

    // Window
    QJsonObject config_window;
    config_window["maximized"] = isMaximized();
    config_window["width"] = width();
    config_window["height"] = height();
    config["window"] = config_window;

    // Code
    QJsonObject config_code;
    config_code["word_wrap"] = fileTabWidget->WordWrapMode();
    config_code["realtime_compilation"] = openGLWidget->RealtimeCompilation();
    config["code"] = config_code;

    // Project
    QJsonObject config_project;
    config_project["last_open_path"] = lastShaderProjectOpenPath;
    config["project"] = config_project;

    QJsonDocument jsonDocument(config);
    file.write(jsonDocument.toJson());
}

void MainWindow::LoadConfig()
{
    if (!AppConfigDirExists()) {
        return;
    }

    QFile file(GetConfigPath() + "/" + CONFIG_FILE_NAME);

    if (!file.open(QIODevice::ReadOnly))
    {
        auto msg = "Could not load config file.";
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
    if (config.find("layout") != config.end())
    {
        auto config_layout = config.find("layout")->toObject();

        // Swapped
        if (config_layout.contains("swapped") && config_layout["swapped"].toBool()) {
            SwapLayout();
        }

        // Width Left
        if (config_layout.contains("width_left")) {
            mainSplitter->widget(0)->resize(config_layout["width_left"].toInt(), mainSplitter->height());
        }

        // Width Right
        if (config_layout.contains("width_right")) {
            mainSplitter->widget(1)->resize(config_layout["width_right"].toInt(), mainSplitter->height());
        }

        // Show Log
        if (config_layout.contains("show_log"))
        {
            auto show_log = config_layout["show_log"].toBool();
            logOutputWidget->setVisible(show_log);
            toggleLogAction->setIconVisibleInMenu(show_log);
        }
    }

    // Window
    if (config.find("window") != config.end())
    {
        auto config_window = config.find("window")->toObject();

        // Maximized?
        if (config_window.contains("maximized") && config_window["maximized"].toBool()) {
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
    if (config.find("code") != config.end())
    {
        auto config_code = config.find("code")->toObject();

        // Realtime Compilation
        if (config_code.contains("realtime_compilation"))
        {
            auto rtcEnabled = config_code["realtime_compilation"].toBool();
            openGLWidget->SetRealtimeCompilationEnabled(rtcEnabled);
            toggleRealtimeCompilationAction->setIconVisibleInMenu(rtcEnabled);
        }

        // Word Wrap
        if (config_code.contains("word_wrap"))
        {
            auto mode = static_cast<QTextOption::WrapMode>(config_code["word_wrap"].toInt());
            fileTabWidget->SetWordWrapMode(mode);
            toggleWordWrapAction->setIconVisibleInMenu(mode != 0);
        }
    }

    // Project
    if (config.find("project") != config.end())
    {
        auto config_project = config.find("project")->toObject();

        // Last Path
        if (config_project.contains("last_open_path")) {
            lastShaderProjectOpenPath = config_project["last_open_path"].toString();
        }
    }
}

void MainWindow::SaveApplicationSettings()
{
    CreateAppConfigDirectory();

    if (!AppConfigDirExists())
    {
        auto msg = "Could not save settings, app config directory not found.";
        LogMessage(msg);
        return;
    }

    // Create settings file.
    QFile file(GetConfigPath() + "/" + SETTINGS_FILE_NAME);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text))
    {
        auto msg = "Could not save settings.";
        LogMessage(msg);
        return;
    }

    // Collect Settings
    settings = QJsonObject();

    // 3D Viewport
    QJsonObject settings_viewport;
    settings_viewport["multisampling"] = applicationSettings.numSamples;
    settings["viewport"] = settings_viewport;

    // Code Editor
    QJsonObject settings_code_editor;
    settings_code_editor["tab_width"] = applicationSettings.tabWidth;
    settings["code_editor"] = settings_code_editor;

    QJsonDocument jsonDocument(settings);
    file.write(jsonDocument.toJson());
}

void MainWindow::LoadApplicationSettings()
{
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

    // 3D Viewport
    if (settings.find("viewport") != settings.end())
    {
        auto settings_viewport = settings.find("viewport")->toObject();

        if (settings_viewport.contains("multisampling")) {
            applicationSettings.numSamples = settings_viewport["multisampling"].toInt();
        }
    }

    // Code Editor
    if (settings.find("code_editor") != settings.end())
    {
        auto settings_code_editor = settings.find("code_editor")->toObject();

        if (settings_code_editor.contains("tab_width")) {
            applicationSettings.tabWidth = settings_code_editor["tab_width"].toInt();
        }
    }
}
