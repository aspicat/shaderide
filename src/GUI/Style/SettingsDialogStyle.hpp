/**
 * SettingsDialogStyle Header
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

#ifndef SHADERIDE_GUI_STYLE_SETTINGSDIALOGSTYLE_HPP
#define SHADERIDE_GUI_STYLE_SETTINGSDIALOGSTYLE_HPP

#include "src/GUI/StyleSheets.hpp"

#define STYLE_SETTINGSDIALOG \
    "QDialog {" \
    "    background: rgb(30, 30, 30);" \
    "}" \
    "QLabel {" \
    "    color: #fafafa;" \
    "}" \
    "QComboBox {" \
    "    min-width: 250px;" \
    "}" \
    ".title {" \
    "    padding: 5px;" \
    "    border-radius: 3px;" \
    "    font-size: 12pt;" \
    "    font-weight: bold;" \
    "    color: #5C9861;" \
    "    background-color: #2D2D2D;" \
    "}" \
    ".note {" \
    "    font-size: 8pt;" \
    "    color: rgb(100, 100, 100);" \
    "}" \
    STYLE_GROUPBOX

#endif // SHADERIDE_GUI_STYLE_SETTINGSDIALOGSTYLE_HPP
