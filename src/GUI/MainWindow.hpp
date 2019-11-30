/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * MainWindow Class
 */

#ifndef SHADERIDE_GUI_MAINWINDOW_HPP
#define SHADERIDE_GUI_MAINWINDOW_HPP

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QStatusBar>
#include "OpenGLWidget.hpp"
#include "LogOutputWidget.hpp"
#include "Widgets/FileTabWidget.hpp"
#include "src/GL/Shader.hpp"
#include "src/Project/ShaderProject.hpp"

using namespace ShaderIDE::GL;
using namespace ShaderIDE::Project;

namespace ShaderIDE::GUI {

    class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;

    private slots:
        void sl_VSCodeChanged(const QString &code);
        void sl_FSCodeChanged(const QString &code);
        void sl_TextureBrowserImageChanged(TextureBrowserImage *image);
        void sl_TextureBrowserImageCleared(TextureBrowserImage *image);
        void sl_TextureBrowserImagePathChanged(TextureBrowserImage *image);
        void sl_GLInitialized();
        void sl_CompileSuccess(const QString &message);
        void sl_CompileError(GLSLCompileError &error);
        void sl_GeneralError(const GeneralException &e);
        void sl_UpdateStatusBarMessage(const QString &message);

        // Menu / File
        void sl_FileNewProject();
        void sl_FileOpenProject();
        void sl_FileSaveProject();
        void sl_FileSaveProjectAs();
        void sl_FileExportShaders();

    private:
        QWidget *centralWidget;
        QVBoxLayout *verticalLayout;
        QSplitter *verticalSplitter;
        QSplitter *mainSplitter;
        QMenuBar *menuBar;
        OpenGLWidget *openGLWidget;
        FileTabWidget *fileTabWidget;
        LogOutputWidget *logOutputWidget;
        QStatusBar *statusBar;

        // Menu
        QMenu *fileMenu;
        QAction *newProjectAction;
        QAction *openProjectAction;
        QAction *saveProjectAction;
        QAction *saveProjectAsAction;
        QAction *exportShadersAction;

        // Project
        ShaderProject *shaderProject;

        void InitLayout();
        void InitMenuBar();
        void InitOpenGLWidget();
        void InitFileTabWidget();
        void InitLogOutputWidget();
        void InitStatusBar();
        void InitShaderProject();

        void DestroyShaderProject();
        void DestroyMenuBar();
        void DestroyLayout();

        void UpdateWindowTitle();

        void ApplyTextureSlots();

        void ResetProject();
        void ApplyProjectFromUI();
        void ApplyUIFromProject();
        void ExportShaders(const QUrl &directory);
    };
}

#endif // SHADERIDE_GUI_MAINWINDOW_HPP
