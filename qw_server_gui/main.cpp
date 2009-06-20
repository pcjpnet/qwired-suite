/*! \todo Support comments, folder types and respect them in file operations.

*/


#include <QApplication>
#include <QTranslator>
#include "QwmMonitorController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Translation loader
    QTranslator translator;
    if (translator.load(QString(":/lang/lang_%1.qm")
        .arg(QLocale::QLocale::system().name().section("_",0,0)))) {
        app.installTranslator(&translator);
    }

    // Now create a new controller and start the GUI
    QwmMonitorController controller;
    controller.startMonitor();

    return app.exec();
}
