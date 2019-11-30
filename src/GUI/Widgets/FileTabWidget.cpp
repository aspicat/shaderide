// Copyright (c) 2019 Aspicat - Florian Roth

#include <QDebug>
#include <QTabBar>
#include "FileTabWidget.hpp"
#include "src/GL/GLDefaults.hpp"
#include "src/Core/Memory.hpp"

using namespace ShaderIDE::GUI;

FileTabWidget::FileTabWidget(QWidget *parent)
    : QTabWidget                (parent),
      vsCodeEditor              (nullptr),
      fsCodeEditor              (nullptr),
      textureBrowser            (nullptr),
      textureBrowserToggle      (nullptr)
{
    InitCodeEditors();
    InitTextureBrowser();
    LoadTextureBrowserSlots();

    // Quickfix for the tab width to not overdraw the text.
    tabBar()->setStyleSheet("QTabBar::tab { min-width: 140px; }");
}

FileTabWidget::~FileTabWidget() {
    DestroyTextureBrowser();
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

TextureBrowser* FileTabWidget::GetTextureBrowser() {
    return textureBrowser;
}

void FileTabWidget::resizeEvent(QResizeEvent *event) {
    QTabWidget::resizeEvent(event);
    UpdateTextureBrowserGeometry();
    UpdateTextureBrowserToggleButton();
}

void FileTabWidget::sl_VSCodeChanged(const QString &code) {
    emit si_VSCodeChanged(code);
}

void FileTabWidget::sl_FSCodeChanged(const QString &code) {
    emit si_FSCodeChanged(code);
}

void FileTabWidget::sl_HideTextureBrowser() {
    textureBrowser->setVisible(false);
}

void FileTabWidget::sl_ToggleTextureBrowser() {
    textureBrowser->setVisible(!textureBrowser->isVisible());
}

void FileTabWidget::InitCodeEditors() {

    // Vertex Shader Code Editor
    vsCodeEditor = new CodeEditor();
    vsCodeEditor->LoadSyntaxFile("../config/glsl460.json");
    addTab(vsCodeEditor, "Vertex Shader");

    connect(vsCodeEditor, SIGNAL(si_CodeChanged(const QString&)),
            this, SLOT(sl_VSCodeChanged(const QString&)));

    connect(vsCodeEditor, SIGNAL(si_MouseReleased()),
            this, SLOT(sl_HideTextureBrowser()));

    // Fragment Shader Code Editor
    fsCodeEditor = new CodeEditor();
    fsCodeEditor->LoadSyntaxFile("../config/glsl460.json");
    addTab(fsCodeEditor, "Fragment Shader");

    connect(fsCodeEditor, SIGNAL(si_CodeChanged(const QString&)),
            this, SLOT(sl_FSCodeChanged(const QString&)));

    connect(fsCodeEditor, SIGNAL(si_MouseReleased()),
            this, SLOT(sl_HideTextureBrowser()));
}

void FileTabWidget::InitTextureBrowser() {

    // Texture Browser
    textureBrowser = new TextureBrowser(this);
    textureBrowser->setVisible(false);

    // Texture Browser Toggle Button
    textureBrowserToggle = new ImageButton("../assets/icons/icon-image.png", this);

    connect(textureBrowserToggle, SIGNAL(clicked()),
            this, SLOT(sl_ToggleTextureBrowser()));
}

void FileTabWidget::DestroyTextureBrowser() {
    Memory::Release(textureBrowserToggle);
    Memory::Release(textureBrowser);
}

void FileTabWidget::DestroyCodeEditors() {
    Memory::Release(fsCodeEditor);
    Memory::Release(vsCodeEditor);
}

void FileTabWidget::LoadTextureBrowserSlots() {
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_0_NAME, "");
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_1_NAME, "");
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_2_NAME, "");
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_3_NAME, "");
}

void FileTabWidget::UpdateTextureBrowserGeometry() {
    textureBrowser->resize(width(), textureBrowser->height());
    textureBrowser->move(0, height() - textureBrowser->height());
}

void FileTabWidget::UpdateTextureBrowserToggleButton() {
    auto right = width() - textureBrowserToggle->width() - 10;
    auto bottom = height() - textureBrowserToggle->height() - 10;
    textureBrowserToggle->move(right, bottom);
}
