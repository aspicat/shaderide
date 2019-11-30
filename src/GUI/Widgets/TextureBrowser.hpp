/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * TextureBrowser Class
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

    class TextureBrowser : public QWidget {
        Q_OBJECT
    public:
        explicit TextureBrowser(QWidget *parent = nullptr);
        ~TextureBrowser() override;

        void AddImage(const QString &name, const QString &path);
        TextureBrowserImage* GetImage(const QString &name);
        QMap<QString, TextureBrowserImage*> Images();
        void RemoveImage(const QString &name);

        void ClearImages();

    signals:
        void si_ImageChanged(TextureBrowserImage *image);
        void si_ImageCleared(TextureBrowserImage *image);
        void si_ImagePathChanged(TextureBrowserImage *image);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private slots:
        void sl_ImageChanged(TextureBrowserImage *image);
        void sl_ImageCleared(TextureBrowserImage *image);
        void sl_ImagePathChanged(TextureBrowserImage *image);

    private:
        QVBoxLayout *mainLayout;
        QWidget *scrollWidget;
        QHBoxLayout *scrollLayout;
        QScrollArea *scrollArea;
        QMap<QString, TextureBrowserImage*> images;

        void InitLayout();

        void DestroyImages();
        void DestroyLayout();

        void ResizeScrollWidget();
    };
}

#endif // SHADERIDE_GUI_WIDGETS_TEXTUREBROWSER_HPP
