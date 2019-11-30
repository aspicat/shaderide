/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * SyntaxHighlighter Class
 */

#ifndef SHADERIDE_GUI_CODE_SYNTAXHIGHLIGHTER_HPP
#define SHADERIDE_GUI_CODE_SYNTAXHIGHLIGHTER_HPP

#include <QSyntaxHighlighter>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

namespace ShaderIDE::GUI {

    struct MatchBlock {
        QTextCharFormat format;
        QString expression;
    };

    class SyntaxHighlighter : public QSyntaxHighlighter {
    public:
        explicit SyntaxHighlighter(QObject *parent);
        explicit SyntaxHighlighter(QTextDocument *parent);

        void LoadSyntaxFile(const QString &path);
        void AddMatchBlock(const MatchBlock &matchBlock);

    protected:
        void highlightBlock(const QString &text) override;

    private:
        QVector<MatchBlock> matchBlocks;

        void HighlightMatchBlocks(const QString &text);
        void ApplyFromSyntaxFile(const QJsonObject &json);

        static QString MakeRegexKeywordExpression(const QJsonArray &keywords);
    };
}

#endif // SHADERIDE_GUI_CODE_SYNTAXHIGHLIGHTER_HPP
