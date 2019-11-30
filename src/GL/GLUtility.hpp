/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * GLUtility Class
 *
 * Helpers for OpenGL.
 */

#ifndef SHADERIDE_GL_GLUTILITY_HPP
#define SHADERIDE_GL_GLUTILITY_HPP

#include <QOpenGLFunctions_4_5_Core>

namespace ShaderIDE::GL {

    class GLUtility {
    public:
        static GLsizei StrideSize(const uint8_t &count) {
            return sizeof(GLfloat) * count;
        }

        static const void* StridePtr(const uint8_t &offset) {
            return (const void*) (sizeof(GLfloat) * offset);
        }
    };
}

#endif // SHADERIDE_GL_GLUTILITY_HPP
