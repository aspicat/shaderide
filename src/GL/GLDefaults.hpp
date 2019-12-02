/**
 * GLDefaults Header
 *
 * This header contains initial values for the shader
 * project and GL related constants.
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

#ifndef SHADERIDE_GL_GLDEFAULTS_HPP
#define SHADERIDE_GL_GLDEFAULTS_HPP

#include <glm/glm.hpp>

#define GLSL_DEFAULT_VS_SOURCE \
    "#version 450 core\n" \
    "\n" \
    "in vec3 position;\n" \
    "in vec3 normal;\n" \
    "in vec2 uv;\n" \
    "\n" \
    "uniform mat4 modelMat;\n" \
    "uniform mat4 viewMat;\n" \
    "uniform mat4 projectionMat;\n" \
    "uniform float time;\n" \
    "\n" \
    "out vec3 vPosition;\n" \
    "out vec3 vNormal;\n" \
    "out vec2 vUV;\n" \
    "out mat4 vMVP;\n" \
    "\n" \
    "void main() {\n" \
    "    mat4 mvp       = projectionMat * viewMat * modelMat;\n" \
    "    vPosition      = position;\n" \
    "    vNormal        = normal;\n" \
    "    vUV            = uv;\n" \
    "    vMVP           = mvp;\n" \
    "    gl_Position    = mvp * vec4(position, 1.0f);\n" \
    "}"

#define GLSL_DEFAULT_FS_SOURCE \
    "#version 450 core\n" \
    "\n" \
    "uniform sampler2D tex0;\n" \
    "uniform sampler2D tex1;\n" \
    "uniform sampler2D tex2;\n" \
    "uniform sampler2D tex3;\n" \
    "\n" \
    "uniform mat4 modelMat;\n" \
    "uniform mat4 viewMat;\n" \
    "uniform mat4 projectionMat;\n" \
    "uniform float time;\n" \
    "\n" \
    "in vec3 vPosition;\n" \
    "in vec3 vNormal;\n" \
    "in vec2 vUV;\n" \
    "in mat4 vMVP;\n" \
    "\n" \
    "out vec4 fragColor;\n" \
    "\n" \
    "void main() {\n" \
    "    vec4 lightPos      = vec4(-0.2f, 0.3f, 0.5f, 1.0f) * modelMat;\n" \
    "    float lightInt     = 1.8f;\n" \
    "    float lightVal     = dot(lightPos.xyz, vNormal);\n" \
    "    float lightSpec    = pow(lightVal, 20) * 8000;\n" \
    "    vec4 color         = vec4(0.0f, 0.5f, 1.0f, 1.0f);\n" \
    "    vec4 tex           = texture(tex0, vUV);\n" \
    "    fragColor          = tex * lightVal * lightInt + lightSpec;\n" \
    "}"

#define GLSL_TEXTURE_SLOT_0_NAME "tex0"
#define GLSL_TEXTURE_SLOT_1_NAME "tex1"
#define GLSL_TEXTURE_SLOT_2_NAME "tex2"
#define GLSL_TEXTURE_SLOT_3_NAME "tex3"

#define OPENGLWIDGET_DEFAULT_MODEL_ROTATION \
    glm::vec3(glm::radians(30.0f), glm::radians(45.0f), 0)

#define OPENGLWIDGET_DEFAULT_CAMERA_POSITION \
    glm::vec3(0.0f, 0.0f, -3.0f)

#endif // SHADERIDE_GL_GLDEFAULTS_HPP
