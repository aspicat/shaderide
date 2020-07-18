/**
 * ShaderProject Class
 *
 * --------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/aspicat/shaderide.
 * --------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Aspicat - Florian Roth
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
#include "SerializableVector3.hpp"
#include "src/version.hpp"
#include "src/GL/GLDefaults.hpp"

namespace ShaderIDE::Project {

    class ShaderProject
    {
    public:
        static ShaderProject* Load(const std::string& path);

        explicit ShaderProject(std::string path);

        std::string Version();
        std::string Path();
        std::string VertexShaderSource();
        std::string FragmentShaderSource();
        std::string MeshName();
        std::unordered_map<std::string, std::string> TextureData();
        bool Realtime() const;
        bool Plane2D() const;
        glm::vec3 ModelRotation();
        glm::vec3 CameraPosition();

        std::string PathOnly();

        void SetFileVersion(const std::string& newFileVersion);
        void SetPath(const std::string& newPath);
        void SetVertexShaderSource(const std::string& newVSSource);
        void SetFragmentShaderSource(const std::string& newFSSource);
        void SetMeshName(const std::string& newMeshName);
        void SetRealtime(bool newRealtime);
        void SetPlane2D(bool newPlane2D);
        void SetModelRotation(const glm::vec3& newModelRotation);
        void SetCameraPosition(const glm::vec3& newCameraPosition);

        void SetTextureData(const std::string& name, const std::string& data);
        void ClearTextureData(const std::string& name);

        void Save();

    private:
        static void CheckPath(const std::string& filePath);
        static std::ifstream OpenInputFile(const std::string& filePath);
        static std::ofstream OpenOutputFile(const std::string& filePath);

        std::string file_version{ SHADERIDE_VERSION };
        std::string path{ "" };
        std::string vsSource{ "" };
        std::string fsSource{ "" };
        std::string meshName{ "" };
        std::unordered_map<std::string, std::string> textureData;
        bool realtime{ false };
        bool plane2D{ false };
        SerializableVector3 modelRotation{ OPENGLWIDGET_DEFAULT_MODEL_ROTATION };
        SerializableVector3 cameraPosition{ OPENGLWIDGET_DEFAULT_CAMERA_POSITION };

        QJsonObject MakeJsonObjectFromTextureData();

        size_t LastSlashPos();
    };
}

#endif // SHADERIDE_PROJECT_SHADERPROJECT_HPP
