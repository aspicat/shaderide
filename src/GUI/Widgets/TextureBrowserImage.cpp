/**
 * TextureBrowserImage Class
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

#include <QDebug>
#include <QMouseEvent>
#include <QFileDialog>
#include <QMenu>
#include "TextureBrowserImage.hpp"
#include "src/Core/Memory.hpp"
#include "src/Core/QtUtility.hpp"
#include "src/GUI/Style/TextureBrowserStyle.hpp"

using namespace ShaderIDE::GUI;

TextureBrowserImage::TextureBrowserImage(const QString& name,
                                         const QString& path,
                                         QWidget* parent)
        : QWidget(parent)
{
    InitLayout();
    InitImageLabel(path);
    InitNameLabel(name);
}

TextureBrowserImage::TextureBrowserImage(const QString& name,
                                         const QImage& image,
                                         QWidget* parent)
        : QWidget(parent)
{
    InitLayout();
    InitImageLabel(image);
    InitNameLabel(name);
}

TextureBrowserImage::~TextureBrowserImage()
{
    Memory::Release(nameLabel);
    Memory::Release(imageLabel);
    Memory::Release(mainLayout);
}

QString TextureBrowserImage::Name()
{
    return nameLabel->text();
}

QImage TextureBrowserImage::Image()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    return imageLabel->pixmap(Qt::ReturnByValueConstant::ReturnByValue).toImage();
#else
    return imageLabel->pixmap()->toImage();
#endif
}

QImage TextureBrowserImage::ImageHQ()
{
    return imageHQ;
}

void TextureBrowserImage::UpdateImage(const QImage& image)
{
    imageHQ = image;
    imageLabel->setPixmap(QPixmap::fromImage(MakeThumbnail(image)));
    emit NotifyImageChanged(this);
}

void TextureBrowserImage::OnClearImage()
{
    imageHQ = QImage();
    imageLabel->setPixmap(QPixmap());
    imageLabel->setText(STYLE_TEXTUREBROWSERIMAGE_NOIMAGE_TEXT);
    emit NotifyImageCleared(this);
}

void TextureBrowserImage::paintEvent(QPaintEvent* event)
{
    QtUtility::PaintQObjectStyleSheets(this);
}

void TextureBrowserImage::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);

    if (event->button() == Qt::MouseButton::LeftButton) {
        OpenImageFileDialog();
    }

    if (event->button() == Qt::MouseButton::RightButton) {
        ShowContextMenu(event->pos());
    }
}

QImage TextureBrowserImage::MakeThumbnail(const QImage& image)
{
    return image.scaled(
            IMAGELABEL_SIZE,
            IMAGELABEL_SIZE,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
    );
}

void TextureBrowserImage::InitLayout()
{
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

void TextureBrowserImage::InitImageLabel(const QString& path)
{
    QImage image{};

    if (!path.isEmpty()) {
        image = QImage(path);
    }

    SetImageLabelImage(image);
}

void TextureBrowserImage::InitImageLabel(const QImage& image)
{
    SetImageLabelImage(image);
}

void TextureBrowserImage::InitNameLabel(const QString& name)
{
    // Font
    auto font = QFont(STYLE_TEXTUREBROWSER_FONT);
    font.setPixelSize(14);

    // Name Label
    nameLabel = new QLabel(name);
    nameLabel->setFont(font);
    nameLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(nameLabel, 1, Qt::AlignBottom);
}

void TextureBrowserImage::SetImageLabelImage(const QImage& image)
{
    imageLabel = new QLabel(STYLE_TEXTUREBROWSERIMAGE_NOIMAGE_TEXT);
    imageLabel->setStyleSheet(STYLE_TEXTUREBROWSERIMAGE_LABEL);

    if (!image.isNull())
    {
        imageHQ = image;
        imageLabel->setText("");
        imageLabel->setPixmap(QPixmap::fromImage(MakeThumbnail(image)));
    }

    mainLayout->addWidget(imageLabel, 1, Qt::AlignTop);
    imageLabel->resize(IMAGELABEL_SIZE, IMAGELABEL_SIZE);
    imageLabel->setAlignment(Qt::AlignCenter);
}

void TextureBrowserImage::OpenImageFileDialog()
{
    auto path = QFileDialog::getOpenFileName(
            this,
            "Open Texture Image",
            QString(),
            QString("Image Files (*.png *.jpg *.jpeg *.tga *.gif *.bmp)")
    );

    if (!path.isEmpty()) {
        UpdateImage(QImage(path));
    }
}

void TextureBrowserImage::ShowContextMenu(const QPoint& pos)
{
    QMenu contextMenu{};

    // Clear Image Action
    QAction clearImageAction("Clear Image");

    connect(&clearImageAction, SIGNAL(triggered()),
            this, SLOT(OnClearImage()));

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    if (imageLabel->pixmap(Qt::ReturnByValueConstant::ReturnByValue).isNull()) {
#else
    if (imageLabel->pixmap()->isNull()) {
#endif
        clearImageAction.setDisabled(true);
    }

    contextMenu.addAction(&clearImageAction);

    // Exec Context Menu
    contextMenu.exec(mapToGlobal(pos));
}
