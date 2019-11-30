// Copyright (c) 2019 Aspicat - Florian Roth

#include <utility>
#include <fstream>
#include "ShaderProject.hpp"
#include "src/Core/GeneralException.hpp"

using namespace ShaderIDE::Project;

ShaderProject* ShaderProject::Load(const std::string &path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        throw GeneralException(
                std::string("Could not load shader project file \"")
                + path + "\""
        );
    }

    auto *shaderProject = new ShaderProject(path);

    boost::archive::binary_iarchive ia(file);
    ia >> *shaderProject;
    file.close();

    return shaderProject;
}

ShaderProject::ShaderProject(std::string path)
    : path      (std::move(path)),
      vsSource  (""),
      fsSource  (""),
      meshName  ("")
{}

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

void ShaderProject::SetTexturePaths(const std::unordered_map<std::string, std::string> &newTexturePaths) {
    texturePaths = newTexturePaths;
}

void ShaderProject::SetTexturePath(const std::string &name, const std::string &texturePath) {
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
        throw GeneralException(
                "Could not save shader project. "
                "Project path not provided."
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
