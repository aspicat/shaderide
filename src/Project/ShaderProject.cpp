/**
 * ShaderProject Class
 *
 * --------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/aspicat/shaderide.
 * --------------------------------------------------------------------------
 *
 * Copyright (c) 2019 Aspicat - Florian Roth
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

#include <utility>
#include <fstream>
#include "ShaderProject.hpp"
#include "ProjectException.hpp"
#include "src/version.hpp"
#include "src/GL/GLDefaults.hpp"

using namespace ShaderIDE::Project;

ShaderProject* ShaderProject::Load(const std::string &path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw GeneralException(
                std::string("Could not load shader project file \"") +
                path +
                "\""
        );
    }

    auto *shaderProject = new ShaderProject(path);

    try {
        boost::archive::binary_iarchive ia(file);
        ia >> *shaderProject;

    } catch (boost::archive::archive_exception &e) {
        // Loaded an older version.

    } catch (std::exception &e) {
        file.close();
        return nullptr;
    }

    file.close();

    return shaderProject;
}

ShaderProject::ShaderProject(std::string path)
    : file_version      (SHADERIDE_VERSION),
      path              (std::move(path)),
      vsSource          (""),
      fsSource          (""),
      meshName          (""),
      realtime          (false),
      plane2D           (false),
      modelRotation     (OPENGLWIDGET_DEFAULT_MODEL_ROTATION),
      cameraPosition    (OPENGLWIDGET_DEFAULT_CAMERA_POSITION)
{}

std::string ShaderProject::Version() {
    return file_version;
}

std::string ShaderProject::Path() {
    return path;
}

std::string ShaderProject::VertexShaderSource() {
    return vsSource;
}

std::string ShaderProject::FragmentShaderSource() {
    return fsSource;
}

std::string ShaderProject::MeshName() {
    return meshName;
}

std::unordered_map<std::string, std::string> ShaderProject::TexturePaths() {
    return texturePaths;
}

bool ShaderProject::Realtime() {
    return realtime;
}

bool ShaderProject::Plane2D() {
    return plane2D;
}

glm::vec3 ShaderProject::ModelRotation() {
    return modelRotation;
}

glm::vec3 ShaderProject::CameraPosition() {
    return cameraPosition;
}

void ShaderProject::SetPath(const std::string &newPath) {
    path = newPath;
}

void ShaderProject::SetVertexShaderSource(const std::string &newVSSource) {
    vsSource = newVSSource;
}

void ShaderProject::SetFragmentShaderSource(const std::string &newFSSource) {
    fsSource = newFSSource;
}

void ShaderProject::SetMeshName(const std::string &newMeshName) {
    meshName = newMeshName;
}

void ShaderProject::SetRealtime(bool newRealtime) {
    realtime = newRealtime;
}

void ShaderProject::SetPlane2D(bool newPlane2D) {
    plane2D = newPlane2D;
}

void ShaderProject::SetModelRotation(const glm::vec3 &newModelRotation) {
    modelRotation = newModelRotation;
}

void ShaderProject::SetCameraPosition(const glm::vec3 &newCameraPosition) {
    cameraPosition = newCameraPosition;
}

void ShaderProject::SetTexturePath(const std::string &name,
                                   const std::string &texturePath)
{
    texturePaths[name] = texturePath;
}

void ShaderProject::ClearTexturePath(const std::string &name) {
    if (texturePaths.find(name) != texturePaths.end()) {
        texturePaths[name] = "";
    }
}

void ShaderProject::Save() {

    // Check Path
    if (path.empty()) {
        throw ProjectException(
                "Could not save shader project. "
                "Project path not provided.",
                ProjectException::ExceptionCode::PATH_EMPTY
        );
    }

    // Save File
    std::ofstream file(path);

    if (!file.is_open()) {
        throw GeneralException(
                std::string("Could not save shader project to \"") +
                path + "\""
        );
    }

    boost::archive::binary_oarchive oa(file);
    oa << *this;
    file.close();

}
