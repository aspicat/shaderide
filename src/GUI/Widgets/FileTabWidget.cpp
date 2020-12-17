/**
 * FileTabWidget Class
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

#include <QDebug>
#include <QTabBar>
#include "FileTabWidget.hpp"
#include "src/Core/Memory.hpp"
#include "src/GUI/StyleSheets.hpp"

using namespace ShaderIDE::GUI;

FileTabWidget::FileTabWidget(QWidget* parent)
        : QTabWidget(parent)
{
    InitLayout();
    InitCodeEditors();
    InitEnvSettingsPanel();
    InitFindWidget();
    InitShortcuts();

    OnTabChanged(currentIndex());

    // Quickfix for the tab width to not overdraw the text.
    tabBar()->setStyleSheet("QTabBar::tab { min-width: 140px; }");

    connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(OnTabChanged(const int&)));
}

FileTabWidget::~FileTabWidget()
{
    Memory::Release(showFindWidgetSC);
    Memory::Release(findWidget);
    Memory::Release(findButton);
    Memory::Release(envSettingsPanelToggle);
    Memory::Release(envSettingsPanel);
    Memory::Release(fsCodeEditor);
    Memory::Release(vsCodeEditor);
}

bool FileTabWidget::CodeEditorsReady()
{
    return vsCodeEditor != nullptr && fsCodeEditor != nullptr;
}

void FileTabWidget::SetVertexShaderSource(const QString& source)
{
    vsCodeEditor->setPlainText(source);
}

QString FileTabWidget::VertexShaderSource()
{
    return vsCodeEditor->toPlainText();
}

void FileTabWidget::SetFragmentShaderSource(const QString& source)
{
    fsCodeEditor->setPlainText(source);
}

QString FileTabWidget::FragmentShaderSource()
{
    return fsCodeEditor->toPlainText();
}

void FileTabWidget::ToggleWordWrap()
{
    vsCodeEditor->ToggleWordWrap();
    fsCodeEditor->ToggleWordWrap();
}

void FileTabWidget::SetWordWrapMode(QTextOption::WrapMode& mode)
{
    vsCodeEditor->setWordWrapMode(mode);
    fsCodeEditor->setWordWrapMode(mode);
}

QTextOption::WrapMode FileTabWidget::WordWrapMode()
{
    // Word wrap mode is equal for all code editors,
    // only one code editor word wrap mode required here.
    return vsCodeEditor->wordWrapMode();
}

bool FileTabWidget::WordWrap()
{
    return WordWrapMode() == QTextOption::WordWrap;
}

CodeEditor* FileTabWidget::GetVSCodeEditor()
{
    return vsCodeEditor;
}

CodeEditor* FileTabWidget::GetFSCodeEditor()
{
    return fsCodeEditor;
}

TextureBrowser* FileTabWidget::GetTextureBrowser()
{
    return envSettingsPanel->GetTextureBrowser();
}

void FileTabWidget::ResetUI()
{
    envSettingsPanel->ResetUI();
}

void FileTabWidget::resizeEvent(QResizeEvent* event)
{
    QTabWidget::resizeEvent(event);
    UpdateEnvSettingsPanelGeometry();
    UpdateBottomButtons();
    UpdateFindWidgetGeometry();
}

void FileTabWidget::keyPressEvent(QKeyEvent* event)
{
    QTabWidget::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape)
    {
        vsCodeEditor->ResetSearchLines();
        fsCodeEditor->ResetSearchLines();
        findWidget->Hide();
    }
}

void FileTabWidget::OnVSCodeChanged(const QString& code)
{
    emit NotifyVSCodeChanged(code);
}

void FileTabWidget::OnFSCodeChanged(const QString& code)
{
    emit NotifyFSCodeChanged(code);
}

void FileTabWidget::OnHideEnvSettingsPanel()
{
    envSettingsPanel->Hide();
}

void FileTabWidget::OnFindButtonClicked()
{
    findWidget->Show();
}

void FileTabWidget::OnToggleEnvSettingsPanel()
{
    envSettingsPanel->Toggle();
}

void FileTabWidget::OnTabChanged(const int& index)
{
    switch (index)
    {
        case VS_TAB_INDEX:
            findWidget->SetCodeEditor(vsCodeEditor);
            break;

        case FS_TAB_INDEX:
            findWidget->SetCodeEditor(fsCodeEditor);
            break;

        default:
            break;
    }
}

void FileTabWidget::InitLayout()
{
    setMinimumWidth(400);
    resize(800, height());
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet(STYLE_TABWIDGET);
}

void FileTabWidget::InitCodeEditors()
{
    // Vertex Shader Code Editor
    vsCodeEditor = new CodeEditor();
    vsCodeEditor->LoadSyntaxFile(":/config/glsl460.json");
    addTab(vsCodeEditor, "Vertex Shader");

    connect(vsCodeEditor, SIGNAL(NotifyCodeChanged(const QString&)),
            this, SLOT(OnVSCodeChanged(const QString&)));

    connect(vsCodeEditor, SIGNAL(NotifyMouseReleased()),
            this, SLOT(OnHideEnvSettingsPanel()));

    // Fragment Shader Code Editor
    fsCodeEditor = new CodeEditor();
    fsCodeEditor->LoadSyntaxFile(":/config/glsl460.json");
    addTab(fsCodeEditor, "Fragment Shader");

    connect(fsCodeEditor, SIGNAL(NotifyCodeChanged(const QString&)),
            this, SLOT(OnFSCodeChanged(const QString&)));

    connect(fsCodeEditor, SIGNAL(NotifyMouseReleased()),
            this, SLOT(OnHideEnvSettingsPanel()));

    // Find Button
    findButton = new ImageButton(":/icons/icon-find.png", this);

    connect(findButton, SIGNAL(clicked()),
            this, SLOT(OnFindButtonClicked()));
}

void FileTabWidget::InitEnvSettingsPanel()
{
    envSettingsPanel = new EnvSettingsPanel(this);
    envSettingsPanel->Hide();

    envSettingsPanelToggle = new ImageButton(":/icons/icon-settings.png", this);

    connect(envSettingsPanelToggle, SIGNAL(clicked()),
            this, SLOT(OnToggleEnvSettingsPanel()));
}

void FileTabWidget::InitFindWidget()
{
    findWidget = new FindWidget(this);
    findWidget->Hide();
}

void FileTabWidget::InitShortcuts()
{
    showFindWidgetSC = new QShortcut(
            QKeySequence(Qt::CTRL | Qt::Key_F), this
    );

    connect(showFindWidgetSC, &QShortcut::activated,
            findWidget, &FindWidget::Show);
}

void FileTabWidget::UpdateEnvSettingsPanelGeometry()
{
    envSettingsPanel->resize(width(), envSettingsPanel->height());
    envSettingsPanel->move(0, height() - envSettingsPanel->height());
}

void FileTabWidget::UpdateBottomButtons()
{
    // Note: Move buttons to an extra QHBoxLayout.

    // Find Button
    auto right = width() - findButton->width() - 60;
    auto bottom = height() - findButton->height() - 10;
    findButton->move(right, bottom);

    // Env Settings Panel Toggle
    right = width() - envSettingsPanelToggle->width() - 10;
    bottom = height() - envSettingsPanelToggle->height() - 10;
    envSettingsPanelToggle->move(right, bottom);
}

void FileTabWidget::UpdateFindWidgetGeometry()
{
    findWidget->resize(width(), findWidget->height());
    findWidget->move(0, height() - findWidget->height());
}
