/**
 * Mesh Class
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Florian Roth
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

#ifndef SHADERIDE_GL_WORLD_MESH_HPP
#define SHADERIDE_GL_WORLD_MESH_HPP

#include <string>
#include <cstdint>
#include "Vertex.hpp"

namespace ShaderIDE::GL {

    class Mesh
    {
    public:
        void AddVertex(const glm::vec3& vertex);
        void AddVertexNormal(const glm::vec3& vertexNormal);
        void AddVertexUV(const glm::vec2& vertexUV);
        void AddIndex(const uint32_t& index);
        void AddNormalIndex(const uint32_t& normalIndex);
        void AddUVIndex(const uint32_t& uvIndex);

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

        std::string ComposedVerticesString();

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
