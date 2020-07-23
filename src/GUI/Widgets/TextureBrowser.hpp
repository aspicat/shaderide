/**
 * TextureBrowser Class
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

#ifndef SHADERIDE_GUI_WIDGETS_TEXTUREBROWSER_HPP
#define SHADERIDE_GUI_WIDGETS_TEXTUREBROWSER_HPP

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include "TextureBrowserImage.hpp"

namespace ShaderIDE::GUI {

    class TextureBrowser : public QWidget
    {
        Q_OBJECT
        static constexpr int IMAGE_SPACING = 20;

    public:
        explicit TextureBrowser(QWidget* parent = nullptr);
        ~TextureBrowser() override;

        void AddImage(const QString& name, const QString& path);
        TextureBrowserImage* GetImage(const QString& name);
        QMap<QString, TextureBrowserImage*> Images();
        void RemoveImage(const QString& name);

        void ClearImages();

    signals:
        void NotifyImageChanged(TextureBrowserImage* image);
        void NotifyImageCleared(TextureBrowserImage* image);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private slots:
        void OnImageChanged(TextureBrowserImage* image);
        void OnImageCleared(TextureBrowserImage* image);

    private:
        QVBoxLayout* mainLayout{ nullptr };
        QWidget* scrollWidget{ nullptr };
        QLabel* titleLabel{ nullptr };
        QHBoxLayout* scrollLayout{ nullptr };
        QScrollArea* scrollArea{ nullptr };
        QMap<QString, TextureBrowserImage*> images;

        void InitLayout();

        void DestroyImages();

        void ResizeScrollWidget();
    };
}

#endif // SHADERIDE_GUI_WIDGETS_TEXTUREBROWSER_HPP
