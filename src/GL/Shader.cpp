// Copyright (c) 2019 Aspicat - Florian Roth

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
