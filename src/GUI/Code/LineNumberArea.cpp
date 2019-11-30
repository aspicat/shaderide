// Copyright (c) 2019 Aspicat - Florian Roth

#include <QDebug>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include "LineNumberArea.hpp"
#include "src/GUI/Code/CodeEditor.hpp"

using namespace ShaderIDE::GUI;

LineNumberArea::LineNumberArea(CodeEditor *codeEditor)
    : QWidget       (codeEditor),
      codeEditor    (codeEditor)
{
    // Initial Size
    resize(60, height());
}

void LineNumberArea::sl_CodeChanged(const QString &code) {
    update();
}

void LineNumberArea::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    DrawBackground(painter);
    DrawBlocks(painter);
}

void LineNumberArea::DrawBackground(QPainter &painter) {
    painter.fillRect(rect(), QBrush(QColor(40, 40, 40, 255)));
}

void LineNumberArea::DrawBlocks(QPainter &painter) {

    // Pen
    auto penColor = QColor(80, 80, 80); // TODO StyleSheet

    QPen pen;
    pen.setColor(penColor);
    painter.setPen(pen);

    // Font
    QFont font("Consolas"); // TODO StyleSheet
    font.setPointSize(10);
    font.setWeight(QFont::Bold);
    painter.setFont(font);

    // Block Geometry
    auto fontMetrics = codeEditor->fontMetrics();
    auto marginTop = codeEditor->contentsMargins().top();

    auto block = codeEditor->firstVisibleBlock();
    auto blockNumber = block.blockNumber();

    auto top = static_cast<int>(codeEditor->blockBoundingGeometry(block)
            .translated(codeEditor->contentOffset()).top() + marginTop);

    auto bottom = top + static_cast<int>(codeEditor->blockBoundingRect(block).height());

    // Loop Blocks
    while (block.isValid() && top <= rect().bottom()) {
        if (block.isVisible() && bottom >= rect().top()) {
            QString number = QString::number(blockNumber + 1);

            // Highlight current block position.
            if (blockNumber == codeEditor->textCursor().blockNumber()) {
                pen.setColor(QColor("#4DDCFE")); // TODO StyleSheet
            } else {
                pen.setColor(penColor);
            }

            painter.setPen(pen);
            painter.drawText(10, top, width() - 20, fontMetrics.height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(codeEditor->blockBoundingRect(block).height());
        blockNumber++;

        // Adjust width for line numbers.
        if (blockNumber > 1000 && width() < 80) {
            resize(80, height());
        } else if (width() > 80) {
            resize(60, height());
        }
    }
}
