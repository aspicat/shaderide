/**
 * StyleSheets Header
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
    "    border-bottom: 2px solid rgb(28, 28, 28);" \
    "    background: rgb(32, 32, 32);" \
    "}" \
    "QMenuBar::item:selected {" \
    "    color: #4EC6D5;" \
    "    background: rgb(60, 60, 60);" \
    "}"

// OpenGL Widget
#define STYLE_OPENGLWIDGET \
    "QCheckBox {" \
    "    color: #fafafa;" \
    "    background: transparent;" \
    "}"

// Code Editor
#define STYLE_CODEEDITOR \
    "QPlainTextEdit {" \
    "    padding: 10px;" \
    "    border: none;" \
    "    color: #eaeaea;" \
    "    background: rgb(30, 30, 30);" \
    "}"

#define STYLE_CODEEDITOR_FONT_SIZE 10

#ifdef WIN32
#define STYLE_CODEEDITOR_FONT "Consolas"
#else
#define STYLE_CODEEDITOR_FONT "DejaVu Sans Mono"
#endif

// Code Editor / Line Number Area
#define STYLE_LINENUMBERAREA_BG_COLOR QColor(40, 40, 40, 255)
#define STYLE_LINENUMBERAREA_FONT_COLOR QColor(80, 80, 80)
#define STYLE_LINENUMBERAREA_FONT_HIGHLIGHT_COLOR QColor("#4DDCFE")

#if WIN32
#define STYLE_LINENUMBERAREA_FONT "Consolas"
#else
#define STYLE_LINENUMBERAREA_FONT "DejaVu Sans Mono"
#endif

// Tab Widget
#define STYLE_TABWIDGET \
    "QTabWidget::pane {" \
    "    border: none;" \
    "}" \
    "QTabBar::tab {" \
    "    padding: 12px 20px;" \
    "    font-size: 14px;" \
    "    color: #fafafa;" \
    "    background: rgb(40, 40, 40);" \
    "}" \
    "QTabBar::tab:selected {" \
    "    font-weight: bold;" \
    "    color: #4EC6D5;" \
    "    background: rgb(50, 50, 50);" \
    "} background: red;"

// Log Output Widget
#define STYLE_LOGOUTPUTWIDGET \
    "QPlainTextEdit {" \
    "    padding: 10px;" \
    "    border: none;" \
    "    color: rgb(120, 120, 120);" \
    "    background: rgb(24, 24, 24);" \
    "}" \

#define STYLE_LOGOUTPUTWIDGET_FONT_SIZE 10

#ifdef WIN32
#define STYLE_LOGOUTPUTWIDGET_FONT "Consolas"
#else
#define STYLE_LOGOUTPUTWIDGET_FONT "Consolas"
#endif

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

// Environment Settings Panel
#define STYLE_ENVSETTINGSPANEL \
    "#EnvSettingsPanel {" \
    "    border-top: 4px solid #233151;" \
    "    background: rgba(10, 10, 10, 0.95);" \
    "}" \
    "#EnvSettingsPanel QLabel#Title {" \
    "    font-family: \"" STYLE_CODEEDITOR_FONT "\";" \
    "    font-size: 12pt;" \
    "    font-weight: bold;" \
    "    color: #fafafa;" \
    "    background: transparent;" \
    "}"

// Texture Browser
#define STYLE_TEXTUREBROWSER \
    "#TextureBrowser {" \
    "    background: transparent;" \
    "}"

// Texture Browser Image
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

// About Dialog
#define STYLE_ABOUTDIALOG \
    "QDialog {" \
    "    background: rgb(40, 40, 40);" \
    "}" \
    "QLabel {" \
    "    color: #D9D9D9;" \
    "}"

#endif // SHADERIDE_GUI_STYLESHEETS_HPP
