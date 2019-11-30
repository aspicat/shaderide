// Copyright (c) 2019 Aspicat - Florian Roth

#include <iostream>
#include "Mesh.hpp"
#include "src/Core/GeneralException.hpp"

using namespace ShaderIDE::GL;

void Mesh::AddVertex(const glm::vec3 &vertex) {
    vertices.push_back(vertex);
}

void Mesh::AddVertexNormal(const glm::vec3 &vertexNormal) {
    vertexNormals.push_back(vertexNormal);
}

void Mesh::AddVertexUV(const glm::vec2 &vertexUV) {
    vertexUVs.push_back(vertexUV);
}

void Mesh::AddIndex(const uint32_t &index) {
    indices.push_back(index);
}

void Mesh::AddNormalIndex(const uint32_t &index) {
    normalIndices.push_back(index);
}

void Mesh::AddUVIndex(const uint32_t &uvIndex) {
    uvIndices.push_back(uvIndex);
}

std::vector<glm::vec3> Mesh::Vertices() {
    return vertices;
}

std::vector<glm::vec3> Mesh::VertexNormals() {
    std::vector<glm::vec3> normals;

    for (auto &vertex : vertices) {
        normals.push_back(vertex);
    }

    return normals;
}

std::vector<glm::vec3> Mesh::VertexIndexedNormals() {
    return vertexNormals;
}

std::vector<glm::vec2> Mesh::VertexUVs() {
    return vertexUVs;
}

std::vector<uint32_t> Mesh::Indices() {
    return indices;
}

std::vector<uint32_t> Mesh::NormalIndices() {
    return normalIndices;
}

std::vector<uint32_t> Mesh::UVIndices() {
    return uvIndices;
}

VertexVec Mesh::ComposedVertices() {
    VertexVec stride;

    // Check equal count of all indices.
    auto indexSize = indices.size();
    auto normalIndexSize = normalIndices.size();

    if (indexSize != normalIndexSize) {
        throw GeneralException(
                "The amount of mesh indices and normal indices are not identical."
        );
    }

    for (size_t i = 0; i < indexSize; ++i) {
        auto vertex = Vertex{};
        vertex.vertex = vertices.at(indices.at(i) - 1);
        vertex.normal = vertexNormals.at(normalIndices.at(i) - 1);
        stride.push_back(vertex);
    }

    // Optional: Texture UV's
    auto uvIndexSize = uvIndices.size();

    if (indexSize == uvIndexSize) {
        try {
            for (size_t i = 0; i < stride.size(); i++) {
                stride.at(i).uv = vertexUVs.at(uvIndices.at(i) - 1);
            }
        } catch (std::out_of_range &e) {
            std::cout << "Mesh has no UV's (vt) -> ";
            std::cout << "UV indices out of range: " << e.what() << "\n";
        }
    }

    return stride;
}
