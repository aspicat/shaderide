/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * OpenGLWidget Class
 *
 * This is the main widget for the 3D view (left side).
 */

#ifndef SHADERIDE_GUI_OPENGLWIDGET_HPP
#define SHADERIDE_GUI_OPENGLWIDGET_HPP

#include <vector>
#include <array>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLTexture>
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
#include <glm/glm.hpp>
#include <src/Core/GeneralException.hpp>
#include "Widgets/ImageButton.hpp"
#include "src/GL/World/Mesh.hpp"
#include "src/GL/Shader.hpp"
#include "src/GL/GLSLCompileError.hpp"

using namespace ShaderIDE::GL;

namespace ShaderIDE::GUI {

    class OpenGLWidget : public QOpenGLWidget,
                         protected QOpenGLFunctions_4_5_Core {

        static constexpr float MODEL_ROTATION_INTENSITY = 0.5f;
        static constexpr float MODEL_ZOOM_INTENSITY     = 0.02f;
        static constexpr float MODEL_SHIFT_INTENSITY    = 0.008f;

        Q_OBJECT
    public:
        enum class SLOT {
            TEX_0,
            TEX_1,
            TEX_2,
            TEX_3
        };

        explicit OpenGLWidget(QWidget *parent = nullptr);
        ~OpenGLWidget() override;

        void LoadOBJMesh(const QString &file);

        void SetVertexShaderSource(const QString &source);
        void SetFragmentShaderSource(const QString &source);

        static SLOT FindSlotByName(const QString &slotName);
        void ApplyTextureToSlot(const QImage &image, SLOT slot);
        void ClearTextureSlot(SLOT slot);

        void ResetModelRotation();

    signals:
        void si_GLInitialized();
        void si_CompileSuccess(const QString &message);
        void si_CompileError(GLSLCompileError &error);
        void si_StateUpdated(const QString &message);
        void si_GeneralError(const GeneralException &error);

    public slots:
        void sl_CompileShaders();

    private slots:
        void sl_LoadModelCube();
        void sl_LoadModelSphere();
        void sl_LoadModelTeapot();
        void sl_LoadModelBunny();
        void sl_LoadModelDragon();
        void sl_RealtimeUpdateStateChanged(const int &state);
        void sl_Tick();

    protected:
        void initializeGL() override;
        void resizeGL(int w, int h) override;
        void paintGL() override;

        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;

    private:
        GLuint program;
        ShaderSPtr vertexShader;
        ShaderSPtr fragmentShader;

        GLuint vao;
        GLuint vertexBuffer;
        VertexVec vertices;
        VertexVec cubeVertices;
        VertexVec sphereVertices;
        VertexVec teapotVertices;
        VertexVec bunnyVertices;
        VertexVec dragonVertices;

        // Overlay Layout
        QVBoxLayout *overlayLayout;

        // Top Left Layout
        QVBoxLayout *topLeftLayout;
        QCheckBox *cbRealtimeUpdate;

        // Bottom Left (Quick Load Models)
        QHBoxLayout *quickLoadModelsLayout;
        ImageButton *btLoadCube;
        ImageButton *btLoadSphere;
        ImageButton *btLoadTeapot;
        ImageButton *btLoadBunny;
        ImageButton *btLoadDragon;

        // Texture Slots
        QOpenGLTexture *slot0Texture;
        QOpenGLTexture *slot1Texture;
        QOpenGLTexture *slot2Texture;
        QOpenGLTexture *slot3Texture;

        // Shortcuts
        QShortcut *shortcutCompileShaders;

        bool realtime;

        // Render Time
        QDateTime startTime;
        float renderTime;

        // Mouse Model Controls
        int dragStartX;
        int dragStartY;
        int zoomStart;
        float zoomLastPosition;
        int shiftStartX;
        int shiftStartY;
        bool mouseDragEnabled;
        bool mouseZoomEnabled;
        bool mouseShiftEnabled;

        glm::vec3 modelStartRotation;
        glm::vec3 modelRotation;
        glm::vec3 cameraPosition;
        glm::vec3 cameraStartPosition;
        glm::mat4 modelMatrix;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        // Init
        void InitShaders();
        void InitVAO();
        void InitOverlay();
        void InitTopLeftLayout();
        void InitQuickModelButtons();
        void InitShortcuts();

        // Destroy
        void DestroyShortcuts();
        void DestroyTextureSlots();
        void DestroyQuickModelButtons();
        void DestroyTopLeftLayout();
        void DestroyOverlay();
        void DestroyVAO();

        // Realtime
        void EnableRealtime();
        void DisableRealtime();
        void UpdateRealtime();

        // Mouse Model Controls
        void EnableMouseDrag();
        void DisableMouseDrag();
        void EnableMouseZoom();
        void DisableMouseZoom();
        void EnableMouseShift();
        void DisableMouseShift();

        void MoveCamera(const glm::vec3 &position);
        void RotateModel(const GLfloat &x, const GLfloat &y, const GLfloat &z);

        void LoadModel(const QString &name,
                       const QString &file,
                       VertexVec &vertexStore);

        void LoadOBJMeshIntoBuffer(const QString &file,VertexVec &buffer);

        void ApplyVertices(const VertexVec &newVertices);

        // Render Time
        void DetermineStartTime();
        void UpdateRenderTime();

        void RecreateTexture(QOpenGLTexture *texture, const QImage &image);

        void BindTexture(QOpenGLTexture *texture,
                         const QString &location,
                         const uint8_t &unit);

        void BindTextures();
        void ReleaseTextures();
    };
}

#endif // SHADERIDE_GUI_OPENGLWIDGET_HPP
