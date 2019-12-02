/**
 * FileTabWidget Class
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

#ifndef SHADERIDE_GUI_WIDGETS_FILETABWIDGET_HPP
#define SHADERIDE_GUI_WIDGETS_FILETABWIDGET_HPP

#include <QTabWidget>
#include "src/GUI/Code/CodeEditor.hpp"
#include "ImageButton.hpp"
#include "EnvSettingsPanel.hpp"

namespace ShaderIDE::GUI {

    class FileTabWidget : public QTabWidget {
        Q_OBJECT
    public:
        explicit FileTabWidget(QWidget *parent = nullptr);
        ~FileTabWidget() override;

        bool CodeEditorsReady();

        void SetVertexShaderSource(const QString &source);
        QString VertexShaderSource();

        void SetFragmentShaderSource(const QString &source);
        QString FragmentShaderSource();

        CodeEditor* GetVSCodeEditor();
        CodeEditor* GetFSCodeEditor();
        TextureBrowser* GetTextureBrowser();

        void ResetUI();

    signals:
        void si_VSCodeChanged(const QString&);
        void si_FSCodeChanged(const QString&);

    protected:
        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void sl_VSCodeChanged(const QString &code);
        void sl_FSCodeChanged(const QString &code);
        void sl_HideEnvSettingsPanel();
        void sl_ToggleEnvSettingsPanel();

    private:
        CodeEditor *vsCodeEditor;
        CodeEditor *fsCodeEditor;
        EnvSettingsPanel *envSettingsPanel;
        ImageButton *envSettingsPanelToggle;

        void InitCodeEditors();
        void InitEnvSettingsPanel();

        void DestroyEnvSettingsPanel();
        void DestroyCodeEditors();

        void UpdateEnvSettingsPanelGeometry();
        void UpdateEnvSettingsPanelToggleButton();
    };
}

#endif // SHADERIDE_GUI_WIDGETS_FILETABWIDGET_HPP
