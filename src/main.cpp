

// own
#include <MainWindow.hpp>

// qt
#include <QApplication>
#include <QFile>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Cargar estilos QSS
    QFile styleFile(":/resources/styles.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        QString style = styleFile.readAll();
        app.setStyleSheet(style);
        styleFile.close();
    }

    MainWindow window;
    window.showMaximized();

    return app.exec();
}