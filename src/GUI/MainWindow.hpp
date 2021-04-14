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
#include "src/Core/ApplicationDefaults.hpp"
#include "src/GUI/Dialogs/SettingsDialog.hpp"
#include "src/GUI/Dialogs/AboutDialog.hpp"
#include "src/GL/Shader.hpp"
#include "src/Project/ShaderProject.hpp"

using namespace ShaderIDE::GL;
using namespace ShaderIDE::Project;

namespace ShaderIDE::GUI {

    struct ApplicationSettings
    {
        int numSamples{ SHADERIDE_SURFACEFORMAT_NUM_SAMPLES };
        int tabWidth{ SHADERIDE_CODE_EDITOR_TAB_WIDTH };
    };

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

        friend class SettingsDialog;

        static constexpr const char* CONFIG_FILE_NAME = "config.json";
        static constexpr const char* SETTINGS_FILE_NAME = "settings.json";

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override;

        ApplicationSettings GetApplicationSettings();

    private slots:
        void OnVSCodeChanged(const QString& code);
        void OnFSCodeChanged(const QString& code);
        void OnTextureBrowserImageChanged(TextureBrowserImage* image);
        void OnTextureBrowserImageCleared(TextureBrowserImage* image);
        void OnGLInitialized();
        void OnCompileSuccess(const QString& message);
        void OnCompileError(GLSLCompileError& error);
        void OnGeneralError(const QString& error);
        void OnGeneralError(const GeneralException& e);
        void OnUpdateStatusBarMessage(const QString& message);

        // Menu / File
        void OnMenuFileNewProject();
        void OnMenuFileOpenProject();
        void OnMenuFileSaveProject();
        void OnMenuFileSaveProjectAs();
        void OnMenuFileExportShaders();
        void OnMenuFileExportSPIRV();
        void OnMenuFileSettings();
        void OnMenuFileExit();

        // Menu / View
        void OnMenuViewSwapLayout();
        void OnMenuViewResetLayout();
        void OnMenuViewToggleLog();

        // Menu / Code
        void OnMenuCodeCompile();
        void OnMenuCodeToggleRealtimeCompilation();
        void OnMenuCodeToggleWordWrap();

        // Menu / Help
        void OnMenuHelpAbout();

        // OpenGL Widget
        void OnOpenGLWidgetMeshSelected(const QString& meshName);
        void OnOpenGLWidgetRealtimeToggled(const bool& realtimeActive);
        void OnOpenGLWidgetPlane2DToggled(const bool& plane2DActive);
        void OnOpenGLWidgetModelRotationChanged(const glm::vec3& modelRotation);
        void OnOpenGLWidgetCameraPositionChanged(const glm::vec3& cameraPosition);

        // Project
        void OnShaderProjectMarkSaved();
        void OnShaderProjectMarkUnsaved();

    protected:
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;
        void dragLeaveEvent(QDragLeaveEvent* event) override;
        void dropEvent(QDropEvent* event) override;
        void closeEvent(QCloseEvent* event) override;

    private:
        static QString GetConfigPath();
        static bool AppConfigDirExists();

        // Application Settings
        ApplicationSettings applicationSettings;

        QJsonObject config;
        QJsonObject settings;

        // Widgets
        QWidget* centralWidget{ nullptr };
        QVBoxLayout* verticalLayout{ nullptr };
        QSplitter* verticalSplitter{ nullptr };
        QSplitter* mainSplitter{ nullptr };
        QMenuBar* menuBar{ nullptr };
        OpenGLWidget* openGLWidget{ nullptr };
        FileTabWidget* fileTabWidget{ nullptr };
        LogOutputWidget* logOutputWidget{ nullptr };
        SettingsDialog* settingsDialog{ nullptr };
        AboutDialog* aboutDialog{ nullptr };
        QStatusBar* statusBar{ nullptr };

        // File Menu
        QMenu* fileMenu{ nullptr };
        QAction* newProjectAction{ nullptr };
        QAction* openProjectAction{ nullptr };
        QAction* saveProjectAction{ nullptr };
        QAction* saveProjectAsAction{ nullptr };
        QAction* exportShadersAction{ nullptr };
        QAction* exportSPIRVAction{ nullptr };
        QAction* settingsAction{ nullptr };
        QAction* exitAction{ nullptr };

        // View Menu
        QMenu* viewMenu{ nullptr };
        QAction* swapLayoutAction{ nullptr };
        QAction* resetLayoutAction{ nullptr };
        QAction* toggleLogAction{ nullptr };

        // Code Menu
        QMenu* codeMenu{ nullptr };
        QAction* compileCodeAction{ nullptr };
        QAction* toggleRealtimeCompilationAction{ nullptr };
        QAction* toggleWordWrapAction{ nullptr };

        // Help Menu
        QMenu* helpMenu{ nullptr };
        QAction* aboutAction{ nullptr };

        // Project
        QString lastShaderProjectOpenPath{ "" };
        ShaderProject* shaderProject{ nullptr };

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
        void InitSettingsDialog();
        void InitAboutDialog();
        void InitStatusBar();
        void InitShaderProject();

        // UI
        void SwapLayout();
        void ResetLayout();
        void UpdateWindowTitle();
        void ApplyTextureSlots();

        void LogMessage(const QString& message);

        // Project
        void ResetProject();
        void OpenProject(const QString& path);
        void MarkProjectSavedDelayed();
        bool ApplyUIFromProject();
        void ExportShaders(const QUrl& directory);
        void ExportSPIRV(const QUrl& directory);

        void ConnectShaderProjectSignals();

        QString MakeVSPath(const QUrl& directory);
        QString MakeFSPath(const QUrl& directory);

        // Config & Application Settings
        void CreateAppConfigDirectory();

        void SaveConfig();
        void LoadConfig();

        void SaveApplicationSettings();
        void LoadApplicationSettings();
    };
}

#endif // SHADERIDE_GUI_MAINWINDOW_HPP
