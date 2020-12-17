/**
 * OpenGLWidget Class
 *
 * This is the main widget for the 3D view (left side).
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Florian Roth
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

#ifndef SHADERIDE_GUI_OPENGLWIDGET_HPP
#define SHADERIDE_GUI_OPENGLWIDGET_HPP

#include <vector>
#include <array>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLFunctions_4_5_Core>
#include <QtOpenGL/QOpenGLTexture>
#include <QThread>
#include <QImage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QSet>
#include <QShortcut>
#include <QTimer>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include <QSplitter>
#include <glm/glm.hpp>
#include "src/Core/GeneralException.hpp"
#include "Widgets/ImageButton.hpp"
#include "Widgets/LoadingWidget.hpp"
#include "src/GL/World/Mesh.hpp"
#include "src/GL/Shader.hpp"
#include "src/GL/GLSLCompileError.hpp"
#include "AsyncModelLoader.hpp"

using namespace ShaderIDE::GL;

namespace ShaderIDE::GUI {

    class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
    {
        Q_OBJECT

        friend class AsyncModelLoader;

        static constexpr float MODEL_ROTATION_INTENSITY = 0.5f;
        static constexpr float MODEL_ZOOM_INTENSITY = 0.02f;
        static constexpr float MODEL_SHIFT_INTENSITY = 0.008f;
        static constexpr uint8_t STRIDE_SIZE = 8;

    public:
        enum class SLOT
        {
            TEX_0,
            TEX_1,
            TEX_2,
            TEX_3
        };

        explicit OpenGLWidget(QSplitter* splitter, QWidget* parent = nullptr);
        ~OpenGLWidget() override;

        QString SelectedMeshName();
        void SelectMesh(const QString& meshName);

        void CheckRealtime(bool realtimeChecked);
        bool Realtime();

        void CheckPlane2D(bool plane2DChecked);
        bool Plane2D();

        void RotateModel(const glm::vec3& rotation);
        void MoveCamera(const glm::vec3& position);

        glm::vec3 ModelRotation();
        glm::vec3 CameraPosition();

        void SetVertexShaderSource(const QString& source);
        void SetFragmentShaderSource(const QString& source);

        void SetRealtimeCompilationEnabled(bool value);
        void ToggleRealtimeCompilation();
        bool RealtimeCompilation();

        static SLOT FindSlotByName(const QString& slotName);
        void ApplyTextureToSlot(const QImage& image, SLOT slot);
        void ClearTextureSlot(SLOT slot);

        void ResetUI();

    signals:
        void NotifyGLInitialized();
        void NotifyCompileSuccess(const QString& message);
        void NotifyCompileError(GLSLCompileError& error);
        void NotifyStateUpdated(const QString& message);
        void NotifyGeneralError(const GeneralException& error);
        void NotifyTriggerModelLoading();

    public slots:
        void OnCompileShaders();

    private slots:
        void OnLoadModelCube();
        void OnLoadModelSphere();
        void OnLoadModelTorus();
        void OnLoadModelTeapot();
        void OnLoadModelBunny();
        void OnModelLoaded(const QString& name);
        void OnRealtimeUpdateStateChanged(const int& state);
        void OnPlane2DStateChanged(const int& state);
        void OnSquareViewportClicked();
        void OnTick();

    protected:
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;

        void mousePressEvent(QMouseEvent* event) override;
        void mouseDoubleClickEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;

    private:
        QSplitter* splitter{ nullptr }; // DO NOT DESTROY

        GLuint program{ 0 };
        ShaderSPtr vertexShader;
        ShaderSPtr fragmentShader;

        GLuint vao{ 0 };
        GLuint vertexBuffer{ 0 };
        VertexVec vertices;
        VertexVec cubeVertices;
        VertexVec sphereVertices;
        VertexVec torusVertices;
        VertexVec teapotVertices;
        VertexVec bunnyVertices;
        QString selectedMeshName{ "" };

        GLuint planeVAO{ 0 };
        GLuint planeVertexBuffer{ 0 };
        VertexVec planeVertices;

        // Overlay Layout
        QVBoxLayout* overlayLayout{ nullptr };

        // Top Layout
        QHBoxLayout* topLayout{ nullptr };
        QCheckBox* cbRealtimeUpdate{ nullptr };
        QCheckBox* cbPlane2D{ nullptr };
        ImageButton* ibSquareViewport{ nullptr };

        // Bottom Left (Quick Load Models)
        QHBoxLayout* quickLoadModelsLayout{ nullptr };
        ImageButton* btLoadCube{ nullptr };
        ImageButton* btLoadSphere{ nullptr };
        ImageButton* btLoadTorus{ nullptr };
        ImageButton* btLoadTeapot{ nullptr };
        ImageButton* btLoadBunny{ nullptr };

        // Loading Widget
        QThread modelLoaderThread;
        QMutex modelLoaderMutex;
        LoadingWidget* loadingWidget{ nullptr };

        // Texture Slots
        QOpenGLTexture* slot0Texture{ nullptr };
        QOpenGLTexture* slot1Texture{ nullptr };
        QOpenGLTexture* slot2Texture{ nullptr };
        QOpenGLTexture* slot3Texture{ nullptr };

        bool realtime{ false };
        bool plane2D{ false };
        bool realtimeCompilation{ false };

        // Render Time
        QDateTime startTime;
        float renderTime{ 0.0f };

        // Mouse Model Controls
        int dragStartX{ 0 };
        int dragStartY{ 0 };
        int zoomStart{ 0 };
        float zoomLastPosition{ 0.0f };
        int shiftStartX{ 0 };
        int shiftStartY{ 0 };
        bool mouseDragEnabled{ false };
        bool mouseZoomEnabled{ false };
        bool mouseShiftEnabled{ false };

        glm::vec3 modelStartRotation{ glm::vec3(0.0f) };
        glm::vec3 modelRotation{ glm::vec3(0.0f) };
        glm::vec3 cameraPosition{ glm::vec3(0.0f) };
        glm::vec3 cameraStartPosition{ glm::vec3(0.0f) };
        glm::mat4 modelMatrix{ glm::mat4(1.0f) };
        glm::mat4 viewMatrix{ glm::mat4(1.0f) };
        glm::mat4 projectionMatrix{ glm::mat4(1.0f) };
        glm::mat4 identityMatrix{ glm::mat4(1.0f) };

        // Init
        void InitShaders();
        void InitVAO();
        void InitPlaneVAO();
        void InitOverlay();
        void InitTopLayout();
        void InitQuickModelButtons();
        void InitLoadingWidget();

        // Layouts
        void ShowQuickLoadModelsLayout();
        void HideQuickLoadModelsLayout();
        void SquareViewportAndUpdateSplitter();

        // Realtime
        void EnableRealtime();
        void DisableRealtime();
        void UpdateRealtime();

        // Plane 2D
        void EnablePlane2D();
        void DisablePlane2D();

        // Mouse Model Controls
        void EnableMouseDrag();
        void DisableMouseDrag();
        void EnableMouseZoom();
        void DisableMouseZoom();
        void EnableMouseShift();
        void DisableMouseShift();

        void ApplyMouseDrag(QMouseEvent* event);
        void ApplyMouseZoom(QMouseEvent* event);
        void ApplyMouseShift(QMouseEvent* event);

        void ResetModelRotation();
        void ResetCameraPosition();

        // Model
        void LoadOBJMesh(const QString& path);
        void LoadPlaneMesh();

        void LoadModelAsync(const QString& name,
                            const QString& file,
                            VertexVec& vertexStore);

        void LoadOBJMeshIntoBuffer(const QString& file,
                                   VertexVec& buffer);

        void ApplyVertices(const VertexVec& newVertices);

        // Value Pointers
        GLfloat* GetModelMatrix();
        GLfloat* GetViewMatrix();
        GLfloat* GetProjectionMatrix();

        // Render Time
        void DetermineStartTime();
        void UpdateRenderTime();

        // Textures
        void RecreateTexture(QOpenGLTexture* texture, const QImage& image);

        void BindTexture(QOpenGLTexture* texture,
                         const QString& location,
                         const uint8_t& unit);

        void BindTextures();
        void ReleaseTextures();

        // GL
        void InitAttribsForVAO();
        void LinkProgramAndRepaint();
        void DrawVAO();
        void DrawPlaneVAO();
    };
}

#endif // SHADERIDE_GUI_OPENGLWIDGET_HPP
