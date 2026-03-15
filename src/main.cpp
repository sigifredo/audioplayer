

// own
#include <MainWindow.hpp>

// qt
#include <QApplication>
#include <QFile>
#include <QIcon>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/resources/icon.png"));

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