/**
 * Copyright (c) 2019 Aspicat - Florian Roth
 *
 * QtUtility Class
 */

#ifndef SHADERIDE_CORE_QTUTILITY_HPP
#define SHADERIDE_CORE_QTUTILITY_HPP

#include <QWidget>
#include <QStyleOption>
#include <QPainter>

namespace ShaderIDE {

    class QtUtility {
    public:

        /**
         * Paint Q_OBJECT Style Sheets
         *
         * To be used in overridden paintEvent(QPaintEvent *event) method
         * in an extended QWidget class with the Q_OBJECT macro, which
         * prevents the widget Stylesheet to be applied.
         *
         * @param QWidget* widget
         */
        static void PaintQObjectStyleSheets(QWidget *widget) {
            QStyleOption styleOption;
            styleOption.initFrom(widget);
            QPainter painter(widget);
            widget->style()->drawPrimitive(
                    QStyle::PE_Widget, &styleOption, &painter, widget
            );
        }
    };
}

#endif // SHADERIDE_CORE_QTUTILITY_HPP
