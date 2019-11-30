/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * TextureBrowserImage Class
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
