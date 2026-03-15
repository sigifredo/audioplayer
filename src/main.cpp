

// own
#include <MainWindow.hpp>

// qt
#include <QApplication>
#include <QFile>

int main(int argc, char **argv)
{
    /*
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
    qputenv("QT_FFMPEG_NO_HW_CODECS", "1");
    qputenv("PULSE_LATENCY_MSEC", "30");
    */

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
    window.show();

    return app.exec();
}