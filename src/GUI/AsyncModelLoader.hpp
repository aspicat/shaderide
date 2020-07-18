// Copyright (c) 2017 - 2020 Aspicat - Florian Roth

#ifndef SHADERIDE_GUI_OPENGLWIDGET_MODELLOADER_HPP
#define SHADERIDE_GUI_OPENGLWIDGET_MODELLOADER_HPP

#include <QObject>
#include "src/GL/World/Vertex.hpp"

using namespace ShaderIDE::GL;

namespace ShaderIDE::GUI {

    // OpenGLWidget Forward Declaration
    class OpenGLWidget;

    class AsyncModelLoader : public QObject
    {
        Q_OBJECT

    public:
        explicit AsyncModelLoader(OpenGLWidget* openGLWidget,
                                  const QString& name,
                                  const QString& file,
                                  VertexVec& vertexBuffer);

        ~AsyncModelLoader() override = default;

    signals:
        void NotifyModelLoaded(const QString& name);

    public slots:
        void OnLoad();

    private:
        OpenGLWidget* openGLWidget{ nullptr };
        QString name{ "" };
        QString file{ "" };
        VertexVec& vertexBuffer;
    };
}

#endif // SHADERIDE_GUI_OPENGLWIDGET_MODELLOADER_HPP
