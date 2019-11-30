/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * FileTabWidget Class
 */

#ifndef SHADERIDE_GUI_WIDGETS_FILETABWIDGET_HPP
#define SHADERIDE_GUI_WIDGETS_FILETABWIDGET_HPP

#include <QTabWidget>
#include "src/GUI/Code/CodeEditor.hpp"
#include "ImageButton.hpp"
#include "TextureBrowser.hpp"

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

        TextureBrowser* GetTextureBrowser();

    signals:
        void si_VSCodeChanged(const QString&);
        void si_FSCodeChanged(const QString&);

    protected:
        void resizeEvent(QResizeEvent *event) override;

    private slots:
        void sl_VSCodeChanged(const QString &code);
        void sl_FSCodeChanged(const QString &code);
        void sl_HideTextureBrowser();
        void sl_ToggleTextureBrowser();

    private:
        CodeEditor *vsCodeEditor;
        CodeEditor *fsCodeEditor;
        TextureBrowser *textureBrowser;
        ImageButton *textureBrowserToggle;

        void InitCodeEditors();
        void InitTextureBrowser();

        void DestroyTextureBrowser();
        void DestroyCodeEditors();

        void LoadTextureBrowserSlots();
        void UpdateTextureBrowserGeometry();
        void UpdateTextureBrowserToggleButton();
    };
}

#endif // SHADERIDE_GUI_WIDGETS_FILETABWIDGET_HPP
