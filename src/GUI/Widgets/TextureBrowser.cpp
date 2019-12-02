/**
 * TextureBrowser Class
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
#include <QResizeEvent>
#include "TextureBrowser.hpp"
#include "src/GUI/StyleSheets.hpp"
#include "src/Core/Memory.hpp"
#include "src/Core/QtUtility.hpp"
#include "src/Core/GeneralException.hpp"

using namespace ShaderIDE::GUI;

TextureBrowser::TextureBrowser(QWidget *parent)
    : QWidget       (parent),
      mainLayout    (nullptr),
      titleLabel    (nullptr),
      scrollWidget  (nullptr),
      scrollLayout  (nullptr),
      scrollArea    (nullptr)
{
    InitLayout();
}

TextureBrowser::~TextureBrowser() {
    DestroyImages();
    DestroyLayout();
}

void TextureBrowser::AddImage(const QString &name, const QString &path) {

    // Check if image already exists.
    if (images.find(name) != images.end()) {
        throw GeneralException(
                (QString("Image \"") + name + " already available in texture browser.")
                .toStdString()
        );
    }

    // Create and apply texture browser image.
    auto *image = new TextureBrowserImage(name, path);
    images[image->Name()] = image;
    scrollLayout->addWidget(image);
    ResizeScrollWidget();

    connect(image, SIGNAL(si_ImageChanged(TextureBrowserImage*)),
            this, SLOT(sl_ImageChanged(TextureBrowserImage*)));

    connect(image, SIGNAL(si_ImageCleared(TextureBrowserImage*)),
            this, SLOT(sl_ImageCleared(TextureBrowserImage*)));

    connect(image, SIGNAL(si_PathChanged(TextureBrowserImage*)),
            this, SLOT(sl_ImagePathChanged(TextureBrowserImage*)));
}

TextureBrowserImage* TextureBrowser::GetImage(const QString &name) {
    auto it = images.find(name);
    return (it != images.end()) ? *it : nullptr;
}

QMap<QString, TextureBrowserImage*> TextureBrowser::Images() {
    return images;
}

void TextureBrowser::RemoveImage(const QString &name) {
    auto it = images.find(name);

    if (it != images.end()) {
        images.erase(it);
        Memory::Release(*it);
    }
}

void TextureBrowser::ClearImages() {
    for (auto *image : Images()) {
        image->sl_ClearImage();
    }
}

void TextureBrowser::paintEvent(QPaintEvent *event) {
    QtUtility::PaintQObjectStyleSheets(this);
}

void TextureBrowser::sl_ImageChanged(TextureBrowserImage *image) {
    emit si_ImageChanged(image);
}

void TextureBrowser::sl_ImageCleared(TextureBrowserImage *image) {
    emit si_ImageCleared(image);
}

void TextureBrowser::sl_ImagePathChanged(TextureBrowserImage *image) {
    emit si_ImagePathChanged(image);
}

void TextureBrowser::InitLayout() {

    setMinimumHeight(240);
    setObjectName("TextureBrowser");
    setStyleSheet(STYLE_TEXTUREBROWSER);

    // Main Layout
    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignLeft);
    setLayout(mainLayout);

    // Title Label
    titleLabel = new QLabel("Texture Slots");
    titleLabel->setObjectName("Title");
    titleLabel->setContentsMargins(20, 20, 20, 0);
    mainLayout->addWidget(titleLabel);

    // Scroll Widget / Layout
    scrollWidget = new QWidget();
    scrollWidget->setFixedHeight(180);
    scrollWidget->setStyleSheet("QWidget { border: none; background: transparent; }");

    scrollLayout = new QHBoxLayout();
    scrollLayout->setAlignment(Qt::AlignLeft);
    scrollLayout->setContentsMargins(20, 20, 20, 20);
    scrollLayout->setSpacing(10);
    scrollWidget->setLayout(scrollLayout);

    // Scroll Area
    scrollArea = new QScrollArea();
    scrollArea->setStyleSheet("QWidget { border: none; background: transparent; }");
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void TextureBrowser::DestroyImages() {
    auto it = images.begin();

    while (it != images.end()) {
        auto *image = it.value();
        Memory::Release(image);
        it++;
    }

    images.clear();
}

void TextureBrowser::DestroyLayout() {
    Memory::Release(scrollArea);
}

void TextureBrowser::ResizeScrollWidget() {
    auto it = images.begin();
    int totalWidth = 0;

    while (it != images.end()) {
        auto *image = it.value();
        totalWidth += image->width() + IMAGE_SPACING;
        it++;
    }

    scrollWidget->resize(totalWidth, height());
}
