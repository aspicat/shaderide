// Copyright (c) 2019 Aspicat - Florian Roth

#include <QDebug>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include "MainWindow.hpp"
#include "StyleSheets.hpp"
#include "src/GL/GLDefaults.hpp"
#include "src/Core/GeneralException.hpp"
#include "src/Core/Memory.hpp"
#include "src/Core/ApplicationDefaults.hpp"

using namespace ShaderIDE::GUI;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow           (parent),
      centralWidget         (nullptr),
      verticalLayout        (nullptr),
      verticalSplitter      (nullptr),
      mainSplitter          (nullptr),
      menuBar               (nullptr),
      openGLWidget          (nullptr),
      fileTabWidget         (nullptr),
      logOutputWidget       (nullptr),
      statusBar             (nullptr),
      fileMenu              (nullptr),
      newProjectAction      (nullptr),
      openProjectAction     (nullptr),
      saveProjectAction     (nullptr),
      saveProjectAsAction   (nullptr),
      exportShadersAction   (nullptr),
      shaderProject         (nullptr)
{
    resize(STYLE_MAINWINDOW_WIDTH, STYLE_MAINWINDOW_HEIGHT);
    setStyleSheet(STYLE_MAINWINDOW);

    InitLayout();
    InitMenuBar();
    InitOpenGLWidget();
    InitFileTabWidget();
    InitLogOutputWidget();
    InitStatusBar();
    InitShaderProject();

    UpdateWindowTitle();

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
        openGLWidget->sl_CompileShaders();
    }
}

void MainWindow::sl_CompileSuccess(const QString &message) {
    logOutputWidget->LogSuccessMessage(message);
}

void MainWindow::sl_CompileError(GLSLCompileError &error) {
    logOutputWidget->LogGLSLError(error);
}

void MainWindow::sl_GeneralError(const GeneralException &e) {
    logOutputWidget->LogErrorMessage(e.what());
}

void MainWindow::sl_UpdateStatusBarMessage(const QString &message) {
    statusBar->showMessage(message, SHADERIDE_STATUSBAR_TIMEOUT);
}

void MainWindow::sl_FileNewProject() {
    auto mbButton = QMessageBox::question(
            nullptr,
            "Reset Active Project?",
            "All current changes will be lost. Create new project?"
    );

    if (mbButton == QMessageBox::Button::Yes) {
        ResetProject();
        sl_UpdateStatusBarMessage("New project created.");
    }
}

void MainWindow::sl_FileOpenProject() {
    QString path = QFileDialog::getOpenFileName(
            this,
            "Open Project...",
            QString(),
            QString("Shader IDE Files (") + SHADERIDE_PROJECT_EXTENSION + ")"
    );

    try {
        Memory::Release(shaderProject);
        shaderProject = ShaderProject::Load(path.toStdString());
        ApplyUIFromProject();

    } catch (GeneralException &e) {
        shaderProject = new ShaderProject("");
        logOutputWidget->LogErrorMessage(e.what());
        // TODO MessageBox?
    }

    sl_UpdateStatusBarMessage(QString("Shader project loaded: ") + path);
}

void MainWindow::sl_FileSaveProject() {
    try {
        ApplyProjectFromUI();
        shaderProject->Save();

    } catch (GeneralException &e) {
        logOutputWidget->LogErrorMessage(e.what());
        // TODO MessageBox?
    }

    sl_UpdateStatusBarMessage(QString("Shader project saved."));
}

void MainWindow::sl_FileSaveProjectAs() {
    QString path = QFileDialog::getSaveFileName(
            this,
            "Save Project as...",
            QString(),
            QString("Shader IDE Files (") + SHADERIDE_PROJECT_EXTENSION + ")"
    );

    try {
        ApplyProjectFromUI();
        shaderProject->SetPath(path.toStdString());
        shaderProject->Save();

    } catch (GeneralException &e) {
        logOutputWidget->LogErrorMessage(e.what());
        // TODO MessageBox?
    }

    sl_UpdateStatusBarMessage(QString("Shader project saved to ") + path);
    saveProjectAction->setDisabled(false);
}

void MainWindow::sl_FileExportShaders() {
    auto directoryURL = QFileDialog::getExistingDirectoryUrl();

    if (!directoryURL.isEmpty()) {
        ExportShaders(directoryURL);
    }
}

void MainWindow::InitLayout() {

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

    // Menu Bar
    menuBar = new QMenuBar(this);
    menuBar->setStyleSheet(STYLE_MENUBAR);
    setMenuBar(menuBar);

    // File Menu
    fileMenu = new QMenu("&File");
    menuBar->addMenu(fileMenu);

    // File Actions
    newProjectAction     = new QAction("New Project");
    openProjectAction    = new QAction("Open Project...");
    saveProjectAction    = new QAction("Save Project");
    saveProjectAsAction  = new QAction("Save Project as...");
    exportShadersAction  = new QAction("Export Shaders...");

    saveProjectAction->setShortcut(QKeySequence::Save);

    fileMenu->addAction(newProjectAction);
    fileMenu->addAction(openProjectAction);
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(saveProjectAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exportShadersAction);

    saveProjectAction->setDisabled(true);

    // File Signals & Slots
    connect(newProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_FileNewProject()));

    connect(openProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_FileOpenProject()));

    connect(saveProjectAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_FileSaveProject()));

    connect(saveProjectAsAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_FileSaveProjectAs()));

    connect(exportShadersAction, SIGNAL(triggered(bool)),
            this, SLOT(sl_FileExportShaders()));
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
    fileTabWidget->setMinimumWidth(400);
    fileTabWidget->resize(800, fileTabWidget->height());
    fileTabWidget->setContentsMargins(0, 0, 0, 0);
    fileTabWidget->setStyleSheet(STYLE_TABWIDGET);
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

    // File Menu
    Memory::Release(exportShadersAction);
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
        logOutputWidget->LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_0_NAME));
    }

    if (slotIt1 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt1.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_1);
    } else {
        logOutputWidget->LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_1_NAME));
    }

    if (slotIt2 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt2.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_2);
    } else {
        logOutputWidget->LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_2_NAME));
    }

    if (slotIt3 != textures.end()) {
        openGLWidget->ApplyTextureToSlot(slotIt3.value()->ImageHQ(), OpenGLWidget::SLOT::TEX_3);
    } else {
        logOutputWidget->LogMessage(QString::asprintf(errorFmt, GLSL_TEXTURE_SLOT_3_NAME));
    }
}

void MainWindow::ResetProject() {
    Memory::Release(shaderProject);

    shaderProject = new ShaderProject("");
    shaderProject->SetVertexShaderSource(GLSL_DEFAULT_VS_SOURCE);
    shaderProject->SetFragmentShaderSource(GLSL_DEFAULT_FS_SOURCE);

    openGLWidget->ResetModelRotation();
    fileTabWidget->GetTextureBrowser()->ClearImages();

    ApplyUIFromProject();
}

void MainWindow::ApplyProjectFromUI() {
    shaderProject->SetVertexShaderSource(
            fileTabWidget->VertexShaderSource().toStdString());

    shaderProject->SetFragmentShaderSource(
            fileTabWidget->FragmentShaderSource().toStdString());

    // TODO Mesh Name
    // TODO Textures

    for (const auto &texture : fileTabWidget->GetTextureBrowser()->Images()) {
        shaderProject->SetTexturePath(
                texture->Name().toStdString(),
                texture->Path().toStdString()
        );
    }
}

void MainWindow::ApplyUIFromProject() {
    if (!shaderProject) {
        logOutputWidget->LogErrorMessage(
                "Could not apply UI from project. Shader project not initialized."
        );

        return;
    }

    saveProjectAction->setDisabled(shaderProject->Path().empty());

    fileTabWidget->SetVertexShaderSource(shaderProject->VertexShaderSource().c_str());
    fileTabWidget->SetFragmentShaderSource(shaderProject->FragmentShaderSource().c_str());

    openGLWidget->sl_CompileShaders();

    // TODO Mesh Name
    // TODO Textures

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

    UpdateWindowTitle();
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
