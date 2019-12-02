/**
 * OBJMeshLoader Class
 *
 * This is probably the slowest and most basic .obj parser
 * ever, picked out of an old project, but it does its job
 * for now.
 *
 * The parsing is realized by boost Spirit X3.
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

#ifndef SHADERIDE_GL_LOADERS_OBJMESHLOADER_HPP
#define SHADERIDE_GL_LOADERS_OBJMESHLOADER_HPP

#include <string>
#include <fstream>
#include <QString>
#include "src/GL/World/Mesh.hpp"

namespace ShaderIDE::GL {

    struct IndexContainer {
        uint32_t numVertexIndices;
        uint32_t numTextureIndices;
        uint32_t numNormalIndices;
    };

    class OBJMeshLoader {
    public:
        explicit OBJMeshLoader(const QString &path);

        Mesh GetMesh();

    private:
        Mesh mesh;
        uint32_t lineCounter;

        void ReadFile(const QString &path);
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
