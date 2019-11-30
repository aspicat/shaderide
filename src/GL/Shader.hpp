/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * Shader Class
 *
 * Wraps an OpenGL shader object and its related
 * file containing the source code. It is also
 * responsible for the compilation process.
 */

#ifndef SHADERIDE_GL_SHADER_HPP
#define SHADERIDE_GL_SHADER_HPP

#include <QSharedPointer>
#include <QString>
#include <QOpenGLFunctions_4_5_Core>

namespace ShaderIDE::GL {

    // Forward Declaration / ShaderSPtr
    class Shader;
    using ShaderSPtr = QSharedPointer<Shader>;

    class Shader : protected QOpenGLFunctions_4_5_Core {
        static constexpr size_t INFOLOG_BUFFER_SIZE = 1024;

    public:
        static ShaderSPtr MakeShared(const QString &source, GLenum shaderType);
        explicit Shader(const QString &source, GLenum shaderType);
        ~Shader() override;

        void SetSource(const QString &source);
        void Compile(GLuint program);

        void SetFile(const std::string &newFile);
        std::string File();

    private:
        GLuint shader;
        std::string cSource;
        std::string file;

        void InitializeShader(GLenum type);
        void HandleCompilationErrors();
    };
}

#endif // SHADERIDE_GL_SHADER_HPP
