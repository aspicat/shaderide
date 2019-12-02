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
#include "AboutDialog.hpp"
#include "src/GL/Shader.hpp"
#include "src/Project/ShaderProject.hpp"

using namespace ShaderIDE::GL;
using namespace ShaderIDE::Project;

namespace ShaderIDE::GUI {

    class MainWindow : public QMainWindow {
        Q_OBJECT
        static constexpr const char* CONFIG_FILE_NAME = "config.json";

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
        void sl_Menu_FileNewProject();
        void sl_Menu_FileOpenProject();
        void sl_Menu_FileSaveProject();
        void sl_Menu_FileSaveProjectAs();
        void sl_Menu_FileExportShaders();
        void sl_Menu_FileExit();

        // Menu / View
        void sl_Menu_ViewSwapLayout();
        void sl_Menu_ViewResetLayout();
        void sl_Menu_ViewSaveLayout();
        void sl_Menu_ViewToggleLog();

        // Menu / Code
        void sl_Menu_CodeCompile();

        // Menu / Help
        void sl_Menu_HelpAbout();

    protected:
        void closeEvent(QCloseEvent *event) override;

    private:
        static QString GetConfigPath();
        static bool AppConfigDirExists();

        QJsonObject config;

        QWidget *centralWidget;
        QVBoxLayout *verticalLayout;
        QSplitter *verticalSplitter;
        QSplitter *mainSplitter;
        QMenuBar *menuBar;
        OpenGLWidget *openGLWidget;
        FileTabWidget *fileTabWidget;
        LogOutputWidget *logOutputWidget;
        AboutDialog *aboutDialog;
        QStatusBar *statusBar;

        // File Menu
        QMenu *fileMenu;
        QAction *newProjectAction;
        QAction *openProjectAction;
        QAction *saveProjectAction;
        QAction *saveProjectAsAction;
        QAction *exportShadersAction;
        QAction *exitAction;

        // View Menu
        QMenu *viewMenu;
        QAction *swapLayoutAction;
        QAction *resetLayoutAction;
        QAction *saveLayoutAction;
        QAction *toggleLogAction;

        // Code Menu
        QMenu *codeMenu;
        QAction *compileCodeAction;

        // Help Menu
        QMenu *helpMenu;
        QAction *aboutAction;

        // Project
        ShaderProject *shaderProject;

        // Init
        void InitLayout();
        void InitMenuBar();
        void InitMenuFile();
        void InitMenuView();
        void InitMenuCode();
        void InitMenuHelp();
        void InitOpenGLWidget();
        void InitFileTabWidget();
        void InitLogOutputWidget();
        void InitAboutDialog();
        void InitStatusBar();
        void InitShaderProject();

        // Destroy
        void DestroyShaderProject();
        void DestroyMenuBar();
        void DestroyLayout();

        // UI
        void SwapLayout();
        void ResetLayout();
        void UpdateWindowTitle();
        void ApplyTextureSlots();

        void LogMessage(const QString &message);

        // Project
        void ResetProject();
        void ApplyProjectFromUI();
        void ApplyUIFromProject();
        void ExportShaders(const QUrl &directory);

        // Config
        void CreateAppConfigDirectory();
        void StoreConfig();
        void LoadConfig();
    };
}

#endif // SHADERIDE_GUI_MAINWINDOW_HPP
