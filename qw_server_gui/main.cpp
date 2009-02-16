
#include <QApplication>
#include "ServerMonitorController.h"

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);

    ServerMonitorController controller;
    controller.startMonitor();

    return app.exec();

}
