/**
 * QtUtility Class
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

#ifndef SHADERIDE_CORE_QTUTILITY_HPP
#define SHADERIDE_CORE_QTUTILITY_HPP

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QBuffer>

namespace ShaderIDE {

    class QtUtility
    {
    public:

        /**
         * Paint Q_OBJECT Style Sheets
         *
         * To be used in overridden paintEvent(QPaintEvent *event) method
         * in an extended QWidget class with the Q_OBJECT macro, which
         * prevents the widget Stylesheet to be applied.
         *
         * @param QWidget* widget
         */
        static void PaintQObjectStyleSheets(QWidget* widget)
        {
            QStyleOption styleOption;
            styleOption.initFrom(widget);
            QPainter painter(widget);
            widget->style()->drawPrimitive(
                    QStyle::PE_Widget, &styleOption, &painter, widget
            );
        }

        /**
         * Make base64 From Image
         *
         * Converts the given image to base64 as JPEG.
         *
         * @param QImage image
         * @return std::string
         */
        static std::string MakeBase64FromImage(QImage image)
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);

            buffer.open(QIODevice::WriteOnly);

            if (!buffer.isOpen()) {
                return "";
            }

            image.save(&buffer, "JPG");
            buffer.close();

            return QString::fromLatin1(bytes.toBase64().data()).toStdString();
        }

        /**
         * Make QImage From base64
         *
         * @param const QString& data
         * @return QImage
         */
        static QImage MakeQImageFromBase64(const QString& data)
        {
            const auto bytes = QByteArray::fromBase64(data.toLatin1());
            auto image = QImage::fromData(bytes);
            return image;
        }
    };
}

#endif // SHADERIDE_CORE_QTUTILITY_HPP
