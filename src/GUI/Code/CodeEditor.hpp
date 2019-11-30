/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * CodeEditor Class
 */

#ifndef SHADERIDE_GUI_CODE_CODEEDITOR_HPP
#define SHADERIDE_GUI_CODE_CODEEDITOR_HPP

#include <QPlainTextEdit>
#include "SyntaxHighlighter.hpp"
#include "LineNumberArea.hpp"

namespace ShaderIDE::GUI {

    class CodeEditor : public QPlainTextEdit {
        friend class LineNumberArea;
        Q_OBJECT
    public:
        explicit CodeEditor(QWidget *parent = nullptr);
        ~CodeEditor() override;

        void LoadSyntaxFile(const QString &path);

    signals:
        void si_CodeChanged(const QString &code);
        void si_MouseReleased();

    private slots:
        void sl_CodeChanged();
        void sl_Scrolled(int value);

    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void contextMenuEvent(QContextMenuEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void paintEvent(QPaintEvent *event) override;

    private:
        SyntaxHighlighter *syntaxHighlighter;
        LineNumberArea *lineNumberArea;

        void InitSyntaxHighlighter();
        void InitLineNumberArea();

        void ReplaceTabStopsWithSpaces();
        void UpdateLineNumberArea();
    };
}

#endif // SHADERIDE_GUI_CODE_CODEEDITOR_HPP
