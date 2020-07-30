/**
 * EnvSettingsPanel Class
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

#include "EnvSettingsPanel.hpp"
#include "src/Core/QtUtility.hpp"
#include "src/GL/GLDefaults.hpp"
#include "src/Core/Memory.hpp"
#include "src/GUI/Style/EnvSettingsPanelStyle.hpp"

using namespace ShaderIDE::GUI;

EnvSettingsPanel::EnvSettingsPanel(QWidget* parent)
        : QWidget(parent)
{
    InitLayout();
    InitTextureBrowser();
}

EnvSettingsPanel::~EnvSettingsPanel()
{
    Memory::Release(textureBrowser);
    Memory::Release(mainLayout);
}

TextureBrowser* EnvSettingsPanel::GetTextureBrowser()
{
    return textureBrowser;
}

void EnvSettingsPanel::Toggle()
{
    setVisible(!isVisible());
}

void EnvSettingsPanel::Hide()
{
    setVisible(false);
}

void EnvSettingsPanel::ResetUI()
{
    textureBrowser->ClearImages();
}

void EnvSettingsPanel::paintEvent(QPaintEvent* event)
{
    QtUtility::PaintQObjectStyleSheets(this);
}

void EnvSettingsPanel::InitLayout()
{
    setMinimumHeight(250);
    setObjectName("EnvSettingsPanel");
    setStyleSheet(STYLE_ENVSETTINGSPANEL);

    // Main Layout
    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignLeft);
    setLayout(mainLayout);
}

void EnvSettingsPanel::InitTextureBrowser()
{
    textureBrowser = new TextureBrowser();
    mainLayout->addWidget(textureBrowser, 1, Qt::AlignBottom);
    LoadTextureBrowserSlots();
}

void EnvSettingsPanel::LoadTextureBrowserSlots()
{
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_0_NAME, "");
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_1_NAME, "");
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_2_NAME, "");
    textureBrowser->AddImage(GLSL_TEXTURE_SLOT_3_NAME, "");
}
