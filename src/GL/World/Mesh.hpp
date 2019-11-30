/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * Mesh Class
 */

#ifndef SHADERIDE_GL_WORLD_MESH_HPP
#define SHADERIDE_GL_WORLD_MESH_HPP

#include <vector>
#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace ShaderIDE::GL {

    struct Vertex {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    using VertexVec = std::vector<Vertex>;

    class Mesh {
    public:
        void AddVertex(const glm::vec3 &vertex);
        void AddVertexNormal(const glm::vec3 &vertexNormal);
        void AddVertexUV(const glm::vec2 &vertexUV);
        void AddIndex(const uint32_t &index);
        void AddNormalIndex(const uint32_t &normalIndex);
        void AddUVIndex(const uint32_t &uvIndex);

        std::vector<glm::vec3> Vertices();
        std::vector<glm::vec3> VertexNormals();
        std::vector<glm::vec3> VertexIndexedNormals();
        std::vector<glm::vec2> VertexUVs();
        std::vector<uint32_t> Indices();
        std::vector<uint32_t> NormalIndices();
        std::vector<uint32_t> UVIndices();

        /**
         * Composed non-indexed vertices, including
         * normals and UV's.
         *
         * @return VertexVec
         */
        VertexVec ComposedVertices();

    protected:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> vertexNormals;
        std::vector<glm::vec2> vertexUVs;
        std::vector<uint32_t> indices;
        std::vector<uint32_t> normalIndices;
        std::vector<uint32_t> uvIndices;
    };
}

#endif // SHADERIDE_GL_WORLD_MESH_HPP
