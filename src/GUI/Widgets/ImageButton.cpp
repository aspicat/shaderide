// Copyright (c) 2019 Aspicat - Florian Roth

#include <QPixmap>
#include <QIcon>
#include "ImageButton.hpp"
#include "src/GUI/StyleSheets.hpp"

using namespace ShaderIDE::GUI;

ImageButton::ImageButton(const QString &imagePath, QWidget *parent)
    : QPushButton(parent)
{
    setStyleSheet(STYLE_IMAGEBUTTON);
    ApplyIcon(imagePath);
}

void ImageButton::ApplyIcon(const QString &imagePath) {
    setIcon(QIcon(QPixmap(imagePath)));
    setIconSize(QSize(28, 28)); // TODO
}
