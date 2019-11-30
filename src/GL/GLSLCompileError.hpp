/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * GLSLCompileError Class
 *
 * Represents a GLSL compilation error and its information.
 */

#ifndef SHADERIDE_GL_GLSLCOMPILEERROR_HPP
#define SHADERIDE_GL_GLSLCOMPILEERROR_HPP

#include <QString>

namespace ShaderIDE::GL {

    class GLSLCompileError {
    public:
        explicit GLSLCompileError(const QString &file, const QString &rawError);

        uint32_t Line();
        QString Code();
        QString Message();
        QString Raw();
        QString File();

    private:
        uint32_t line;
        QString code;
        QString message;
        QString raw;
        QString file;

    private:
        void Parse(const QString &rawError);
    };
}

#endif // SHADERIDE_GL_GLSLCOMPILEERROR_HPP
