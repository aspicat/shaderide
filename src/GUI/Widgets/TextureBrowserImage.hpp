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

#ifndef SHADERIDE_GUI_WIDGETS_TEXTUREBROWSERIMAGE_HPP
#define SHADERIDE_GUI_WIDGETS_TEXTUREBROWSERIMAGE_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>

namespace ShaderIDE::GUI {

    class TextureBrowserImage : public QWidget {
        static constexpr int IMAGELABEL_SIZE = 128;

        Q_OBJECT
    public:
        explicit TextureBrowserImage(const QString &name,
                                     const QString &path,
                                     QWidget *parent = nullptr);

        explicit TextureBrowserImage(const QString &name,
                                     const QImage &image,
                                     QWidget *parent = nullptr);

        ~TextureBrowserImage() override;

        QString Name();
        QImage Image();
        QImage ImageHQ();
        QString Path();

        void UpdateImage(const QImage &image);
        void SetPath(const QString &newPath);

    signals:
        void si_ImageChanged(TextureBrowserImage *image);
        void si_ImageCleared(TextureBrowserImage *image);
        void si_PathChanged(TextureBrowserImage *image);

    public slots:
        void sl_ClearImage();

    protected:
        void paintEvent(QPaintEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private:
        static QImage MakeThumbnail(const QImage &image);

        QVBoxLayout *mainLayout;
        QLabel *imageLabel;
        QLabel *nameLabel;
        QImage imageHQ;
        QString path;

        void InitLayout();
        void InitImageLabel(const QString &path);
        void InitImageLabel(const QImage &image);
        void InitNameLabel(const QString &name);

        void DestroyLayout();

        void SetImageLabelImage(const QImage &image);
        void OpenImageFileDialog();
        void ShowContextMenu(const QPoint &pos);
    };
}

#endif // SHADERIDE_GUI_WIDGETS_TEXTUREBROWSERIMAGE_HPP
