/**
 * SettingsDialog Class
 *
 * --------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/aspicat/shaderide.
 * --------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Aspicat - Florian Roth
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
        : QDialog(nullptr)
{
    InitLayout();
    InitViewportSection();
    InitButtonLayout();
}

SettingsDialog::~SettingsDialog()
{
    Memory::Release(btSave);
    Memory::Release(buttonLayout);
    Memory::Release(viewportRestartNote);
    Memory::Release(cboxMultisampling);
    Memory::Release(viewportForm);
    Memory::Release(viewportTitle);
    Memory::Release(viewportLayout);
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

    viewportRestartNote->setObjectName("restart_note");
    viewportRestartNote->setWordWrap(true);
    viewportLayout->addWidget(viewportRestartNote);
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
    // Multisampling, number of samples.
    mainWindow->numSamples = cboxMultisampling->itemData(cboxMultisampling->currentIndex()).toInt();
    mainWindow->SaveSettings();
}

void SettingsDialog::FetchSettings()
{
    // Multisampling, number of samples.
    switch (mainWindow->numSamples)
    {
        case 2:
            cboxMultisampling->setCurrentIndex(1);
            break;

        case 4:
            cboxMultisampling->setCurrentIndex(2);
            break;

        case 8:
            cboxMultisampling->setCurrentIndex(3);
            break;

        default:
            cboxMultisampling->setCurrentIndex(0);
    }
}
