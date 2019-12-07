/**
 * TextureBrowser Style Header
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

#ifndef SHADERIDE_GUI_STYLE_TEXTUREBROWSERSTYLE_HPP
#define SHADERIDE_GUI_STYLE_TEXTUREBROWSERSTYLE_HPP

#include "src/GUI/StyleSheets.hpp"

#ifdef WIN32
#define STYLE_TEXTUREBROWSER_FONT "Consolas"
#else
#define STYLE_TEXTUREBROWSER_FONT "DejaVu Sans Mono"
#endif

#define STYLE_TEXTUREBROWSER \
    "#TextureBrowser {" \
    "    background: transparent;" \
    "}"

#define STYLE_TEXTUREBROWSERIMAGE \
    "#TextureBrowserImage {" \
    "    border: none;" \
    "    border-radius: 4px;" \
    "    background: rgba(30, 30, 30, 0.5);" \
    "}" \
    "#TextureBrowserImage::hover {" \
    "    border: 2px solid #233151;" \
    "    background: #233151;" \
    "}" \
    "QLabel {" \
    "    color: #fafafa;" \
    "    background: transparent;" \
    "}"

#define STYLE_TEXTUREBROWSERIMAGE_LABEL \
    "color: #656565;"

#define STYLE_TEXTUREBROWSERIMAGE_NOIMAGE_TEXT "NO IMAGE"

#endif // SHADERIDE_GUI_STYLE_TEXTUREBROWSERSTYLE_HPP