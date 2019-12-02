/**
 * TextureBrowserImage Class
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
#include <QMouseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include "TextureBrowserImage.hpp"
#include "src/Core/Memory.hpp"
#include "src/Core/QtUtility.hpp"
#include "src/GUI/StyleSheets.hpp"

using namespace ShaderIDE::GUI;

TextureBrowserImage::TextureBrowserImage(const QString &name,
                                         const QString &path,
                                         QWidget *parent)
    : QWidget       (parent),
      mainLayout    (nullptr),
      imageLabel    (nullptr),
      nameLabel     (nullptr),
      imageHQ       (nullptr),
      path          (path)
{
    InitLayout();
    InitImageLabel(path);
    InitNameLabel(name);
}

TextureBrowserImage::TextureBrowserImage(const QString &name,
                                         const QImage &image,
                                         QWidget *parent)
    : QWidget       (parent),
      mainLayout    (nullptr),
      imageLabel    (nullptr),
      nameLabel     (nullptr),
      imageHQ       (nullptr),
      path          ("")
{
    InitLayout();
    InitImageLabel(image);
    InitNameLabel(name);
}

TextureBrowserImage::~TextureBrowserImage() {
    DestroyLayout();
}

QString TextureBrowserImage::Name() {
    return nameLabel->text();
}

QImage TextureBrowserImage::Image() {
    return imageLabel->pixmap()->toImage();
}

QImage TextureBrowserImage::ImageHQ() {
    return imageHQ;
}

QString TextureBrowserImage::Path() {
    return path;
}

void TextureBrowserImage::UpdateImage(const QImage &image) {
    imageHQ = image;
    imageLabel->setPixmap(QPixmap::fromImage(MakeThumbnail(image)));
    emit si_ImageChanged(this);
}

void TextureBrowserImage::SetPath(const QString &newPath) {
    path = newPath;
    emit si_PathChanged(this);
}

void TextureBrowserImage::sl_ClearImage() {
    SetPath("");
    imageHQ = QImage();
    imageLabel->setPixmap(QPixmap());
    imageLabel->setText(STYLE_TEXTUREBROWSERIMAGE_NOIMAGE_TEXT);
    emit si_ImageCleared(this);
}

void TextureBrowserImage::paintEvent(QPaintEvent *event) {
    QtUtility::PaintQObjectStyleSheets(this);
}

void TextureBrowserImage::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);

    if (event->button() == Qt::MouseButton::LeftButton) {
        OpenImageFileDialog();
    }

    if (event->button() == Qt::MouseButton::RightButton) {
        ShowContextMenu(event->pos());
    }
}

QImage TextureBrowserImage::MakeThumbnail(const QImage &image) {
    return image.scaled(
            IMAGELABEL_SIZE,
            IMAGELABEL_SIZE,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
    );
}

void TextureBrowserImage::InitLayout() {

    // Style
    setFixedWidth(160);
    setMinimumHeight(100);
    setObjectName("TextureBrowserImage");
    setStyleSheet(STYLE_TEXTUREBROWSERIMAGE);

    // Main Layout
    mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignCenter);
    setLayout(mainLayout);
}

void TextureBrowserImage::InitImageLabel(const QString &newPath) {
    QImage image {};

    if (!newPath.isEmpty()) {
        SetPath(newPath);
        image = QImage(Path());
    }

    SetImageLabelImage(image);
}

void TextureBrowserImage::InitImageLabel(const QImage &image) {
    SetImageLabelImage(image);
}

void TextureBrowserImage::InitNameLabel(const QString &name) {

    // Font
    auto font = QFont(STYLE_CODEEDITOR_FONT);
    font.setPixelSize(14);

    // Name Label
    nameLabel = new QLabel(name);
    nameLabel->setFont(font);
    nameLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(nameLabel, 1, Qt::AlignBottom);
}

void TextureBrowserImage::DestroyLayout() {
    Memory::Release(nameLabel);
    Memory::Release(imageLabel);
    Memory::Release(mainLayout);
}

void TextureBrowserImage::SetImageLabelImage(const QImage &image) {
    imageLabel = new QLabel(STYLE_TEXTUREBROWSERIMAGE_NOIMAGE_TEXT);
    imageLabel->setStyleSheet(STYLE_TEXTUREBROWSERIMAGE_LABEL);

    if (!image.isNull()) {
        imageHQ = image;
        imageLabel->setText("");
        imageLabel->setPixmap(QPixmap::fromImage(MakeThumbnail(image)));
    }

    mainLayout->addWidget(imageLabel, 1, Qt::AlignTop);
    imageLabel->resize(IMAGELABEL_SIZE, IMAGELABEL_SIZE);
    imageLabel->setAlignment(Qt::AlignCenter);
}

void TextureBrowserImage::OpenImageFileDialog() {
    auto filePath = QFileDialog::getOpenFileName(
            this,
            "Open Texture Image",
            QString(),
            QString("Image Files (*.png *.jpg *.jpeg *.tga *.gif *.bmp)")
    );

    if (!filePath.isEmpty()) {
        SetPath(filePath);
        UpdateImage(QImage(Path()));
    }
}

void TextureBrowserImage::ShowContextMenu(const QPoint &pos) {
    QMenu contextMenu {};

    // Clear Image Action
    QAction clearImageAction("Clear Image");

    connect(&clearImageAction, SIGNAL(triggered()),
            this, SLOT(sl_ClearImage()));

    if (imageLabel->pixmap() == nullptr) {
        clearImageAction.setDisabled(true);
    }

    contextMenu.addAction(&clearImageAction);

    // Exec Context Menu
    contextMenu.exec(mapToGlobal(pos));
}
