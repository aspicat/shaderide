/**
 * StyleSheets Header
 *
 * -------------------------------------------------------------------------------
 * This file is part of "Shader IDE" -> https://github.com/thedamncoder/shaderide.
 * -------------------------------------------------------------------------------
 *
 * Copyright (c) 2017 - 2020 Florian Roth
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

#ifndef SHADERIDE_GUI_STYLESHEETS_HPP
#define SHADERIDE_GUI_STYLESHEETS_HPP

// General
#define STYLE_TABSTOP_SIZE 4

// Main Window
#define STYLE_MAINWINDOW_WIDTH 1400
#define STYLE_MAINWINDOW_HEIGHT 900

#define STYLE_MAINWINDOW \
    "QWidget {" \
    "    background: rgb(28, 28, 28);" \
    "}"

// Menu Bar
#define STYLE_MENUBAR \
    "QMenuBar {" \
    "    color: #A8A8A8;" \
    "    background: rgb(32, 32, 32);" \
    "}" \
    "QMenuBar::item:selected {" \
    "    color: #4EC6D5;" \
    "    background: rgb(60, 60, 60);" \
    "}"

// Tab Widget
#define STYLE_TABWIDGET \
    "QTabWidget::pane {" \
    "    border: none;" \
    "}" \
    "QTabBar::tab {" \
    "    padding: 12px 20px;" \
    "    font-size: 12px;" \
    "    color: #fafafa;" \
    "    background: rgb(40, 40, 40);" \
    "}" \
    "QTabBar::tab:selected {" \
    "    color: #4EC6D5;" \
    "    background: rgb(50, 50, 50);" \
    "}"

// Status Bar
#define STYLE_STATUSBAR \
    "QStatusBar {" \
    "    border-top: 2px solid rgb(50, 50, 50);" \
    "    color: #919191;" \
    "    background: rgb(40, 40, 40);" \
    "}"

// Image Button
#define STYLE_IMAGEBUTTON \
    "QPushButton {" \
    "    padding: 5px;" \
    "    border: none;" \
    "    border-radius: 1px;" \
    "    background: rgba(0, 0, 0, 0.5);" \
    "}" \
    "QPushButton::hover {" \
    "    background: #233251;" \
    "}" \
    "QPushButton::pressed {" \
    "    background: #2E416A;" \
    "}"

// Context Menu
#define STYLE_CONTEXTMENU \
    "QMenu {" \
    "    color: #fafafa;" \
    "}" \
    "QMenu::item:selected {" \
    "    background: #2B426A;" \
    "}" \
    "QMenu::item:disabled {" \
    "    color: #535353;" \
    "}"

// Checkbox
#define STYLE_CHECKBOX \
    "QCheckBox {" \
    "}"

// Group Box
#define STYLE_GROUPBOX \
    "QGroupBox {" \
    "    border: none;" \
    "    color: #fafafa;" \
    "}"

// Scroll Bar
#define STYLE_SCROLLBAR \
    "QScrollBar," \
    "QScrollBar::add-page," \
    "QScrollBar::sub-page," \
    "QScrollBar::add-line," \
    "QScrollBar::sub-line {" \
    "    background: transparent;" \
    "}" \
    "QScrollBar:handle {" \
    "    border: none;" \
    "    background: rgba(50, 50, 50, 0.8);" \
    "}" \
    "QScrollBar:vertical {" \
    "    width: 16px;" \
    "}" \
    "QScrollBar:horizontal {" \
    "    height: 16px;" \
    "}"

#endif // SHADERIDE_GUI_STYLESHEETS_HPP
