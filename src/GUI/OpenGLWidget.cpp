// Copyright (c) 2019 Aspicat - Florian Roth

#include <QDebug>
#include <QOpenGLContext>
#include <QMouseEvent>
#include <glm/gtc/type_ptr.hpp>
#include "OpenGLWidget.hpp"
#include "src/Core/SyntaxErrorException.hpp"
#include "src/GL/GLDefaults.hpp"
#include "src/GL/GLUtility.hpp"
#include "src/GL/Loaders/OBJMeshLoader.hpp"
#include "src/Core/Memory.hpp"

using namespace ShaderIDE::GUI;

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget             (parent),
      program                   (0),
      vao                       (0),
      vertexBuffer              (0),
      overlayLayout             (nullptr),
      topLeftLayout             (nullptr),
      cbRealtimeUpdate          (nullptr),
      quickLoadModelsLayout     (nullptr),
      btLoadCube                (nullptr),
      btLoadSphere              (nullptr),
      btLoadTeapot              (nullptr),
      btLoadBunny               (nullptr),
      btLoadDragon              (nullptr),
      slot0Texture              (nullptr),
      slot1Texture              (nullptr),
      slot2Texture              (nullptr),
      slot3Texture              (nullptr),
      shortcutCompileShaders    (nullptr),
      realtime                  (false),
      renderTime                (0.f),
      dragStartX                (0),
      dragStartY                (0),
      zoomStart                 (0),
      zoomLastPosition          (0.f),
      shiftStartX               (0),
      shiftStartY               (0),
      mouseDragEnabled          (false),
      mouseZoomEnabled          (false),
      mouseShiftEnabled         (false),
      modelStartRotation        (glm::vec3(0, 0, 0)),
      modelRotation             (glm::vec3(0, 0, 0)),
      cameraPosition            (glm::vec3(0, 0, 0)),
      cameraStartPosition       (glm::vec3(0, 0, 0)),
      modelMatrix               (glm::mat4(1)),
      viewMatrix                (glm::mat4(1)),
      projectionMatrix          (glm::mat4(1))
{
    InitOverlay();
    InitTopLeftLayout();
    InitQuickModelButtons();
    InitShortcuts();

    // Load Default Mesh
    LoadOBJMesh("../assets/models/cube.obj");
    ResetModelRotation();

    // TODO Preload all default meshes for the quick actions...

    MoveCamera(glm::vec3(0, 0, -3));
}

OpenGLWidget::~OpenGLWidget() {
    DestroyShortcuts();
    DestroyTextureSlots();
    DestroyQuickModelButtons();
    DestroyTopLeftLayout();
    DestroyOverlay();
    DestroyVAO();
}

void OpenGLWidget::LoadOBJMesh(const QString &file) {
    try {
        auto meshLoader = OBJMeshLoader(file.toStdString());
        auto mesh = meshLoader.GetMesh();
        vertices = mesh.ComposedVertices();
    } catch (GeneralException &e) {
        emit si_GeneralError(e);
    }
}

void OpenGLWidget::SetVertexShaderSource(const QString &source) {
    vertexShader->SetSource(source);
}

void OpenGLWidget::SetFragmentShaderSource(const QString &source) {
    fragmentShader->SetSource(source);
}

OpenGLWidget::SLOT OpenGLWidget::FindSlotByName(const QString &slotName) {
    auto slot = OpenGLWidget::SLOT::TEX_0;
    if (slotName == GLSL_TEXTURE_SLOT_1_NAME) slot = OpenGLWidget::SLOT::TEX_1;
    if (slotName == GLSL_TEXTURE_SLOT_2_NAME) slot = OpenGLWidget::SLOT::TEX_2;
    if (slotName == GLSL_TEXTURE_SLOT_3_NAME) slot = OpenGLWidget::SLOT::TEX_3;
    return slot;
}

void OpenGLWidget::ApplyTextureToSlot(const QImage &image, SLOT slot) {

    // Image valid?
    if (image.isNull()) {
        return;
    }

    // QImage mirrored for each texture to not be applied upside down.
    switch (slot) {
        case SLOT::TEX_0:
            slot0Texture = new QOpenGLTexture(image.mirrored());
            slot0Texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            break;

        case SLOT::TEX_1:
            slot1Texture = new QOpenGLTexture(image.mirrored());
            slot1Texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            break;

        case SLOT::TEX_2:
            slot2Texture = new QOpenGLTexture(image.mirrored());
            slot2Texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            break;

        case SLOT::TEX_3:
            slot3Texture = new QOpenGLTexture(image.mirrored());
            slot3Texture->setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Linear);
            break;
    }

    repaint();
}

void OpenGLWidget::ClearTextureSlot(SLOT slot) {

    // Create black clear image.
    QImage clearImage(256, 256, QImage::Format_ARGB32);
    clearImage.fill(Qt::black);

    switch (slot) {
        case SLOT::TEX_0:
            RecreateTexture(slot0Texture, clearImage);
            break;

        case SLOT::TEX_1:
            RecreateTexture(slot1Texture, clearImage);
            break;

        case SLOT::TEX_2:
            RecreateTexture(slot2Texture, clearImage);
            break;

        case SLOT::TEX_3:
            RecreateTexture(slot3Texture, clearImage);
            break;
    }

    repaint();
}

void OpenGLWidget::ResetModelRotation() {

    // Reset model matrix first.
    modelMatrix = glm::rotate(glm::mat4(1.f), 0.f, glm::vec3(1.f, 1.f, 1.f));

    RotateModel(glm::radians(45.f), glm::radians(45.f), 0);
    repaint();
}

void OpenGLWidget::sl_CompileShaders() {
    initializeOpenGLFunctions();

    try {
        vertexShader->Compile(program);
        fragmentShader->Compile(program);
        glLinkProgram(program);

        InitVAO();
        repaint();

        emit si_CompileSuccess("Shader compilation finished without errors.");

    } catch (SyntaxErrorException &e) {
        auto error = GLSLCompileError(QString::fromStdString(e.File()), e.what());
        emit si_CompileError(error);
    }
}

void OpenGLWidget::sl_LoadModelCube() {
    LoadModel("Cube", "../assets/models/cube.obj", cubeVertices);
}

void OpenGLWidget::sl_LoadModelSphere() {
    LoadModel("Sphere", "../assets/models/sphere.obj", sphereVertices);
}

void OpenGLWidget::sl_LoadModelTeapot() {
    LoadModel("Teapot", "../assets/models/teapot.obj", teapotVertices);
}

void OpenGLWidget::sl_LoadModelBunny() {
    LoadModel("Bunny", "../assets/models/bunny.obj", bunnyVertices);
}

void OpenGLWidget::sl_LoadModelDragon() {
    LoadModel("Dragon", "../assets/models/dragon.obj", dragonVertices);
}

void OpenGLWidget::sl_RealtimeUpdateStateChanged(const int &state) {
    switch (state) {
        case Qt::Checked:
            EnableRealtime();
            break;

        case Qt::Unchecked:
            DisableRealtime();
            break;

        default: break;
    }
}

void OpenGLWidget::sl_Tick() {
    if (realtime) {
        repaint();
        UpdateRenderTime();
        UpdateRealtime();
    }
}

void OpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    InitShaders();
    InitVAO();

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    emit si_GLInitialized();
}

void OpenGLWidget::resizeGL(int w, int h) {
    float ratio = static_cast<float>(w) / static_cast<float>(h);
    projectionMatrix = glm::perspective(glm::radians(60.f), ratio, 0.1f, 100.f);
}

void OpenGLWidget::paintGL() {
    initializeOpenGLFunctions();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Shader Uniforms
    glUseProgram(program);

    auto renderTimeLocation    = glGetUniformLocation(program, "renderTime");
    auto modelMatLocation      = glGetUniformLocation(program, "modelMat");
    auto viewMatLocation       = glGetUniformLocation(program, "viewMat");
    auto projectionMatLocation = glGetUniformLocation(program, "projectionMat");

    // Apply Uniform Data
    glUniform1f(renderTimeLocation, renderTime);
    glUniformMatrix4fv(modelMatLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(viewMatLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionMatLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Samplers (Textures)
    BindTextures();

    // Draw
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);

    // Reset Shader Program
    glUseProgram(0);

    ReleaseTextures();
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event) {

    // Left Button
    if (event->button() == Qt::MouseButton::LeftButton) {
        EnableMouseDrag();
    }

    // Mid Button
    else if (event->button() == Qt::MouseButton::MidButton) {
        EnableMouseZoom();
    }

    // Right Button
    else if (event->button() == Qt::MouseButton::RightButton) {
        EnableMouseShift();
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    DisableMouseDrag();
    DisableMouseZoom();
    DisableMouseShift();
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event) {

    // Mouse Drag / Model Rotation
    if (mouseDragEnabled) {
        if (!dragStartX) dragStartX = mapToParent(event->pos()).x();
        if (!dragStartY) dragStartY = mapToParent(event->pos()).y();

        auto rotX = static_cast<GLfloat>(-(dragStartY - mapToParent(event->pos()).y()));
        auto rotY = static_cast<GLfloat>(-(dragStartX - mapToParent(event->pos()).x()));

        modelRotation = modelStartRotation + glm::vec3(
                glm::radians(rotX) * MODEL_ROTATION_INTENSITY,
                glm::radians(rotY) * MODEL_ROTATION_INTENSITY,
                0 /* Z */
        );

        modelMatrix = glm::rotate(glm::mat4(1), modelRotation.x, glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, modelRotation.y, glm::vec3(0, 1, 0));
    }

    // Mouse Drag / Model Zoom (Camera Forward/Backwards)
    else if (mouseZoomEnabled) {
        if (!zoomStart) zoomStart = mapToParent(event->pos()).y();
        auto zoom = -(zoomStart - mapToParent(event->pos()).y());
        MoveCamera(glm::vec3(
                cameraPosition.x,
                cameraPosition.y,
                (zoom * MODEL_ZOOM_INTENSITY + zoomLastPosition))
        );
    }

    // Mouse Shift / Model Shifting
    else if (mouseShiftEnabled) {
        if (!shiftStartX) shiftStartX = mapToParent(event->pos()).x();
        if (!shiftStartY) shiftStartY = mapToParent(event->pos()).y();

        auto shiftX = -(shiftStartX - mapToParent(event->pos()).x());
        auto shiftY = shiftStartY - mapToParent(event->pos()).y();

        cameraPosition = cameraStartPosition + glm::vec3(
                static_cast<GLfloat>(shiftX) * MODEL_SHIFT_INTENSITY,
                static_cast<GLfloat>(shiftY) * MODEL_SHIFT_INTENSITY,
                0 /* Z */
        );

        viewMatrix = glm::translate(glm::mat4(1), cameraPosition);
    }

    repaint();
}

void OpenGLWidget::InitShaders() {
    program = glCreateProgram();

    vertexShader = Shader::MakeShared("", GL_VERTEX_SHADER);
    vertexShader->SetFile("VS");

    fragmentShader = Shader::MakeShared("", GL_FRAGMENT_SHADER);
    fragmentShader->SetFile("FS");
}

void OpenGLWidget::InitVAO() {

    // VAO
    if (!vao) {
        glGenVertexArrays(1, &vao);
    }

    glBindVertexArray(vao);

    // Vertex Buffer
    if (!vertexBuffer) {
        glGenBuffers(1, &vertexBuffer);
    }

    const auto stride = 8;

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * stride * sizeof(GLfloat),
            vertices.data(), GL_STATIC_DRAW);

    // Vertex Position Attrib
    auto vPosLocation = glGetAttribLocation(program, "vPos");
    glEnableVertexAttribArray(vPosLocation);
    glVertexAttribPointer(vPosLocation, 3, GL_FLOAT, GL_FALSE,
                          GLUtility::StrideSize(stride), GLUtility::StridePtr(0));

    // Normal Attrib
    auto vNormalLocation = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormalLocation);
    glVertexAttribPointer(vNormalLocation, 3, GL_FLOAT, GL_FALSE,
                          GLUtility::StrideSize(stride), GLUtility::StridePtr(3));

    // UV Attrib
    auto vTexUVLocation = glGetAttribLocation(program, "vTexUV");
    glEnableVertexAttribArray(vTexUVLocation);
    glVertexAttribPointer(vTexUVLocation, 2, GL_FLOAT, GL_FALSE,
                          GLUtility::StrideSize(stride), GLUtility::StridePtr(6));

    // Unbind VAO
    glBindVertexArray(0);
}

void OpenGLWidget::InitOverlay() {

    // Overlay Layout
    overlayLayout = new QVBoxLayout();
    overlayLayout->setAlignment(Qt::AlignLeft);
    setLayout(overlayLayout);
}

void OpenGLWidget::InitTopLeftLayout() {

    // Top Left Layout
    topLeftLayout = new QVBoxLayout();
    topLeftLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    overlayLayout->addLayout(topLeftLayout);

    // Realtime Update Checkbox
    cbRealtimeUpdate = new QCheckBox("Realtime");

    // TODO
    cbRealtimeUpdate->setStyleSheet(
            "QCheckBox {"
            "    color: #fafafa;"
            "    background: transparent;"
            "}"
    );

    topLeftLayout->addWidget(cbRealtimeUpdate);

    connect(cbRealtimeUpdate, SIGNAL(stateChanged(int)),
            this, SLOT(sl_RealtimeUpdateStateChanged(int)));
}

void OpenGLWidget::InitQuickModelButtons() {

    // Quick Load Models Layout
    quickLoadModelsLayout = new QHBoxLayout();
    quickLoadModelsLayout->setSpacing(10);
    quickLoadModelsLayout->setContentsMargins(0, 0, 0, 0);
    overlayLayout->addLayout(quickLoadModelsLayout, 0);

    // Load Cube
    btLoadCube = new ImageButton("../assets/images/cube.png");
    quickLoadModelsLayout->addWidget(btLoadCube, 0, Qt::AlignLeft);
    connect(btLoadCube, SIGNAL(clicked()), this, SLOT(sl_LoadModelCube()));

    // Load Sphere
    btLoadSphere = new ImageButton("../assets/images/sphere.png");
    quickLoadModelsLayout->addWidget(btLoadSphere, 0, Qt::AlignLeft);
    connect(btLoadSphere, SIGNAL(clicked()), this, SLOT(sl_LoadModelSphere()));

    // Load Teapot
    btLoadTeapot = new ImageButton("../assets/images/teapot.png");
    quickLoadModelsLayout->addWidget(btLoadTeapot, 0, Qt::AlignLeft);
    connect(btLoadTeapot, SIGNAL(clicked()), this, SLOT(sl_LoadModelTeapot()));

    // Load Bunny
    btLoadBunny = new ImageButton("../assets/images/bunny.png");
    quickLoadModelsLayout->addWidget(btLoadBunny, 0, Qt::AlignLeft);
    connect(btLoadBunny, SIGNAL(clicked()), this, SLOT(sl_LoadModelBunny()));

    // Load Dragon
    btLoadDragon = new ImageButton("../assets/images/dragon.png");
    quickLoadModelsLayout->addWidget(btLoadDragon, 0, Qt::AlignLeft);
    connect(btLoadDragon, SIGNAL(clicked()), this, SLOT(sl_LoadModelDragon()));
}

void OpenGLWidget::InitShortcuts() {

    // Compile Shaders
    shortcutCompileShaders = new QShortcut(
            QKeySequence(Qt::CTRL + Qt::Key_R),
            this,
            SLOT(sl_CompileShaders())
    );
}

void OpenGLWidget::DestroyShortcuts() {
    Memory::Release(shortcutCompileShaders);
}

void OpenGLWidget::DestroyTextureSlots() {
    // TODO SIGSEGV (Segmentation fault)

/* QOpenGLTexture::destroy():
       sub    $0x28,%rsp
       mov    (%rcx),%rcx
       callq  0xe07a098                  # <QOpenGLTexturePrivate::destroy()>
       nop
       add    $0x28,%rsp
       retq*/
}

void OpenGLWidget::DestroyQuickModelButtons() {
    Memory::Release(btLoadDragon);
    Memory::Release(btLoadBunny);
    Memory::Release(btLoadTeapot);
    Memory::Release(btLoadSphere);
    Memory::Release(btLoadCube);
    Memory::Release(quickLoadModelsLayout);
}

void OpenGLWidget::DestroyTopLeftLayout() {
    Memory::Release(cbRealtimeUpdate);
    Memory::Release(topLeftLayout);
}

void OpenGLWidget::DestroyOverlay() {
    Memory::Release(overlayLayout);
}

void OpenGLWidget::DestroyVAO() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}

void OpenGLWidget::EnableRealtime() {
    realtime = true;
    DetermineStartTime();
    UpdateRealtime();
}

void OpenGLWidget::DisableRealtime() {
    realtime = false;
}

void OpenGLWidget::UpdateRealtime() {
    QTimer::singleShot(33, this, SLOT(sl_Tick()));
}

void OpenGLWidget::EnableMouseDrag() {
    mouseDragEnabled   = true;
    dragStartX         = 0;
    dragStartY         = 0;
    modelStartRotation = modelRotation;
}

void OpenGLWidget::DisableMouseDrag() {
    mouseDragEnabled = false;
}

void OpenGLWidget::EnableMouseZoom() {
    mouseZoomEnabled = true;
    zoomStart        = 0;
    zoomLastPosition = cameraPosition.z;
}

void OpenGLWidget::DisableMouseZoom() {
    mouseZoomEnabled = false;
}

void OpenGLWidget::EnableMouseShift() {
    mouseShiftEnabled   = true;
    shiftStartX         = 0;
    shiftStartY         = 0;
    cameraStartPosition = cameraPosition;
}

void OpenGLWidget::DisableMouseShift() {
    mouseShiftEnabled = false;
}

void OpenGLWidget::MoveCamera(const glm::vec3 &position) {
    cameraPosition = position;
    viewMatrix = glm::translate(glm::mat4(1), cameraPosition);
}

void OpenGLWidget::RotateModel(const GLfloat &x, const GLfloat &y, const GLfloat &z) {
    modelRotation.x = x;
    modelRotation.y = y;
    modelRotation.z = z;
    modelMatrix = glm::rotate(modelMatrix, modelRotation.x, glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, modelRotation.y, glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, modelRotation.z, glm::vec3(0, 0, 1));
}

void OpenGLWidget::LoadModel(const QString &name,
                             const QString &file,
                             VertexVec &vertexStore)
{
    if (vertexStore.empty()) {
        LoadOBJMeshIntoBuffer(file, vertexStore);
    }

    ApplyVertices(vertexStore);
    emit si_StateUpdated(QString("Model \"") + name + "\" loaded.");
}

void OpenGLWidget::LoadOBJMeshIntoBuffer(const QString &file, VertexVec &buffer) {
    try {
        auto meshLoader = OBJMeshLoader(file.toStdString());
        auto mesh = meshLoader.GetMesh();
        buffer = mesh.ComposedVertices();
    } catch (GeneralException &e) {
        emit si_GeneralError(e);
    }
}

void OpenGLWidget::ApplyVertices(const VertexVec &newVertices) {
    vertices = newVertices;
    InitVAO();
    repaint();
}

void OpenGLWidget::DetermineStartTime() {
    startTime = QDateTime::currentDateTime();
}

void OpenGLWidget::UpdateRenderTime() {
    auto millisDiff = static_cast<float>(startTime.msecsTo(QDateTime::currentDateTime()));
    renderTime = millisDiff / 1000.f;
}

void OpenGLWidget::RecreateTexture(QOpenGLTexture *texture, const QImage &image) {
    if (texture == nullptr) {
        return;
    }

    texture->destroy();
    texture->setData(image, QOpenGLTexture::MipMapGeneration::GenerateMipMaps);
    texture->create();
}

void OpenGLWidget::BindTexture(QOpenGLTexture *texture,
                               const QString &location,
                               const uint8_t &unit)
{
    if (texture != nullptr) {
        auto uniformLocation = glGetUniformLocation(program, location.toStdString().c_str());

        if (uniformLocation) {
            glUniform1i(uniformLocation, unit);
            texture->bind(unit);
        }
    }
}

void OpenGLWidget::BindTextures() {
    BindTexture(slot0Texture, GLSL_TEXTURE_SLOT_0_NAME, 0);
    BindTexture(slot1Texture, GLSL_TEXTURE_SLOT_1_NAME, 1);
    BindTexture(slot2Texture, GLSL_TEXTURE_SLOT_2_NAME, 2);
    BindTexture(slot3Texture, GLSL_TEXTURE_SLOT_3_NAME, 3);
}

void OpenGLWidget::ReleaseTextures() {
    if (slot0Texture) slot0Texture->release();
    if (slot1Texture) slot1Texture->release();
    if (slot2Texture) slot2Texture->release();
    if (slot3Texture) slot3Texture->release();
}
