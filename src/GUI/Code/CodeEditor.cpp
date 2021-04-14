/**
 * CodeEditor Class
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

#include <QDebug>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>
#include <QPainter>
#include "CodeEditor.hpp"
#include "src/GUI/Style/CodeEditorStyle.hpp"
#include "src/Core/Application.hpp"
#include "src/GUI/MainWindow.hpp"
#include "src/Core/MathUtility.hpp"
#include "src/Core/Memory.hpp"

using namespace ShaderIDE;
using namespace ShaderIDE::GUI;

CodeEditor::CodeEditor(QWidget* parent)
        : QPlainTextEdit(parent)
{
    setAcceptDrops(false);
    setStyleSheet(STYLE_CODEEDITOR);

    setFont(QFont(Application::MonospaceFontFamily(), STYLE_CODEEDITOR_FONT_SIZE));

    fontSize = font().pointSizeF();
    setWordWrapMode(QTextOption::WordWrap);

    InitSyntaxHighlighter();
    InitLineNumberArea();
    InitShortcuts();

    connect(this, SIGNAL(textChanged()),
            this, SLOT(OnCodeChanged()));

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(OnScrolled(int)));

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(OnScrolled(int)));

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(OnCursorPositionChanged()));
}

CodeEditor::~CodeEditor()
{
    Memory::Release(decreaseFontSizeSC);
    Memory::Release(increaseFontSizeSC);
    Memory::Release(lineNumberArea);
    Memory::Release(syntaxHighlighter);
}

void CodeEditor::LoadSyntaxFile(const QString& path)
{
    syntaxHighlighter->LoadSyntaxFile(path);
}

void CodeEditor::HighlightErrorLine(const int& line, const QColor& color)
{
    QTextCursor cursor(document()->findBlockByLineNumber(line - 1));

    QTextEdit::ExtraSelection selection{};
    selection.format.setBackground(color);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = cursor;
    selection.cursor.clearSelection();
    errorLines.append(selection);
    ApplyExtraSelections();
}

void CodeEditor::ResetErrorLines()
{
    errorLines.clear();
    ApplyExtraSelections();
}

void CodeEditor::ToggleWordWrap()
{
    if (wordWrapMode() == QTextOption::WordWrap) {
        setWordWrapMode(QTextOption::NoWrap);
    } else {
        setWordWrapMode(QTextOption::WordWrap);
    }
}

void CodeEditor::Find(const QString& text,
                      bool caseSensitive,
                      bool words)
{
    ResetSearchLines();

    QTextDocument::FindFlags flags;

    if (caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    if (words) {
        flags |= QTextDocument::FindWholeWords;
    }

    auto cursor = textCursor();
    const auto savedCursor = textCursor();

    cursor.movePosition(QTextCursor::Start);
    setTextCursor(cursor);

    QTextCharFormat format;
    format.setUnderlineStyle(QTextCharFormat::NoUnderline);
    format.setBackground(QBrush(QColor("#4B6F93")));

    while (find(text, flags))
    {
        QTextEdit::ExtraSelection selection{ textCursor(), format };
        searchLines.append(selection);
    }

    ApplyExtraSelections();

    // Reset text cursor to previous user position.
    setTextCursor(savedCursor);
}

void CodeEditor::ResetSearchLines()
{
    searchLines.clear();
    ApplyExtraSelections();
}

void CodeEditor::OnIncreaseFontSize()
{
    ApplyFontScale(1.0f);
}

void CodeEditor::OnDecreaseFontSize()
{
    ApplyFontScale(-1.0f);
}

void CodeEditor::OnCodeChanged()
{
    emit NotifyCodeChanged(toPlainText());
}

void CodeEditor::OnScrolled(int value)
{
    lineNumberArea->update();
}

void CodeEditor::OnLineNumberAreaUpdated()
{
    setViewportMargins(lineNumberArea->width(), 0, 0, 0);
}

void CodeEditor::OnCursorPositionChanged()
{
    HighlightCursorLine();
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
    QPlainTextEdit::keyPressEvent(event);

    // Tab
    if (event->key() == Qt::Key_Tab)
    {
        ReplaceTabStopsWithSpaces();
        UpdateLineNumberArea();
        return;
    }

    // Control
    if (event->key() == Qt::Key_Control)
    {
        EnableFontScaling();
        return;
    }
}

void CodeEditor::keyReleaseEvent(QKeyEvent* event)
{
    QPlainTextEdit::keyReleaseEvent(event);

    // Control
    if (event->key() == Qt::Key_Control) {
        DisableFontScaling();
    }
}

void CodeEditor::mousePressEvent(QMouseEvent* event)
{
    QPlainTextEdit::mousePressEvent(event);
    UpdateLineNumberArea();
}

void CodeEditor::mouseReleaseEvent(QMouseEvent* event)
{
    QPlainTextEdit::mouseReleaseEvent(event);
    emit NotifyMouseReleased();
}

void CodeEditor::wheelEvent(QWheelEvent* event)
{
    if (fontScalingEnabled)
    {
        ApplyFontScale(static_cast<float>(event->angleDelta().y()) / 120.0f);
        return;
    }

    QPlainTextEdit::wheelEvent(event);
}

void CodeEditor::contextMenuEvent(QContextMenuEvent* event)
{
    auto contextMenu = createStandardContextMenu();
    contextMenu->setStyleSheet(STYLE_CONTEXTMENU);
    contextMenu->exec(event->globalPos());
}

void CodeEditor::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);
    UpdateLineNumberArea();
}

void CodeEditor::paintEvent(QPaintEvent* event)
{
    QPlainTextEdit::paintEvent(event);
}

void CodeEditor::InitSyntaxHighlighter()
{
    syntaxHighlighter = new SyntaxHighlighter(document());
}

void CodeEditor::InitLineNumberArea()
{
    lineNumberArea = new LineNumberArea(this);
    lineNumberArea->SetFontSize(font().pointSizeF());

    connect(this, SIGNAL(NotifyCodeChanged(const QString&)),
            lineNumberArea, SLOT(OnCodeChanged(const QString&)));

    connect(lineNumberArea, SIGNAL(NotifyUpdated()),
            this, SLOT(OnLineNumberAreaUpdated()));
}

void CodeEditor::InitShortcuts()
{
    // Increase Font Size Shortcut
    increaseFontSizeSC = new QShortcut(
            QKeySequence(Qt::CTRL | Qt::Key_Plus), this
    );

    connect(increaseFontSizeSC, SIGNAL(activated()),
            this, SLOT(OnIncreaseFontSize()));

    // Decrease Font Size Shortcut
    decreaseFontSizeSC = new QShortcut(
            QKeySequence(Qt::CTRL | Qt::Key_Minus), this
    );

    connect(decreaseFontSizeSC, SIGNAL(activated()),
            this, SLOT(OnDecreaseFontSize()));
}

void CodeEditor::ReplaceTabStopsWithSpaces()
{
    auto tabWidth = SHADERIDE_CODE_EDITOR_TAB_WIDTH;

    auto* win = dynamic_cast<GUI::MainWindow*>(window());

    if (win != nullptr)
    {
        const auto appSettings = win->GetApplicationSettings();
        tabWidth = appSettings.tabWidth;
    }

    QString spaces{};
    spaces.resize(tabWidth, ' ');

    auto cursor = textCursor();
    auto blockPos = cursor.positionInBlock();

    // Calculate Even Tab Width
    if ((blockPos - 1) % tabWidth != 0)
    {
        const auto offset = (blockPos - 1) % tabWidth;
        spaces.resize(tabWidth - offset, ' ');
    }

    // Replace Tabs
    cursor.deletePreviousChar();
    cursor.insertText(spaces);
}

void CodeEditor::UpdateLineNumberArea()
{
    setViewportMargins(lineNumberArea->width(), 0, 0, 0);
    lineNumberArea->SetFontSize(fontSize);
    lineNumberArea->resize(lineNumberArea->width(), height());
    lineNumberArea->update();
}

void CodeEditor::HighlightCursorLine()
{
    cursorLines.clear();

    QTextEdit::ExtraSelection selection{};
    selection.format.setBackground(STYLE_CODEEDITOR_HIGHLIGHT_COLOR);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    cursorLines.append(selection);
    ApplyExtraSelections();
}

void CodeEditor::EnableFontScaling()
{
    fontScalingEnabled = true;
}

void CodeEditor::DisableFontScaling()
{
    fontScalingEnabled = false;
}

void CodeEditor::ApplyFontScale(const float& value)
{
    fontSize = MathUtility::clamp(fontSize + value, FONT_SIZE_MIN, FONT_SIZE_MAX);

    auto newFont = font();
    newFont.setPointSizeF(fontSize);
    setFont(newFont);

    UpdateLineNumberArea();
}

void CodeEditor::ApplyExtraSelections()
{
    QList<QTextEdit::ExtraSelection> selections{};
    selections.append(cursorLines);
    selections.append(errorLines);
    selections.append(searchLines);
    setExtraSelections(selections);
}
