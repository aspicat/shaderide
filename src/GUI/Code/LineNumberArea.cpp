/**
 * LineNumberArea Class
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Florian Roth
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

#include <string>
#include <QDebug>
#include <QPainter>
#include <QResizeEvent>
#include "LineNumberArea.hpp"
#include "src/GUI/Code/CodeEditor.hpp"
#include "src/GUI/Style/CodeEditorStyle.hpp"

using namespace ShaderIDE::GUI;

LineNumberArea::LineNumberArea(CodeEditor* codeEditor)
        : QWidget(codeEditor),
          codeEditor(codeEditor)
{
    // Initial Size
    resize(60, height());
}

void LineNumberArea::SetFontSize(const float& newFontSize)
{
    fontSize = newFontSize;
}

void LineNumberArea::OnCodeChanged(const QString& code)
{
    update();
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    DrawBackground(painter);
    DrawBlocks(painter);
}

void LineNumberArea::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    emit NotifyUpdated();
}

void LineNumberArea::DrawBackground(QPainter& painter)
{
    painter.fillRect(rect(), QBrush(STYLE_LINENUMBERAREA_BG_COLOR));
}

void LineNumberArea::DrawBlocks(QPainter& painter)
{
    // Pen
    auto penColor = STYLE_LINENUMBERAREA_FONT_COLOR;

    QPen pen;
    pen.setColor(penColor);
    painter.setPen(pen);

    // Font
    QFont font(STYLE_LINENUMBERAREA_FONT);
    font.setPointSizeF(fontSize);
    font.setWeight(QFont::Bold);
    painter.setFont(font);

    // Font Metrics
    auto fontMetrics = codeEditor->fontMetrics();
    auto charWidth = fontMetrics.averageCharWidth();

    // Block Geometry
    auto marginTop = codeEditor->contentsMargins().top();

    auto block = codeEditor->firstVisibleBlock();
    auto blockNumber = block.blockNumber();

    auto top = static_cast<int>(codeEditor->blockBoundingGeometry(block)
                                        .translated(codeEditor->contentOffset()).top() + marginTop);

    auto bottom = top + static_cast<int>(codeEditor->blockBoundingRect(block).height());

    // ++ Loop Blocks ++
    // The layout is calculated by using one character width as margin
    // on the left and the right side -> Example: |-1-|, |-1000-|.
    while (block.isValid() && top <= rect().bottom())
    {
        if (block.isVisible() && bottom >= rect().top())
        {
            QString number = QString::number(blockNumber + 1);

            // Highlight current block position.
            if (blockNumber == codeEditor->textCursor().blockNumber()) {
                pen.setColor(STYLE_LINENUMBERAREA_FONT_HIGHLIGHT_COLOR);
            } else {
                pen.setColor(penColor);
            }

            painter.setPen(pen);

            painter.drawText(charWidth,
                             top,
                             width() - (2 * charWidth),
                             fontMetrics.height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(codeEditor->blockBoundingRect(block).height());
        blockNumber++;
    }

    // Adjust widget width by line number text length
    // plus two times the character width as margin left and right.
    auto numDigits = std::to_string(blockNumber).length();
    auto widgetWidth = numDigits * fontMetrics.averageCharWidth();
    resize(static_cast<int>(widgetWidth) + (2 * charWidth), height());

    // TODO Cleanup.
}
