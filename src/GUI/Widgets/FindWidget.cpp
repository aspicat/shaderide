/**
 * FindWidget Class
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
#include "FindWidget.hpp"
#include "src/Core/Memory.hpp"
#include "src/Core/QtUtility.hpp"
#include "src/GUI/Code/CodeEditor.hpp"

using namespace ShaderIDE::GUI;

FindWidget::FindWidget(QWidget* parent)
        : QWidget(parent)
{
    InitLayout();
}

FindWidget::~FindWidget()
{
    Memory::Release(btClose);
    Memory::Release(searchLineEdit);
    Memory::Release(searchLabel);
    Memory::Release(mainLayout);
}

void FindWidget::SetCodeEditor(CodeEditor* newCodeEditor)
{
    codeEditor = newCodeEditor;

    if (lastKeyword != nullptr) {
        TriggerSearch(lastKeyword);
    }
}

void FindWidget::Show()
{
    if (codeEditor == nullptr) {
        return;
    }

    show();
    searchLineEdit->setFocus();

    TriggerSearch(lastKeyword);
}

void FindWidget::Hide()
{
    if (codeEditor != nullptr) {
        codeEditor->setFocus();
    }

    hide();
}

void FindWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QtUtility::PaintQObjectStyleSheets(this);
}

void FindWidget::OnSearchTextChanged(const QString& text)
{
    TriggerSearch(text);
}

void FindWidget::InitLayout()
{
    setFixedHeight(50);
    setObjectName("FindWidget");

    // TODO Move stylesheet to header.
    setStyleSheet(
            "#FindWidget {"
            "    border-top: 3px solid rgb(38, 38, 38);"
            "    background: rgb(35, 35, 35);"
            "}"
            ""
            "QLabel {"
            "    color: #fafafa;"
            "    background: transparent;"
            "}"
            ""
            "QLineEdit {"
            "    padding: 3px 5px;"
            "    border: 2px solid rgb(40, 40, 40);"
            "    border-radius: 5px;"
            "    color: #fafafa;"
            "}"
    );

    // Main Layout
    mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    setLayout(mainLayout);

    // Search Label
    searchLabel = new QLabel("Find in all shaders, case insensitive: ");
    mainLayout->addWidget(searchLabel, 0, Qt::AlignLeft);

    // Search Line Edit
    searchLineEdit = new QLineEdit();
    searchLineEdit->setFixedWidth(350);
    mainLayout->addWidget(searchLineEdit, 1, Qt::AlignLeft);

    connect(searchLineEdit, SIGNAL(textChanged(const QString&)),
            this, SLOT(OnSearchTextChanged(const QString&)));

    // Close Button
    btClose = new ImageButton(":/icons/icon-exit.png", this);
    mainLayout->addWidget(btClose, 0, Qt::AlignRight);

    connect(btClose, SIGNAL(clicked()),
            this, SLOT(Hide()));
}

void FindWidget::TriggerSearch(const QString& keyword)
{
    if (!isVisible()) {
        return;
    }

    if (codeEditor == nullptr || searchLineEdit == nullptr) {
        return;
    }

    // TODO Case sensitive, whole words...
    codeEditor->Find(keyword);
    lastKeyword = keyword;
}
