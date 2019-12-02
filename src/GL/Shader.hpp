/**
 * Shader Class
 *
 * Wraps an OpenGL shader object and its related
 * file containing the source code. It is also
 * responsible for the compilation process.
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
