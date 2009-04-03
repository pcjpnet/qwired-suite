
#include "QwsServerController.h"
#include "QwsConsoleSocket.h"

#include <iostream>
#include <QtCore>
#include <QSqlQuery>
#include "QwTrackerClientSocket.h"

const QString QWSERVER_VERSION("0.1.0");

int main (int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList cliArgs = app.arguments();

    // Check command line parameters
    if (cliArgs.contains("-v")) {
        QTextStream so(stdout);
        so << "Qwired Server Version " << QWSERVER_VERSION << "\n"
                "Copyright (c) " << QDate::currentDate().year()
                << " by Bastian Bense <bastibense@gmail.com>\n";
        return 0;

    } else if (cliArgs.contains("-h") || cliArgs.contains("--help")) {
        if (cliArgs.count() == 1) {
            QTextStream so(stdout);
            so << app.tr("Usage: %1 [options]\n\n").arg(cliArgs.value(0));
            so << app.tr("Server-mode commands:\n"
                         "    -d            Run in daemon mode.\n"
                         "    -r            Enable console socket for GUI.\n"
                         "    -db <file>    Override default path to configuration database.\n"
                         "\nAdministration commands:\n"
                         "    -c <key>=<value>  Set a configuration parameter. See documentation for\n"
                         "                  more information about available configuration keys.\n"
                         "    -v            Print version and exit.\n"
                         "\n");
            return 0;
        }
    }


    QwsServerController controller;

    // Don't print to std when in remote mode!
    controller.logToStdout = !cliArgs.contains("-r");

    if (!controller.loadConfiguration()) {
        return 1;
    }

    // Check if we need to override configuration parameters
    QStringListIterator i(cliArgs);
    while (i.hasNext()) {
        QString item = i.next();
        if (!i.hasNext()) { break; }
        QString itemValue = i.peekNext();
        if (item == "-sc") {
            QString confKey = itemValue.section("=", 0, 0);
            QString confValue = itemValue.section("=", 1, 1);
            if (confKey.isEmpty() || confKey.indexOf("%")>-1 ) { continue; }
            controller.qwLog(app.tr("Setting configuration parameter '%1' to '%2'").arg(confKey).arg(confValue));
            QSqlQuery query;

            // Delete old configuration entry
            query.prepare("DELETE FROM qws_config WHERE conf_key=:_key");
            query.bindValue(":_key", confKey);
            query.exec();

            // Insert new entry
            query.clear();
            query.prepare("INSERT INTO qws_config (conf_key, conf_value) VALUES (:_key, :_value);");
            query.bindValue(":_key", confKey);
            query.bindValue(":_value", confValue);
            query.exec();
        }
    }

    // Attempt to start the server socket
    if (!controller.startServer()) {
        return 1;
    }

    // Attempt to start the console socket
    QwsConsoleSocketController consoleController;
    if (cliArgs.contains("-r")) {
        consoleController.authSecret = controller.getConfigurationParam("console/secret", "").toString();
        consoleController.setServerController(&controller);

        QHostAddress listenInterface(controller.getConfigurationParam("console/address", "127.0.0.1").toString());
        quint16 listenPort = controller.getConfigurationParam("server/port", 2000).toInt()+2;

        controller.qwLog(app.tr("Starting remote console listener on %1:%2...")
                         .arg(listenInterface.toString()).arg(listenPort));

        if (!consoleController.startConsole(listenInterface, listenPort)) {
            controller.qwLog(app.tr("Fatal: Unable to start console listener on %1:%2: %3")
                             .arg(listenInterface.toString()).arg(listenPort).arg(consoleController.tcpServer()->errorString()));
            return 1;
        }

        QTextStream(stdout) << QString("+REMOTE_PORT=%1\n+REMOTE_AUTH_CODE=%2\n+REMOTE_READY\n")
                .arg(listenPort).arg(consoleController.authSecret);

    }

    //
    //    QwTrackerClientSocket *trackerSocket = new QwTrackerClientSocket(0);
    //    trackerSocket->mode = Qw::TrackerClientSocketModeAutomatic;
    //    trackerSocket->localServerInfo.name = "Test Servar";
    //    trackerSocket->localServerInfo.description = "No Comment";
    //    trackerSocket->localServerInfo.url = "wired://qwired.neo.de/";
    //    trackerSocket->connectToTracker("wired.zankasoftware.com");



    return app.exec();
}



