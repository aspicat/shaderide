/**
 * SyntaxHighlighter Class
 *
 * --------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/aspicat/shaderide.
 * --------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Aspicat - Florian Roth
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

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QRegularExpression>
#include "SyntaxHighlighter.hpp"
#include "src/Core/GeneralException.hpp"

using namespace ShaderIDE::GUI;

SyntaxHighlighter::SyntaxHighlighter(QObject* parent)
        : QSyntaxHighlighter(parent)
{}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* parent)
        : QSyntaxHighlighter(parent)
{}

void SyntaxHighlighter::LoadSyntaxFile(const QString& path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!file.isOpen())
    {
        throw GeneralException(
                QString("Could not open syntax file \"")
                        .append(path)
                        .append("\".")
                        .toStdString()
        );
    }

    auto content = file.readAll();
    auto jsonDocument = QJsonDocument::fromJson(content);
    ApplyFromSyntaxFile(jsonDocument.object());
    file.close();
}

void SyntaxHighlighter::AddMatchBlock(const MatchBlock& matchBlock)
{
    matchBlocks.push_back(matchBlock);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    HighlightMatchBlocks(text);
}

void SyntaxHighlighter::HighlightMatchBlocks(const QString& text)
{
    for (const auto& matchBlock : matchBlocks)
    {
        QRegularExpression expr(matchBlock.expression);
        auto it = expr.globalMatch(text);

        while (it.hasNext())
        {
            auto match = it.next();
            setFormat(match.capturedStart(),
                      match.capturedLength(),
                      matchBlock.format);
        }
    }
}

void SyntaxHighlighter::ApplyFromSyntaxFile(const QJsonObject& json)
{
    // Spec
    auto spec = json.value("spec").toString();

    // Scalar
    auto scalarColor = json.value("scalar_color").toString();
    QTextCharFormat scalarFormat;
    scalarFormat.setForeground(QColor(scalarColor));
    AddMatchBlock({ scalarFormat, "-?\\b(\\d+\\.?\\d*f?)\\b" });

    // Preprocessor
    auto preprocessorColor = json.value("preprocessor_color").toString();
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setFontWeight(QFont::Bold);
    preprocessorFormat.setForeground(QColor(preprocessorColor));
    AddMatchBlock({ preprocessorFormat, "(^\\s*\\#.*)" });

    // Macros
    auto macrosColor = json.value("macros_color").toString();
    auto macros = json.value("macros").toArray();
    QTextCharFormat macroFormat;
    macroFormat.setForeground(QColor(macrosColor));
    AddMatchBlock({ macroFormat, MakeRegexKeywordExpression(macros) });

    // Types
    auto typesColor = json.value("types_color").toString();
    auto types = json.value("types").toArray();
    QTextCharFormat typeFormat;
    typeFormat.setFontWeight(QFont::Bold);
    typeFormat.setForeground(QColor(typesColor));
    AddMatchBlock({ typeFormat, MakeRegexKeywordExpression(types) });

    // Qualifiers
    auto qualifiersColor = json.value("qualifiers_color").toString();
    auto qualifiers = json.value("qualifiers").toArray();
    QTextCharFormat qualifierFormat;
    qualifierFormat.setFontWeight(QFont::Bold);
    qualifierFormat.setForeground(QColor(qualifiersColor));
    AddMatchBlock({ qualifierFormat, MakeRegexKeywordExpression(qualifiers) });

    // Built-In Vars
    auto builtinVarsColor = json.value("builtin_vars_color").toString();
    auto builtinVars = json.value("builtin_vars").toArray();
    QTextCharFormat builtinVarsFormat;
    builtinVarsFormat.setFontWeight(QFont::Bold);
    builtinVarsFormat.setForeground(QColor(builtinVarsColor));
    AddMatchBlock({ builtinVarsFormat, MakeRegexKeywordExpression(builtinVars) });

    // Functions
    auto functionsColor = json.value("functions_color").toString();
    auto functions = json.value("functions").toArray();
    QTextCharFormat functionsFormat;
    functionsFormat.setForeground(QColor(functionsColor));
    AddMatchBlock({ functionsFormat, MakeRegexKeywordExpression(functions) });

    // Texture Functions
    auto textureFunctionsColor = json.value("texture_functions_color").toString();
    auto textureFunctions = json.value("texture_functions").toArray();
    QTextCharFormat textureFunctionsFormat;
    textureFunctionsFormat.setForeground(QColor(textureFunctionsColor));
    AddMatchBlock({ textureFunctionsFormat, MakeRegexKeywordExpression(textureFunctions) });

    // Comments
    auto commentsColor = json.value("comments_color").toString();
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(commentsColor));
    // TODO Multiline Comments
    AddMatchBlock({ commentFormat, "(\\/\\/.*)" });
}

QString SyntaxHighlighter::MakeRegexKeywordExpression(const QJsonArray& keywords)
{
    QString expr("\\b(");

    for (auto i = 0; i < keywords.size(); ++i)
    {
        expr.append(keywords.at(i).toString());

        if (i + 1 < keywords.size()) {
            expr.append("|");
        }
    }

    return expr.append(")\\b");
}
