// Copyright (c) 2019 Aspicat - Florian Roth

#include <QRegularExpression>
#include "GLSLCompileError.hpp"

using namespace ShaderIDE::GL;

GLSLCompileError::GLSLCompileError(const QString &file, const QString &rawError)
    : line      (0),
      code      (""),
      message   (""),
      raw       (""),
      file      (file)
{
    Parse(rawError);
}

uint32_t GLSLCompileError::Line() {
    return line;
}

QString GLSLCompileError::Code() {
    return code;
}

QString GLSLCompileError::Message() {
    return message;
}

QString GLSLCompileError::Raw() {
    return raw;
}

QString GLSLCompileError::File() {
    return file;
}

void GLSLCompileError::Parse(const QString &rawError) {
    // 0(0) : error C0000: MESSAGE
    auto errorPattern = R"(\((\d+)\)\s*:\s*error\s*([C0-9]+)\s*:\s*(.*))";

    QRegularExpression expr(errorPattern);
    auto matches = expr.globalMatch(rawError);

    while (matches.hasNext()) {
        auto match = matches.next();
        line       = match.captured(1).toUInt();
        code       = match.captured(2);
        message    = match.captured(3);
    }

    raw = rawError;
}
