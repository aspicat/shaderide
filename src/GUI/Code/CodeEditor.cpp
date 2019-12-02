/**
 * CodeEditor Class
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

#include <QDebug>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>
#include <QPainter>
#include "CodeEditor.hpp"
#include "src/GUI/StyleSheets.hpp"
#include "src/Core/MathUtility.hpp"
#include "src/Core/Memory.hpp"

using namespace ShaderIDE::GUI;

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit        (parent),
      syntaxHighlighter     (nullptr),
      lineNumberArea        (nullptr),
      fontSize              (10.0f),
      fontScalingEnabled    (false),
      increaseFontSizeSC    (nullptr),
      decreaseFontSizeSC    (nullptr)
{
    setStyleSheet(STYLE_CODEEDITOR);
    setFont(QFont(STYLE_CODEEDITOR_FONT, STYLE_CODEEDITOR_FONT_SIZE));

    fontSize = font().pointSizeF();

    InitSyntaxHighlighter();
    InitLineNumberArea();
    InitShortcuts();

    connect(this, SIGNAL(textChanged()), this, SLOT(sl_CodeChanged()));

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(sl_Scrolled(int)));

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(sl_Scrolled(int)));
}

CodeEditor::~CodeEditor() {
    Memory::Release(decreaseFontSizeSC);
    Memory::Release(increaseFontSizeSC);
    Memory::Release(lineNumberArea);
    Memory::Release(syntaxHighlighter);
}

void CodeEditor::LoadSyntaxFile(const QString &path) {
    syntaxHighlighter->LoadSyntaxFile(path);
}

void CodeEditor::HighlightLine(const int &line, const QColor &color) {
    QTextCursor cursor(document()->findBlockByLineNumber(line - 1));

    QTextEdit::ExtraSelection selection{};
    selection.format.setBackground(color);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = cursor;
    selection.cursor.clearSelection();
    highlightedLines.append(selection);
    setExtraSelections(highlightedLines);
}

void CodeEditor::ResetHighlightedLines() {
    highlightedLines.clear();
    setExtraSelections(highlightedLines);
}

void CodeEditor::sl_IncreaseFontSize() {
    ApplyFontScale(1.0f);
}

void CodeEditor::sl_DecreaseFontSize() {
    ApplyFontScale(-1.0f);
}

void CodeEditor::sl_CodeChanged() {
    emit si_CodeChanged(toPlainText());
}

void CodeEditor::sl_Scrolled(int value) {
    lineNumberArea->update();
}

void CodeEditor::sl_LineNumberAreaUpdated() {
    setViewportMargins(lineNumberArea->width(), 0, 0, 0);
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    QPlainTextEdit::keyPressEvent(event);

    // Tab
    if (event->key() == Qt::Key_Tab) {
        ReplaceTabStopsWithSpaces();
        UpdateLineNumberArea();
        return;
    }

    // Control
    if (event->key() == Qt::Key_Control) {
        EnableFontScaling();
        return;
    }
}

void CodeEditor::keyReleaseEvent(QKeyEvent *event) {
    QPlainTextEdit::keyReleaseEvent(event);

    // Control
    if (event->key() == Qt::Key_Control) {
        DisableFontScaling();
    }
}

void CodeEditor::mousePressEvent(QMouseEvent *event) {
    QPlainTextEdit::mousePressEvent(event);
    UpdateLineNumberArea();
}

void CodeEditor::mouseReleaseEvent(QMouseEvent *event) {
    QPlainTextEdit::mouseReleaseEvent(event);
    emit si_MouseReleased();
}

void CodeEditor::wheelEvent(QWheelEvent *event) {
    if (fontScalingEnabled) {
        ApplyFontScale(static_cast<float>(event->angleDelta().y()) / 120.0f);
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event) {
    auto contextMenu = createStandardContextMenu();
    contextMenu->setStyleSheet(STYLE_CONTEXTMENU);
    contextMenu->exec(event->globalPos());
}

void CodeEditor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);
    UpdateLineNumberArea();
}

void CodeEditor::paintEvent(QPaintEvent *event) {
    QPlainTextEdit::paintEvent(event);

    // TODO Highlight Current Line
}

void CodeEditor::InitSyntaxHighlighter() {
    syntaxHighlighter = new SyntaxHighlighter(document());
}

void CodeEditor::InitLineNumberArea() {
    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->SetFontSize(font().pointSizeF());

    connect(this, SIGNAL(si_CodeChanged(const QString&)),
            lineNumberArea, SLOT(sl_CodeChanged(const QString&)));

    connect(lineNumberArea, SIGNAL(si_Updated()),
            this, SLOT(sl_LineNumberAreaUpdated()));
}

void CodeEditor::InitShortcuts() {

    // Increase Font Size Shortcut
    increaseFontSizeSC = new QShortcut(
            QKeySequence(Qt::CTRL + Qt::Key_Plus), this
    );

    connect(increaseFontSizeSC, SIGNAL(activated()),
            this, SLOT(sl_IncreaseFontSize()));

    // Decrease Font Size Shortcut
    decreaseFontSizeSC = new QShortcut(
            QKeySequence(Qt::CTRL + Qt::Key_Minus), this
    );

    connect(decreaseFontSizeSC, SIGNAL(activated()),
            this, SLOT(sl_DecreaseFontSize()));
}

void CodeEditor::ReplaceTabStopsWithSpaces() {
    // TODO Fetch number of spaces from config.

    constexpr int numSpaces = 4;

    QString spaces{};
    spaces.resize(numSpaces, ' ');

    auto cursor     = textCursor();
    auto blockPos   = cursor.positionInBlock();
    auto lastPos    = cursor.position();
    auto nextPos    = lastPos + 3;

    // Calculate Even Tab Width
    if ((blockPos - 1) % numSpaces != 0) {
        auto offset = (blockPos - 1) % numSpaces;
        nextPos = lastPos + (numSpaces - offset - 1);
        spaces.resize(numSpaces - offset, ' ');
    }

    // Replace Tabs
    setPlainText(toPlainText().replace("\t", spaces));
    cursor.setPosition(nextPos);
    setTextCursor(cursor);
}

void CodeEditor::UpdateLineNumberArea() {
    setViewportMargins(lineNumberArea->width(), 0, 0, 0);
    lineNumberArea->SetFontSize(fontSize);
    lineNumberArea->resize(lineNumberArea->width(), height());
    lineNumberArea->update();
}

void CodeEditor::EnableFontScaling() {
    fontScalingEnabled = true;
}

void CodeEditor::DisableFontScaling() {
    fontScalingEnabled = false;
}

void CodeEditor::ApplyFontScale(const float &value) {
    fontSize = MathUtility::clamp(
            fontSize + value, FONT_SIZE_MIN, FONT_SIZE_MAX
    );

    auto newFont = font();
    newFont.setPointSizeF(fontSize);
    setFont(newFont);

    UpdateLineNumberArea();
}
