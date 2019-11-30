/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * ShaderProject Class
 */

#ifndef SHADERIDE_PROJECT_SHADERPROJECT_HPP
#define SHADERIDE_PROJECT_SHADERPROJECT_HPP

#include <string>
#include <unordered_map>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace ShaderIDE::Project {

    class ShaderProject {
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive &ar, const uint32_t version) {
            ar & path;
            ar & vsSource;
            ar & fsSource;
            ar & meshName;
            ar & texturePaths;
        }

    public:
        static ShaderProject* Load(const std::string &path);

        explicit ShaderProject(std::string path);

        std::string Path();
        std::string VertexShaderSource();
        std::string FragmentShaderSource();
        std::string MeshName();
        std::unordered_map<std::string, std::string> TexturePaths();

        void SetPath(const std::string &newPath);
        void SetVertexShaderSource(const std::string &newVSSource);
        void SetFragmentShaderSource(const std::string &newFSSource);
        void SetMeshName(const std::string &newMeshName);
        void SetTexturePaths(const std::unordered_map<std::string, std::string> &newTexturePaths);

        void SetTexturePath(const std::string &name, const std::string &texturePath);
        void ClearTexturePath(const std::string &name);

        void Save();

    private:
        std::string path;
        std::string vsSource;
        std::string fsSource;
        std::string meshName;
        std::unordered_map<std::string, std::string> texturePaths;
    };
}

#endif // SHADERIDE_PROJECT_SHADERPROJECT_HPP
