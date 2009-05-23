/*! \todo Support comments, folder types and respect them in file operations.

*/


#include <QApplication>
#include "QwmMonitorController.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QwmMonitorController controller;
    controller.startMonitor();

    return app.exec();
}
