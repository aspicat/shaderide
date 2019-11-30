/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * OBJMeshLoader Class
 *
 * This is probably the slowest and most basic .obj parser
 * ever, picked out of an old project, but it does its job
 * for now.
 *
 * The parsing is realized by boost Spirit X3.
 */

#ifndef SHADERIDE_GL_LOADERS_OBJMESHLOADER_HPP
#define SHADERIDE_GL_LOADERS_OBJMESHLOADER_HPP

#include <string>
#include <fstream>
#include "src/GL/World/Mesh.hpp"

namespace ShaderIDE::GL {

    struct IndexContainer {
        uint32_t numVertexIndices;
        uint32_t numTextureIndices;
        uint32_t numNormalIndices;
    };

    class OBJMeshLoader {
    public:
        explicit OBJMeshLoader(const std::string &path);

        Mesh GetMesh();

    private:
        Mesh mesh;
        uint32_t lineCounter;

        void ReadFile(const std::string &path);
        void ParseLine(const std::string &line);
        glm::vec2 ParseVector2(const std::string &line);
        glm::vec3 ParseVector3(const std::string &line);

        std::vector<IndexContainer>
        ParseIndices(const std::string &line);

        bool IsVertex(const std::string &line);
        bool IsVertexNormal(const std::string &line);
        bool IsVertexTexture(const std::string &line);
        bool IsFace(const std::string &line);

        bool IdentifyDataType(const std::string &line,
                              const std::string &pattern);
    };
}

#endif // SHADERIDE_GL_LOADERS_OBJMESHLOADER_HPP
