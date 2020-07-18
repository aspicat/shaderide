/**
 * Vertex Struct
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

#ifndef SHADERIDE_GL_WORLD_VERTEX_HPP
#define SHADERIDE_GL_WORLD_VERTEX_HPP

#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace ShaderIDE::GL {

    struct Vertex
    {
        glm::vec3 vertex;
        glm::vec3 normal;
        glm::vec2 uv;

        Vertex() = default;

        explicit Vertex(const float& posX, const float& posY, const float& posZ,
                        const float& normalX, const float& normalY, const float& normalZ,
                        const float& uvX, const float& uvY)
                : vertex(posX, posY, posZ),
                  normal(normalX, normalY, normalZ),
                  uv(uvX, uvY)
        {}

        ~Vertex() = default;

        Vertex& operator=(const float vtx[8])
        {
            vertex = { vtx[0], vtx[1], vtx[2] };
            normal = { vtx[3], vtx[4], vtx[5] };
            uv = { vtx[6], vtx[7] };
            return *this;
        }
    };

    using VertexVec = std::vector<Vertex>;
}

#endif // SHADERIDE_GL_WORLD_VERTEX_HPP
