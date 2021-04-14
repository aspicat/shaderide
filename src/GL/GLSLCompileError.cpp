/**
 * GLSLCompileError Class
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

#include <QRegularExpression>
#include "GLSLCompileError.hpp"

using namespace ShaderIDE::GL;

GLSLCompileError::GLSLCompileError(const ShaderType& shaderType, const QString& rawError)
    : type(shaderType)
{
    Parse(rawError);
}

ShaderType GLSLCompileError::GetShaderType()
{
    return type;
}

uint32_t GLSLCompileError::Line()
{
    return line;
}

QString GLSLCompileError::Code()
{
    return code;
}

QString GLSLCompileError::Message()
{
    return message;
}

QString GLSLCompileError::Raw()
{
    return raw;
}

void GLSLCompileError::Parse(const QString& rawError)
{
    // 0(0) : error C0000: MESSAGE
    auto errorPattern = R"(\((\d+)\)\s*:\s*error\s*([C0-9]+)\s*:\s*(.*))";

    QRegularExpression expr(errorPattern);
    auto matches = expr.globalMatch(rawError);

    while (matches.hasNext())
    {
        auto match = matches.next();
        line = match.captured(1).toUInt();
        code = match.captured(2);
        message = match.captured(3);
    }

    raw = rawError;
}
