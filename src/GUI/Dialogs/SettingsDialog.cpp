/**
 * SettingsDialog Class
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

#include "SettingsDialog.hpp"
#include "src/GUI/MainWindow.hpp"
#include "src/Core/Memory.hpp"
#include "src/GUI/Style/SettingsDialogStyle.hpp"

using namespace ShaderIDE::GUI;

SettingsDialog::SettingsDialog(MainWindow* mainWindow)
        : QDialog(nullptr),
          mainWindow(mainWindow)
{
    InitLayout();
    InitViewportSection();
    InitCodeEditorSection();
    InitButtonLayout();
}

SettingsDialog::~SettingsDialog()
{
    // Button Layout
    Memory::Release(btSave);
    Memory::Release(buttonLayout);

    // Code Editor
    Memory::Release(cboxTabWidth);
    Memory::Release(codeEditorForm);
    Memory::Release(codeEditorTitle);
    Memory::Release(codeEditorLayout);

    // 3D Viewport
    Memory::Release(viewportRestartNote);
    Memory::Release(cboxMultisampling);
    Memory::Release(viewportForm);
    Memory::Release(viewportTitle);
    Memory::Release(viewportLayout);

    // Main Layout
    Memory::Release(mainLayout);
}

void SettingsDialog::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);
    FetchSettings();
}

void SettingsDialog::OnSave()
{
    ApplySettings();
    close();
}

void SettingsDialog::InitLayout()
{
    // Window
    setWindowTitle("Settings");
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedWidth(500);
    setFixedHeight(300);
    setStyleSheet(STYLE_SETTINGSDIALOG);

    // Main Layout
    mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(20);
    setLayout(mainLayout);
}

void SettingsDialog::InitViewportSection()
{
    viewportLayout = new QVBoxLayout();
    mainLayout->addLayout(viewportLayout);

    // Title
    viewportTitle = new QLabel("3D Viewport");
    viewportTitle->setProperty("class", "title");
    viewportLayout->addWidget(viewportTitle);

    // Layout
    viewportForm = new QFormLayout();
    viewportForm->setContentsMargins(0, 0, 0, 0);
    viewportForm->setSpacing(10);
    viewportLayout->addLayout(viewportForm);

    // Multisampling
    cboxMultisampling = new QComboBox();
    cboxMultisampling->setFixedWidth(120);
    cboxMultisampling->addItem("Off", 0);
    cboxMultisampling->addItem("2 Samples", 2);
    cboxMultisampling->addItem("4 Samples", 4);
    cboxMultisampling->addItem("8 Samples", 8);
    viewportForm->addRow("Multisampling", cboxMultisampling);
    viewportForm->setAlignment(cboxMultisampling, Qt::AlignRight);

    // Restart Note
    viewportRestartNote = new QLabel("Application restart required for multisampling to be applied.");

    viewportRestartNote->setProperty("class", "note");
    viewportRestartNote->setWordWrap(true);
    viewportLayout->addWidget(viewportRestartNote);
}

void SettingsDialog::InitCodeEditorSection()
{
    codeEditorLayout = new QVBoxLayout();
    mainLayout->addLayout(codeEditorLayout);

    // Title
    codeEditorTitle = new QLabel("Code Editor");
    codeEditorTitle->setProperty("class", "title");
    codeEditorLayout->addWidget(codeEditorTitle);

    // Layout
    codeEditorForm = new QFormLayout();
    codeEditorForm->setContentsMargins(0, 0, 0, 0);
    codeEditorForm->setSpacing(10);
    codeEditorLayout->addLayout(codeEditorForm);

    // Tab Width
    cboxTabWidth = new QComboBox();
    cboxTabWidth->setFixedWidth(120);
    cboxTabWidth->addItem("2 Spaces", 2);
    cboxTabWidth->addItem("4 Spaces", 4);
    cboxTabWidth->addItem("8 Spaces", 8);
    codeEditorForm->addRow("Tab Width", cboxTabWidth);
    codeEditorForm->setAlignment(cboxTabWidth, Qt::AlignRight);
}

void SettingsDialog::InitButtonLayout()
{
    buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(10);
    mainLayout->addLayout(buttonLayout, 1);

    btSave = new QPushButton("Save");
    buttonLayout->addWidget(btSave);

    connect(btSave, SIGNAL(clicked(bool)),
            this, SLOT(OnSave()));
}

void SettingsDialog::ApplySettings()
{
    // 3D Viewport
    mainWindow->applicationSettings.numSamples = cboxMultisampling->itemData(cboxMultisampling->currentIndex()).toInt();

    // Code Editor
    mainWindow->applicationSettings.tabWidth = cboxTabWidth->itemData(cboxTabWidth->currentIndex()).toInt();

    mainWindow->SaveApplicationSettings();
}

void SettingsDialog::FetchSettings()
{
    // ++++ 3D Viewport ++++

    // Multisampling, number of samples.
    if (mainWindow->applicationSettings.numSamples == 2) {
        cboxMultisampling->setCurrentIndex(1);

    } else if (mainWindow->applicationSettings.numSamples == 4) {
        cboxMultisampling->setCurrentIndex(2);

    } else if (mainWindow->applicationSettings.numSamples == 8) {
        cboxMultisampling->setCurrentIndex(3);

    } else {
        cboxMultisampling->setCurrentIndex(0);
    }

    // ++++ 3D Viewport ++++

    // Tab Width
    if (mainWindow->applicationSettings.tabWidth == 2) {
        cboxTabWidth->setCurrentIndex(0);

    } else if (mainWindow->applicationSettings.tabWidth == 4) {
        cboxTabWidth->setCurrentIndex(1);

    } else if (mainWindow->applicationSettings.tabWidth == 8) {
        cboxTabWidth->setCurrentIndex(2);
    }
}
