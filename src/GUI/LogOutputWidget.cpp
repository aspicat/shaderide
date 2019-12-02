/**
 * LogOutputWidget Class
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

#include <QTextCharFormat>
#include "LogOutputWidget.hpp"
#include "StyleSheets.hpp"
#include "src/Core/Memory.hpp"

using namespace ShaderIDE::GUI;

LogOutputWidget::LogOutputWidget(QWidget *parent)
    : QPlainTextEdit    (parent),
      syntaxHighlighter (nullptr),
      contextMenu       (nullptr),
      clearLogAction    (nullptr)
{
    setReadOnly(true);
    setMinimumHeight(100);
    setMaximumHeight(600);
    setStyleSheet(STYLE_LOGOUTPUTWIDGET);

    InitFont();
    InitSyntaxHighlighter();
    InitContextMenu();
}

LogOutputWidget::~LogOutputWidget() {
    DestroyContextMenu();
    DestroySyntaxHighlighter();
}

void LogOutputWidget::LogMessage(const QString &message) {
    appendPlainText(message);
}

void LogOutputWidget::LogSuccessMessage(const QString &message) {
    appendPlainText(QString("[SUCCESS] ") + message);
}

void LogOutputWidget::LogErrorMessage(const QString &message) {
    appendPlainText(QString("[ERROR] ") + message);
}

QString LogOutputWidget::LogGLSLError(GLSLCompileError &error) {
    QString err("[GLSL] [");
    err.append(error.File());
    err.append("] Syntax error at line ");
    err.append(QString::number(error.Line()));
    err.append(": Code ").append(error.Code()).append(" ");
    err.append("-> ").append(error.Message());
    LogErrorMessage(err);
    return err;
}

void LogOutputWidget::contextMenuEvent(QContextMenuEvent *event) {
    contextMenu->exec(event->globalPos());
}

void LogOutputWidget::sl_ClearLog() {
    clear();
}

void LogOutputWidget::InitFont() {
    setFont(QFont(STYLE_LOGOUTPUTWIDGET_FONT, STYLE_LOGOUTPUTWIDGET_FONT_SIZE));
    setCenterOnScroll(true);
    setWordWrapMode(QTextOption::NoWrap);

    QFontMetrics fm(font());

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    setTabStopDistance(fm.horizontalAdvance(' ') * STYLE_TABSTOP_SIZE);
#endif
}

void LogOutputWidget::InitSyntaxHighlighter() {
    syntaxHighlighter = new SyntaxHighlighter(document());

    // Success Messages
    QTextCharFormat successFormat;
    successFormat.setFontWeight(QFont::Bold);
    successFormat.setForeground(QColor("#54CF49")); // TODO Stylesheet.

    syntaxHighlighter->AddMatchBlock({
        successFormat,
        "(\\[SUCCESS\\].*)"
    });

    // Error Messages
    QTextCharFormat errorFormat;
    errorFormat.setFontWeight(QFont::Bold);
    errorFormat.setForeground(QColor("#CF3550")); // TODO Stylesheet.

    syntaxHighlighter->AddMatchBlock({
        errorFormat,
        "(\\[ERROR\\].*)"
    });
}

void LogOutputWidget::InitContextMenu() {
    contextMenu = createStandardContextMenu();
    contextMenu->setStyleSheet(STYLE_CONTEXTMENU);

    // Clear Log Action
    clearLogAction = new QAction("Clear Log");
    contextMenu->addAction(clearLogAction);
    connect(clearLogAction, SIGNAL(triggered()),
            this, SLOT(sl_ClearLog()));
}

void LogOutputWidget::DestroyContextMenu() {
    Memory::Release(clearLogAction);
    Memory::Release(contextMenu);
}

void LogOutputWidget::DestroySyntaxHighlighter() {
    Memory::Release(syntaxHighlighter);
}
