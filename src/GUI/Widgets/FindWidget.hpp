/**
 * FindWidget Class
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

#ifndef SHADERIDE_GUI_WIDGETS_FINDWIDGET_HPP
#define SHADERIDE_GUI_WIDGETS_FINDWIDGET_HPP

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "src/GUI/Widgets/ImageButton.hpp"

namespace ShaderIDE::GUI {

    // CodeEditor Forward Declaration
    class CodeEditor;

    class FindWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit FindWidget(QWidget* parent = nullptr);
        ~FindWidget() override;

        void SetCodeEditor(CodeEditor* newCodeEditor);

    public slots:
        void Show();
        void Hide();

    protected:
        void paintEvent(QPaintEvent* event) override;

    private slots:
        void OnSearchTextChanged(const QString& text);

    private:
        QString lastKeyword{ "" };
        CodeEditor* codeEditor{ nullptr };
        QHBoxLayout* mainLayout{ nullptr };
        QLabel* searchLabel{ nullptr };
        QLineEdit* searchLineEdit{ nullptr };
        ImageButton* btClose{ nullptr };

        void InitLayout();

        void TriggerSearch(const QString& text);
    };
}

#endif // SHADERIDE_GUI_WIDGETS_FINDWIDGET_HPP
