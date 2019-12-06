// Copyright (c) 2019 Aspicat - Florian Roth

#include "OpenGLWidget_ModelLoader.hpp"
#include "OpenGLWidget.hpp"

using namespace ShaderIDE::GUI;

OpenGLWidget_ModelLoader::OpenGLWidget_ModelLoader(OpenGLWidget *openGLWidget,
                                                   const QString &name,
                                                   const QString &file,
                                                   VertexVec &vertexBuffer)
    : QObject       (),
      openGLWidget  (openGLWidget),
      name          (name),
      file          (file),
      vertexBuffer  (vertexBuffer)
{}

void OpenGLWidget_ModelLoader::sl_Load() {
    if (vertexBuffer.empty()) {
        openGLWidget->LoadOBJMeshIntoBuffer(file, vertexBuffer);
    }

    openGLWidget->ApplyVertices(vertexBuffer);
    emit si_ModelLoaded(name);
}
