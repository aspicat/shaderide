/**
 * Shader Class
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

#include <array>
#include "Shader.hpp"
#include "src/Core/SyntaxErrorException.hpp"

using namespace ShaderIDE::GL;

ShaderSPtr Shader::MakeShared(const QString& source, const ShaderType& shaderType)
{
    return QSharedPointer<Shader>::create(source, shaderType);
}

QString Shader::ShaderTypeToString(const ShaderType& shaderType)
{
    if (shaderType == ShaderType::VertexShader) {
        return "Vertex Shader";

    } else if (shaderType == ShaderType::FragmentShader) {
        return "Fragment Shader";
    }

    return "Unknown";
}

Shader::Shader(const QString& source, const ShaderType& shaderType)
    : type(shaderType)
{
    InitializeShader();
    SetSource(source);
}

Shader::~Shader()
{
    glDeleteShader(shader);
}

void Shader::SetSource(const QString& source)
{
    cSource = source.trimmed();
}

void Shader::Compile(GLuint program)
{
    // Apply Source
    auto srcStdStr = cSource.toStdString();
    auto src = srcStdStr.c_str();
    glShaderSource(shader, 1, &src, nullptr);

    // Compile
    glCompileShader(shader);
    HandleCompilationErrors();
    glAttachShader(program, shader);
}

void Shader::InitializeShader()
{
    initializeOpenGLFunctions();
    shader = glCreateShader(static_cast<GLenum>(type));
}

void Shader::HandleCompilationErrors()
{
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        std::array<char, INFOLOG_BUFFER_SIZE> infoLog{};
        glGetShaderInfoLog(shader, INFOLOG_BUFFER_SIZE, nullptr, infoLog.data());
        throw SyntaxErrorException(ShaderTypeToString(type), QString(infoLog.data()));
    }
}
