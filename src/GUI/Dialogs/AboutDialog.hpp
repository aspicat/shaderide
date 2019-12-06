/**
 * AboutDialog Class
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

#ifndef SHADERIDE_GUI_DIALOGS_ABOUTDIALOG_HPP
#define SHADERIDE_GUI_DIALOGS_ABOUTDIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

namespace ShaderIDE::GUI {

    class AboutDialog : public QDialog {
    public:
        explicit AboutDialog(QWidget *parent = nullptr);
        ~AboutDialog() override;

    private:
        QVBoxLayout *mainLayout;
        QLabel *logoLabel;
        QLabel *versionLabel;
        QLabel *copyrightLabel;
        QLabel *licenseLinkLabel;
        QSpacerItem *bottomLayoutSpacer;

        QHBoxLayout *bottomLayout;
        QLabel *linkLabel;
        QPushButton *closeButton;

        void InitLayout();
        void InitBottomLayout();

        void DestroyBottomLayout();
        void DestroyLayout();
    };
}

#endif // SHADERIDE_GUI_DIALOGS_ABOUTDIALOG_HPP
