// Copyright (c) 2019 Aspicat - Florian Roth

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
      scrollWidget  (nullptr),
      scrollLayout  (nullptr),
      scrollArea    (nullptr)
{
    setMinimumHeight(180);
    setObjectName("TextureBrowser");
    setStyleSheet(STYLE_TEXTUREBROWSER);

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
                (QString("Image \"") + name + " already available in texture browser.").toStdString()
        );
    }

    // Create and aply texture browser image.
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

    // Main Layout
    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignLeft);
    setLayout(mainLayout);

    // Scroll Widget / Layout
    scrollWidget = new QWidget();
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
    mainLayout->addWidget(scrollArea, 1);
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
        totalWidth += it.value()->width();
        it++;
    }

    scrollWidget->resize(totalWidth, height());
}
