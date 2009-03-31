#include <QtCore>
#include "QwsServerController.h"
#include "QwTrackerClientSocket.h"

const QString QWSERVER_VERSION("0.1.0");

int main (int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

//    QwTrackerClientSocket *trackerSocket = new QwTrackerClientSocket(0);
//    trackerSocket->mode = Qw::TrackerClientSocketModeAutomatic;
//    trackerSocket->localServerInfo.name = "Test Servar";
//    trackerSocket->localServerInfo.description = "No Comment";
//    trackerSocket->localServerInfo.url = "wired://qwired.neo.de/";
//    trackerSocket->connectToTracker("wired.zankasoftware.com");

    QwsServerController controller;
    if (!controller.loadConfiguration()) {
        return 1;
    }
    if (!controller.startServer()) {
        return 1;
    }




    return app.exec();
}



