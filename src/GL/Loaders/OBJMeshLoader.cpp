/**
 * OBJMeshLoader Class
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2019 - 2021 Florian Roth (The Damn Coder)
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

#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <boost/spirit/home/x3.hpp>
#include "OBJMeshLoader.hpp"
#include "src/Core/GeneralException.hpp"

using namespace ShaderIDE::GL;
using namespace boost::spirit;

OBJMeshLoader::OBJMeshLoader(const QString& path)
{
    ReadFile(path);
}

Mesh OBJMeshLoader::GetMesh()
{
    return mesh;
}

void OBJMeshLoader::ReadFile(const QString& path)
{
    lineCounter = 0;

    // Load .obj File
    QFile objFile(path);

    if (!objFile.open(QIODevice::ReadOnly))
    {
        throw GeneralException(
                QString("[") + __FUNCTION__ + "] Could not open file " + path + "."
        );
    }

    // Convert the Qt text stream to the C++
    // standard stream, since it's faster.
    QTextStream ofts(&objFile);
    std::stringstream ofs;
    ofs << ofts.readAll().toStdString();

    // Parse
    std::string line;

    while (std::getline(ofs, line))
    {
        ParseLine(line);
        lineCounter++;
    }

    objFile.close();
}

void OBJMeshLoader::ParseLine(const std::string& line)
{
    // v -> vertex
    if (IsVertex(line))
    {
        mesh.AddVertex(ParseVector3(line));
        return;
    }

    // vn -> vertex normal
    if (IsVertexNormal(line))
    {
        mesh.AddVertexNormal(ParseVector3(line));
        return;
    }

    // vt -> vertex texture
    if (IsVertexTexture(line))
    {
        mesh.AddVertexUV(ParseVector2(line));
        return;
    }

    // f -> face (indices)
    if (IsFace(line))
    {
        std::vector<IndexContainer> indices = ParseIndices(line);

        for (auto& indexContainer : indices)
        {
            mesh.AddIndex(indexContainer.numVertexIndices);
            mesh.AddNormalIndex(indexContainer.numNormalIndices);
            mesh.AddUVIndex(indexContainer.numTextureIndices);
        }

        return;
    }
}

glm::vec2 OBJMeshLoader::ParseVector2(const std::string& line)
{
    std::vector<float> vectorValues;

    // Vertex Texture -> vt double double
    auto vectorParser = x3::omit[x3::lexeme[+(x3::char_("vt")) - x3::space]] >>
                        x3::double_ >> x3::double_;

    // Parse
    if (!x3::phrase_parse(line.begin(), line.end(), vectorParser, x3::space, vectorValues))
    {
        throw GeneralException(
                QString::fromStdString(std::string("[") + __FUNCTION__ + "] Could not parse vector vt on line " +
                std::to_string(lineCounter) + ".")
        );
    }

    return glm::vec2(vectorValues.at(0), vectorValues.at(1));
}

glm::vec3 OBJMeshLoader::ParseVector3(const std::string& line)
{
    std::vector<float> vectorValues;

    // Vertex, Vertex Normal -> v | vn double double double
    auto vectorParser = x3::omit[x3::lexeme[+(x3::char_("vn")) - x3::space]] >>
                        x3::double_ >> x3::double_ >> x3::double_;

    // Parse
    if (!x3::phrase_parse(line.begin(), line.end(), vectorParser, x3::space, vectorValues))
    {
        throw GeneralException(
                QString::fromStdString(std::string("[") + __FUNCTION__ + "] Could not parse vector v on line " +
                std::to_string(lineCounter) + ".")
        );
    }

    return glm::vec3(vectorValues.at(0), vectorValues.at(1), vectorValues.at(2));
}

std::vector<IndexContainer>
OBJMeshLoader::ParseIndices(const std::string& line)
{
    std::vector<uint32_t> vertexIndices;
    std::vector<uint32_t> textureIndices;
    std::vector<uint32_t> normalIndices;

    auto addVertexIndex = [&](auto& ctx) {
        vertexIndices.push_back(static_cast<uint32_t>(x3::_attr(ctx)));
    };

    auto addTextureIndex = [&](auto& ctx) {
        textureIndices.push_back(static_cast<uint32_t>(x3::_attr(ctx)));
    };

    auto addNormalIndex = [&](auto& ctx) {
        normalIndices.push_back(static_cast<uint32_t>(x3::_attr(ctx)));
    };

    // Vertex Index / Normal Index -> vi//ni
    auto vi_ni_parser = x3::int_[addVertexIndex] >>
                        x3::lit("//") >>
                        x3::int_[addNormalIndex];

    // Vertex Index / Vertex Texture Index / Normal Index -> vi/vti/ni
    auto vi_vti_ni_parser = x3::int_[addVertexIndex] >>
                            x3::lit("/") >>
                            x3::int_[addTextureIndex] >>
                            x3::lit("/") >>
                            x3::int_[addNormalIndex];

    // Parse Face Indices -> f vi//ni vi//ni vi//ni
    auto indexParser = x3::omit[x3::lexeme[+(x3::char_("f")) - x3::space]] >>
                       vi_ni_parser >> vi_ni_parser >> vi_ni_parser;

    // Parse All Lines
    if (!x3::phrase_parse(line.begin(), line.end(), indexParser, x3::space))
    {
        // Clear all indices and try again with UV index layout.
        vertexIndices.clear();
        textureIndices.clear();
        normalIndices.clear();

        // Parse Face Indices -> f vi/vti/ni vi/vti/ni vi/vti/ni
        auto uvIndexParser = x3::omit[x3::lexeme[+(x3::char_("f")) - x3::space]] >>
                             vi_vti_ni_parser >> vi_vti_ni_parser >> vi_vti_ni_parser;

        if (!x3::phrase_parse(line.begin(), line.end(), uvIndexParser, x3::space))
        {
            throw GeneralException(
                    QString::fromStdString(std::string("[") + __FUNCTION__ + "] Could not parse index on line " +
                    std::to_string(lineCounter) + ".")
            );
        }
    }

    // Move vertex & normal indices to indices pair vector
    std::vector<IndexContainer> indices;

    for (size_t i = 0; i < vertexIndices.size(); i++)
    {
        IndexContainer indexContainer{};
        indexContainer.numVertexIndices = vertexIndices.at(i);
        indexContainer.numTextureIndices = (!textureIndices.empty()) ? textureIndices.at(i) : 0;
        indexContainer.numNormalIndices = normalIndices.at(i);
        indices.push_back(indexContainer);
    }

    return indices;
}

bool OBJMeshLoader::IsVertex(const std::string& line)
{
    return IdentifyDataType(line, "v");
}

bool OBJMeshLoader::IsVertexNormal(const std::string& line)
{
    return IdentifyDataType(line, "vn");
}

bool OBJMeshLoader::IsVertexTexture(const std::string& line)
{
    return IdentifyDataType(line, "vt");
}

bool OBJMeshLoader::IsFace(const std::string& line)
{
    return IdentifyDataType(line, "f");
}

bool OBJMeshLoader::IdentifyDataType(const std::string& line, const std::string& pattern)
{
    // Minimum required length must be 3 for v[space], vn[space]...
    if (line.length() < 3) {
        return false;
    }

    // Parse .\s
    if (pattern.length() == 1)
    {
        return line.at(0) == pattern.at(0) &&
               line.at(1) == ' ';
    }

    // Parse ..\s
    if (pattern.length() == 2)
    {
        return line.at(0) == pattern.at(0) &&
               line.at(1) == pattern.at(1) &&
               line.at(2) == ' ';
    }

    return false;
}
