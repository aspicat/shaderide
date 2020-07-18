/**
 * Mesh Class
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

#include <iostream>
#include <sstream>
#include "Mesh.hpp"
#include "src/Core/GeneralException.hpp"

using namespace ShaderIDE::GL;

void Mesh::AddVertex(const glm::vec3& vertex)
{
    vertices.push_back(vertex);
}

void Mesh::AddVertexNormal(const glm::vec3& vertexNormal)
{
    vertexNormals.push_back(vertexNormal);
}

void Mesh::AddVertexUV(const glm::vec2& vertexUV)
{
    vertexUVs.push_back(vertexUV);
}

void Mesh::AddIndex(const uint32_t& index)
{
    indices.push_back(index);
}

void Mesh::AddNormalIndex(const uint32_t& index)
{
    normalIndices.push_back(index);
}

void Mesh::AddUVIndex(const uint32_t& uvIndex)
{
    uvIndices.push_back(uvIndex);
}

std::vector<glm::vec3> Mesh::Vertices()
{
    return vertices;
}

std::vector<glm::vec3> Mesh::VertexNormals()
{
    std::vector<glm::vec3> normals;

    for (auto& vertex : vertices) {
        normals.push_back(vertex);
    }

    return normals;
}

std::vector<glm::vec3> Mesh::VertexIndexedNormals()
{
    return vertexNormals;
}

std::vector<glm::vec2> Mesh::VertexUVs()
{
    return vertexUVs;
}

std::vector<uint32_t> Mesh::Indices()
{
    return indices;
}

std::vector<uint32_t> Mesh::NormalIndices()
{
    return normalIndices;
}

std::vector<uint32_t> Mesh::UVIndices()
{
    return uvIndices;
}

VertexVec Mesh::ComposedVertices()
{
    VertexVec stride;

    // Check equal count of all indices.
    auto indexSize = indices.size();
    auto normalIndexSize = normalIndices.size();

    if (indexSize != normalIndexSize)
    {
        throw GeneralException(
                "The amount of mesh indices and normal indices are not identical."
        );
    }

    for (size_t i = 0; i < indexSize; ++i)
    {
        auto vertex = Vertex{};
        vertex.vertex = vertices.at(indices.at(i) - 1);
        vertex.normal = vertexNormals.at(normalIndices.at(i) - 1);
        stride.push_back(vertex);
    }

    // Optional: Texture UV's
    auto uvIndexSize = uvIndices.size();

    if (indexSize == uvIndexSize)
    {
        try
        {
            for (size_t i = 0; i < stride.size(); i++) {
                stride.at(i).uv = vertexUVs.at(uvIndices.at(i) - 1);
            }
        }
        catch (std::out_of_range& e)
        {
            std::cout << "Mesh has no UV's (vt) -> ";
            std::cout << "UV indices out of range: " << e.what() << "\n";
        }
    }

    return stride;
}

std::string Mesh::ComposedVerticesString()
{
    std::stringstream stream;

    for (const auto& vtx : ComposedVertices())
    {
        stream << "Vertex(";

        stream << vtx.vertex.x << ", " <<
               vtx.vertex.y << ", " <<
               vtx.vertex.z << ", ";

        stream << vtx.normal.x << ", " <<
               vtx.normal.y << ", " <<
               vtx.normal.z << ", ";

        stream << vtx.uv.x << ", " <<
               vtx.uv.y << "),\n";
    }

    auto str = stream.str();

    // Cut off last ",\n".
    return str.substr(0, str.length() - 2);
}
