/**
 * SettingsDialog Class
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

#ifndef SHADERIDE_GUI_DIALOGS_SETTINGSDIALOG_HPP
#define SHADERIDE_GUI_DIALOGS_SETTINGSDIALOG_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>

namespace ShaderIDE::GUI {

    // MainWindow Forward Declaration
    class MainWindow;

    class SettingsDialog : public QDialog {
    Q_OBJECT
    public:
        explicit SettingsDialog(MainWindow *mainWindow);
        ~SettingsDialog() override;

    protected:
        void showEvent(QShowEvent *event) override;

    private slots:
        void sl_Save();

    private:
        MainWindow *mainWindow;
        QVBoxLayout *mainLayout;

        // 3D Viewport
        QVBoxLayout *viewportLayout;
        QLabel *viewportTitle;
        QFormLayout *viewportForm;
        QComboBox *cboxMultisampling;
        QLabel *viewportRestartNote;

        // Button Layout
        QHBoxLayout *buttonLayout;
        QPushButton *btSave;

        void InitLayout();
        void InitViewportSection();
        void InitButtonLayout();

        void Destroy();

        void ApplySettings();
        void FetchSettings();
    };
}

#endif //SHADERIDE_GUI_DIALOGS_SETTINGSDIALOG_HPP
