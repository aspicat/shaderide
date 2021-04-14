/**
 * ShaderProject Class
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

#ifndef SHADERIDE_PROJECT_SHADERPROJECT_HPP
#define SHADERIDE_PROJECT_SHADERPROJECT_HPP

#include <string>
#include <unordered_map>
#include <QObject>
#include "SerializableVector3.hpp"
#include "src/version.hpp"
#include "src/GL/GLDefaults.hpp"

namespace ShaderIDE::GUI {
    class MainWindow;
}

namespace ShaderIDE::Project {

    class ShaderProject : public QObject
    {
        friend class GUI::MainWindow;

        Q_OBJECT
    public:
        static ShaderProject* Load(const QString& path);

        explicit ShaderProject(QString path);

        QString Version();
        QString Path();
        QString VertexShaderSource();
        QString FragmentShaderSource();
        QString MeshName();
        std::unordered_map<QString, QString> TextureData();
        bool Realtime() const;
        bool Plane2D() const;
        glm::vec3 ModelRotation();
        glm::vec3 CameraPosition();

        QString PathOnly();

        bool Saved() const;

        void SetFileVersion(const QString& newFileVersion);
        void SetPath(const QString& newPath);
        void SetVertexShaderSource(const QString& newVSSource);
        void SetFragmentShaderSource(const QString& newFSSource);
        void SetMeshName(const QString& newMeshName);
        void SetRealtime(bool newRealtime);
        void SetPlane2D(bool newPlane2D);
        void SetModelRotation(const glm::vec3& newModelRotation);
        void SetCameraPosition(const glm::vec3& newCameraPosition);

        void SetTextureData(const QString& name, const QString& data);
        void ClearTextureData(const QString& name);

        void Save();

    signals:
        void NotifyMarkSaved();
        void NotifyMarkUnsaved();

    private:
        static void CheckPath(const QString& filePath);
        static std::ifstream OpenInputFile(const QString& filePath);
        static std::ofstream OpenOutputFile(const QString& filePath);

        bool saved{ false };

        QString file_version{ SHADERIDE_VERSION };
        QString path{ "" };
        QString vsSource{ "" };
        QString fsSource{ "" };
        QString meshName{ "" };
        std::unordered_map<QString, QString> textureData;
        bool realtime{ false };
        bool plane2D{ false };
        SerializableVector3 modelRotation{ OPENGLWIDGET_DEFAULT_MODEL_ROTATION };
        SerializableVector3 cameraPosition{ OPENGLWIDGET_DEFAULT_CAMERA_POSITION };

        QJsonObject MakeJsonObjectFromTextureData();

        size_t LastSlashPos();

        void MarkSaved();
        void MarkUnsaved();
    };
}

#endif // SHADERIDE_PROJECT_SHADERPROJECT_HPP
