// Copyright (c) 2017 - 2020 Aspicat - Florian Roth

#include "AsyncModelLoader.hpp"
#include "OpenGLWidget.hpp"

using namespace ShaderIDE::GUI;

AsyncModelLoader::AsyncModelLoader(OpenGLWidget* openGLWidget,
                                   const QString& name,
                                   const QString& file,
                                   VertexVec& vertexBuffer)
        : QObject(),
          openGLWidget(openGLWidget),
          name(name),
          file(file),
          vertexBuffer(vertexBuffer)
{}

void AsyncModelLoader::OnLoad()
{
    if (vertexBuffer.empty()) {
        openGLWidget->LoadOBJMeshIntoBuffer(file, vertexBuffer);
    }

    openGLWidget->ApplyVertices(vertexBuffer);
    emit NotifyModelLoaded(name);
}
