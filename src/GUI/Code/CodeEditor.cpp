// Copyright (c) 2019 Aspicat - Florian Roth

#include <QDebug>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QMenu>
#include <QScrollBar>
#include <QPainter>
#include "CodeEditor.hpp"
#include "src/GUI/StyleSheets.hpp"
#include "src/Core/Memory.hpp"

using namespace ShaderIDE::GUI;

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit    (parent),
      syntaxHighlighter (nullptr),
      lineNumberArea    (nullptr)
{
    setStyleSheet(STYLE_CODEEDITOR);
    setFont(QFont(STYLE_CODEEDITOR_FONT, STYLE_CODEEDITOR_FONT_SIZE));

    InitSyntaxHighlighter();
    InitLineNumberArea();

    connect(this, SIGNAL(textChanged()), this, SLOT(sl_CodeChanged()));

    connect(verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(sl_Scrolled(int)));

    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(sl_Scrolled(int)));
}

CodeEditor::~CodeEditor() {
    Memory::Release(lineNumberArea);
    Memory::Release(syntaxHighlighter);
}

void CodeEditor::LoadSyntaxFile(const QString &path) {
    syntaxHighlighter->LoadSyntaxFile(path);
}

void CodeEditor::sl_CodeChanged() {
    emit si_CodeChanged(toPlainText());
}

void CodeEditor::sl_Scrolled(int value) {
    lineNumberArea->update();
}

void CodeEditor::keyPressEvent(QKeyEvent *event) {
    QPlainTextEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Tab) {
        ReplaceTabStopsWithSpaces();
    }

    UpdateLineNumberArea();
}

void CodeEditor::mousePressEvent(QMouseEvent *event) {
    QPlainTextEdit::mousePressEvent(event);
    UpdateLineNumberArea();
}

void CodeEditor::mouseReleaseEvent(QMouseEvent *event) {
    QPlainTextEdit::mouseReleaseEvent(event);
    emit si_MouseReleased();
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

    connect(this, SIGNAL(si_CodeChanged(const QString&)),
            lineNumberArea, SLOT(sl_CodeChanged(const QString&)));
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
    lineNumberArea->resize(lineNumberArea->width(), height());
    lineNumberArea->update();
}
