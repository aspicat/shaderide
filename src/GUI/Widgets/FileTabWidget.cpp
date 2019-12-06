/**
 * FileTabWidget Class
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
#include <QTabBar>
#include "FileTabWidget.hpp"
#include "src/Core/Memory.hpp"
#include "src/GUI/StyleSheets.hpp"

using namespace ShaderIDE::GUI;

FileTabWidget::FileTabWidget(QWidget *parent)
    : QTabWidget                (parent),
      vsCodeEditor              (nullptr),
      fsCodeEditor              (nullptr),
      envSettingsPanel          (nullptr),
      envSettingsPanelToggle    (nullptr)
{
    InitLayout();
    InitCodeEditors();
    InitEnvSettingsPanel();

    // Quickfix for the tab width to not overdraw the text.
    tabBar()->setStyleSheet("QTabBar::tab { min-width: 140px; }");
}

FileTabWidget::~FileTabWidget() {
    DestroyEnvSettingsPanel();
    DestroyCodeEditors();
}

bool FileTabWidget::CodeEditorsReady() {
    return vsCodeEditor != nullptr && fsCodeEditor != nullptr;
}

void FileTabWidget::SetVertexShaderSource(const QString &source) {
    vsCodeEditor->setPlainText(source);
}

QString FileTabWidget::VertexShaderSource() {
    return vsCodeEditor->toPlainText();
}

void FileTabWidget::SetFragmentShaderSource(const QString &source) {
    fsCodeEditor->setPlainText(source);
}

QString FileTabWidget::FragmentShaderSource() {
    return fsCodeEditor->toPlainText();
}

void FileTabWidget::ToggleWordWrap() {
    vsCodeEditor->ToggleWordWrap();
    fsCodeEditor->ToggleWordWrap();
}

void FileTabWidget::SetWordWrapMode(QTextOption::WrapMode &mode) {
    vsCodeEditor->setWordWrapMode(mode);
    fsCodeEditor->setWordWrapMode(mode);
}

QTextOption::WrapMode FileTabWidget::WordWrapMode() {

    // Word wrap mode is equal for all code editors,
    // only one code editor word wrap mode required here.
    return vsCodeEditor->wordWrapMode();
}

bool FileTabWidget::WordWrap() {
    return WordWrapMode() == QTextOption::WordWrap;
}

CodeEditor* FileTabWidget::GetVSCodeEditor() {
    return vsCodeEditor;
}

CodeEditor* FileTabWidget::GetFSCodeEditor() {
    return fsCodeEditor;
}

TextureBrowser* FileTabWidget::GetTextureBrowser() {
    return envSettingsPanel->GetTextureBrowser();
}

void FileTabWidget::ResetUI() {
    envSettingsPanel->ResetUI();
}

void FileTabWidget::resizeEvent(QResizeEvent *event) {
    QTabWidget::resizeEvent(event);
    UpdateEnvSettingsPanelGeometry();
    UpdateEnvSettingsPanelToggleButton();
}

void FileTabWidget::sl_VSCodeChanged(const QString &code) {
    emit si_VSCodeChanged(code);
}

void FileTabWidget::sl_FSCodeChanged(const QString &code) {
    emit si_FSCodeChanged(code);
}

void FileTabWidget::sl_HideEnvSettingsPanel() {
    envSettingsPanel->Hide();
}

void FileTabWidget::sl_ToggleEnvSettingsPanel() {
    envSettingsPanel->Toggle();
}

void FileTabWidget::InitLayout() {
    setMinimumWidth(400);
    resize(800, height());
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet(STYLE_TABWIDGET);
}

void FileTabWidget::InitCodeEditors() {

    // Vertex Shader Code Editor
    vsCodeEditor = new CodeEditor();
    vsCodeEditor->LoadSyntaxFile(":/config/glsl460.json");
    addTab(vsCodeEditor, "Vertex Shader");

    connect(vsCodeEditor, SIGNAL(si_CodeChanged(const QString&)),
            this, SLOT(sl_VSCodeChanged(const QString&)));

    connect(vsCodeEditor, SIGNAL(si_MouseReleased()),
            this, SLOT(sl_HideEnvSettingsPanel()));

    // Fragment Shader Code Editor
    fsCodeEditor = new CodeEditor();
    fsCodeEditor->LoadSyntaxFile(":/config/glsl460.json");
    addTab(fsCodeEditor, "Fragment Shader");

    connect(fsCodeEditor, SIGNAL(si_CodeChanged(const QString&)),
            this, SLOT(sl_FSCodeChanged(const QString&)));

    connect(fsCodeEditor, SIGNAL(si_MouseReleased()),
            this, SLOT(sl_HideEnvSettingsPanel()));
}

void FileTabWidget::InitEnvSettingsPanel() {
    envSettingsPanel = new EnvSettingsPanel(this);
    envSettingsPanel->Hide();

    envSettingsPanelToggle = new ImageButton(":/icons/icon-settings.png", this);

    connect(envSettingsPanelToggle, SIGNAL(clicked()),
            this, SLOT(sl_ToggleEnvSettingsPanel()));
}

void FileTabWidget::DestroyEnvSettingsPanel() {
    Memory::Release(envSettingsPanelToggle);
    Memory::Release(envSettingsPanel);
}

void FileTabWidget::DestroyCodeEditors() {
    Memory::Release(fsCodeEditor);
    Memory::Release(vsCodeEditor);
}

void FileTabWidget::UpdateEnvSettingsPanelGeometry() {
    envSettingsPanel->resize(width(), envSettingsPanel->height());
    envSettingsPanel->move(0, height() - envSettingsPanel->height());
}

void FileTabWidget::UpdateEnvSettingsPanelToggleButton() {
    auto right = width() - envSettingsPanelToggle->width() - 10;
    auto bottom = height() - envSettingsPanelToggle->height() - 10;
    envSettingsPanelToggle->move(right, bottom);
}
