/**
 * Shader Class
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

#include <array>
#include "Shader.hpp"
#include "src/Core/SyntaxErrorException.hpp"

using namespace ShaderIDE::GL;

ShaderSPtr Shader::MakeShared(const QString &source, GLenum shaderType) {
    return QSharedPointer<Shader>::create(source, shaderType);
}

Shader::Shader(const QString &source, GLenum type)
    : shader(0)
{
    InitializeShader(type);
    SetSource(source);
}

Shader::~Shader() {
    glDeleteShader(shader);
}

void Shader::SetSource(const QString &source) {
    cSource = source.trimmed().toStdString();
}

void Shader::Compile(GLuint program) {

    // Apply Source
    auto src = cSource.c_str();
    glShaderSource(shader, 1, &src, nullptr);

    // Compile
    glCompileShader(shader);
    HandleCompilationErrors();
    glAttachShader(program, shader);
}

void Shader::SetFile(const std::string &newFile) {
    file = newFile;
}

std::string Shader::File() {
    return file;
}

void Shader::InitializeShader(GLenum type) {
    initializeOpenGLFunctions();
    shader = glCreateShader(type);
}

void Shader::HandleCompilationErrors() {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        std::array<char, INFOLOG_BUFFER_SIZE> infoLog{};
        glGetShaderInfoLog(shader, INFOLOG_BUFFER_SIZE, nullptr, infoLog.data());
        throw SyntaxErrorException(file, std::string(infoLog.data()));
    }
}
