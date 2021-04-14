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

#ifndef SHADERIDE_GUI_CODE_CODEEDITOR_HPP
#define SHADERIDE_GUI_CODE_CODEEDITOR_HPP

#include <QPlainTextEdit>
#include <QShortcut>
#include "SyntaxHighlighter.hpp"
#include "LineNumberArea.hpp"

namespace ShaderIDE::GUI {

    class CodeEditor : public QPlainTextEdit
    {
        Q_OBJECT

        friend class LineNumberArea;

        static constexpr float FONT_SIZE_MIN = 6.0f;
        static constexpr float FONT_SIZE_MAX = 100.0f;

    public:
        explicit CodeEditor(QWidget* parent = nullptr);
        ~CodeEditor() override;

        void LoadSyntaxFile(const QString& path);

        void HighlightErrorLine(const int& line, const QColor& color);
        void ResetErrorLines();

        void ToggleWordWrap();

        void Find(const QString& text,
                  bool caseSensitive = false,
                  bool words = false);

        void ResetSearchLines();

    signals:
        void NotifyCodeChanged(const QString& code);
        void NotifyMouseReleased();

    public slots:
        void OnIncreaseFontSize();
        void OnDecreaseFontSize();

    private slots:
        void OnCodeChanged();
        void OnScrolled(int value);
        void OnLineNumberAreaUpdated();
        void OnCursorPositionChanged();

    protected:
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void contextMenuEvent(QContextMenuEvent* event) override;
        void resizeEvent(QResizeEvent* event) override;
        void paintEvent(QPaintEvent* event) override;

    private:
        float fontSize{ 10.0f };
        bool fontScalingEnabled{ false };

        SyntaxHighlighter* syntaxHighlighter{ nullptr };
        LineNumberArea* lineNumberArea{ nullptr };
        QList<QTextEdit::ExtraSelection> errorLines;
        QList<QTextEdit::ExtraSelection> cursorLines;
        QList<QTextEdit::ExtraSelection> searchLines;

        QShortcut* increaseFontSizeSC{ nullptr };
        QShortcut* decreaseFontSizeSC{ nullptr };

        void InitSyntaxHighlighter();
        void InitLineNumberArea();
        void InitShortcuts();

        void ReplaceTabStopsWithSpaces();
        void UpdateLineNumberArea();
        void HighlightCursorLine();

        void EnableFontScaling();
        void DisableFontScaling();
        void ApplyFontScale(const float& value);

        void ApplyExtraSelections();
    };
}

#endif // SHADERIDE_GUI_CODE_CODEEDITOR_HPP
