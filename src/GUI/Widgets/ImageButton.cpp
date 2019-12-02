/**
 * ImageButton Class
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
