/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * LogOutputWidget Class
 */

#ifndef SHADERIDE_GUI_LOGOUTPUTWIDGET_HPP
#define SHADERIDE_GUI_LOGOUTPUTWIDGET_HPP

#include <QPlainTextEdit>
#include <QMenu>
#include <QAction>
#include "src/GUI/Code/SyntaxHighlighter.hpp"
#include "src/GL/GLSLCompileError.hpp"

using namespace ShaderIDE::GL;

namespace ShaderIDE::GUI {

    class LogOutputWidget : public QPlainTextEdit {
        Q_OBJECT
    public:
        explicit LogOutputWidget(QWidget *parent = nullptr);
        ~LogOutputWidget() override;

        void LogMessage(const QString &message);
        void LogSuccessMessage(const QString &message);
        void LogErrorMessage(const QString &message);
        void LogGLSLError(GLSLCompileError &error);

    protected:
        void contextMenuEvent(QContextMenuEvent *event) override;

    private slots:
        void sl_ClearLog();

    private:
        SyntaxHighlighter *syntaxHighlighter;
        QMenu *contextMenu;
        QAction *clearLogAction;

        void InitFont();
        void InitSyntaxHighlighter();
        void InitContextMenu();

        void DestroyContextMenu();
        void DestroySyntaxHighlighter();
    };
}

#endif // SHADERIDE_GUI_LOGOUTPUTWIDGET_HPP
