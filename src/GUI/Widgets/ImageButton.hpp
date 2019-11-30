/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * ImageButton Class
 */

#ifndef SHADERIDE_GUI_WIDGETS_IMAGEBUTTON_HPP
#define SHADERIDE_GUI_WIDGETS_IMAGEBUTTON_HPP

#include <QPushButton>

namespace ShaderIDE::GUI {

    class ImageButton : public QPushButton {
    public:
        explicit ImageButton(const QString &imagePath,
                             QWidget *parent = nullptr);

    protected:
        void ApplyIcon(const QString &imagePath);
    };
}

#endif // SHADERIDE_GUI_WIDGETS_IMAGEBUTTON_HPP
