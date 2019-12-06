// Copyright (c) 2019 Aspicat - Florian Roth

#ifndef SHADERIDE_GUI_OPENGLWIDGET_MODELLOADER_HPP
#define SHADERIDE_GUI_OPENGLWIDGET_MODELLOADER_HPP

#include <QObject>
#include "src/GL/World/Vertex.hpp"

using namespace ShaderIDE::GL;

namespace ShaderIDE::GUI {

    // OpenGLWidget Forward Declaration
    class OpenGLWidget;

    class OpenGLWidget_ModelLoader : public QObject {
    Q_OBJECT
    public:
        explicit OpenGLWidget_ModelLoader(OpenGLWidget *openGLWidget,
                                          const QString &name,
                                          const QString &file,
                                          VertexVec &vertexBuffer);

        ~OpenGLWidget_ModelLoader() override = default;

        signals:
            void si_ModelLoaded(const QString &name);

    public slots:
        void sl_Load();

    private:
        OpenGLWidget *openGLWidget;
        QString name;
        QString file;
        VertexVec &vertexBuffer;
    };
}

#endif // SHADERIDE_GUI_OPENGLWIDGET_MODELLOADER_HPP
