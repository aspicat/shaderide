#include <QApplication>
#include <QSurfaceFormat>
#include "GUI/MainWindow.hpp"

using namespace ShaderIDE::GUI;

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // Apply default surface format for Anti-Aliasing.
    QSurfaceFormat surfaceFormat;
    surfaceFormat.setSamples(8);
    QSurfaceFormat::setDefaultFormat(surfaceFormat);

    MainWindow window;
    window.show();
    return app.exec();
}
