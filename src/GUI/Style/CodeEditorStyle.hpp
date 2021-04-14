/**
 * CodeEditor Style Header
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2019 - 2021 Florian Roth (The Damn Coder)
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

#ifndef SHADERIDE_GUI_STYLE_CODEEDITORSTYLE_HPP
#define SHADERIDE_GUI_STYLE_CODEEDITORSTYLE_HPP

#include "src/GUI/StyleSheets.hpp"

// Code Editor
#define STYLE_CODEEDITOR \
    "QPlainTextEdit {" \
    "    border: none;" \
    "    color: #eaeaea;" \
    "    background: rgb(30, 30, 30);" \
    "}" \
    STYLE_SCROLLBAR

#define STYLE_CODEEDITOR_FONT_SIZE 12
#define STYLE_CODEEDITOR_HIGHLIGHT_COLOR QColor(38, 38, 38)

// Line Number Area
#define STYLE_LINENUMBERAREA_BG_COLOR QColor(40, 40, 40, 255)
#define STYLE_LINENUMBERAREA_FONT_COLOR QColor(80, 80, 80)
#define STYLE_LINENUMBERAREA_FONT_HIGHLIGHT_COLOR QColor("#4DDCFE")

#endif // SHADERIDE_GUI_STYLE_CODEEDITORSTYLE_HPP
