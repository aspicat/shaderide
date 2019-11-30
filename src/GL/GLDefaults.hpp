/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * This header contains initial values for the shader
 * project and GL related constants.
 */

#ifndef SHADERIDE_GL_GLDEFAULTS_HPP
#define SHADERIDE_GL_GLDEFAULTS_HPP

#define GLSL_DEFAULT_VS_SOURCE \
    "#version 450 core\n" \
    "\n" \
    "in vec3 vPos;\n" \
    "in vec3 vNormal;\n" \
    "in vec2 vTexUV;\n" \
    "\n" \
    "layout (location = 0) uniform float renderTime;\n" \
    "layout (location = 1) uniform mat4 modelMat;\n" \
    "layout (location = 2) uniform mat4 viewMat;\n" \
    "layout (location = 3) uniform mat4 projectionMat;\n" \
    "\n" \
    "layout (location = 10) out vec3 vPosOut;\n" \
    "layout (location = 11) out vec3 vNormalOut;\n" \
    "layout (location = 12) out vec2 vTexUVOut;\n" \
    "layout (location = 13) out mat4 mvpOut;\n" \
    "\n" \
    "void main() {\n" \
    "    mat4 mvp    = projectionMat * viewMat * modelMat;\n" \
    "    vPosOut     = vPos;\n" \
    "    vNormalOut  = vNormal;\n" \
    "    mvpOut      = mvp;\n" \
    "    vTexUVOut   = vTexUV;\n" \
    "    gl_Position = mvp * vec4(vPos, 1.f);\n" \
    "}"

#define GLSL_DEFAULT_FS_SOURCE \
    "#version 450 core\n" \
    "\n" \
    "uniform sampler2D tex0;\n" \
    "uniform sampler2D tex1;\n" \
    "uniform sampler2D tex2;\n" \
    "uniform sampler2D tex3;\n" \
    "\n" \
    "layout (location = 0) uniform float renderTime;\n" \
    "layout (location = 1) uniform mat4 modelMat;\n" \
    "layout (location = 2) uniform mat4 viewMat;\n" \
    "layout (location = 3) uniform mat4 projectionMat;\n" \
    "\n" \
    "layout (location = 10) in vec3 vPos;\n" \
    "layout (location = 11) in vec3 vNormal;\n" \
    "layout (location = 12) in vec2 vTexUV;\n" \
    "layout (location = 13) in mat4 mvp;\n" \
    "\n" \
    "out vec4 fragColor;\n" \
    "\n" \
    "void main() {\n" \
    "    vec4 lightPos      = vec4(-0.2f, 0.3f, 0.5f, 1.f) * modelMat;\n" \
    "    float lightInt     = 1.8f;\n" \
    "    float lightVal     = dot(lightPos.xyz, vNormal);\n" \
    "    float lightSpec    = pow(lightVal, 20) * 8000;\n" \
    "    vec4 color         = vec4(0.0f, 0.5f, 1.0f, 1.0f);\n" \
    "    vec4 tex           = texture(tex0, vTexUV);\n" \
    "    fragColor          = tex * lightVal * lightInt + lightSpec;\n" \
    "}"

#define GLSL_TEXTURE_SLOT_0_NAME "tex0"
#define GLSL_TEXTURE_SLOT_1_NAME "tex1"
#define GLSL_TEXTURE_SLOT_2_NAME "tex2"
#define GLSL_TEXTURE_SLOT_3_NAME "tex3"

#endif // SHADERIDE_GL_GLDEFAULTS_HPP
