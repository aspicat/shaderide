/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * LineNumberArea Class
 */

#ifndef SHADERIDE_GUI_CODE_LINENUMBERAREA_HPP
#define SHADERIDE_GUI_CODE_LINENUMBERAREA_HPP

#include <QWidget>

namespace ShaderIDE::GUI {

    // CodeEditor Class Forward Declaration
    class CodeEditor;

    class LineNumberArea : public QWidget {
        Q_OBJECT
    public:
        explicit LineNumberArea(CodeEditor *codeEditor);

    public slots:
        void sl_CodeChanged(const QString &code);

    protected:
        void paintEvent(QPaintEvent *event) override;

    private:
        CodeEditor *codeEditor;

        void DrawBackground(QPainter &painter);
        void DrawBlocks(QPainter &painter);
    };
}

#endif // SHADERIDE_GUI_CODE_LINENUMBERAREA_HPP
