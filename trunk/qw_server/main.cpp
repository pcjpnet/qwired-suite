#include "QwsServerController.h"

#include <QCoreApplication>
#include <QtDebug>
#include <QSqlQuery>



const QString QWSERVER_VERSION("0.1.5");


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList cliArgs = a.arguments();


    // Print version if requested.
    if (cliArgs.contains("-v") || cliArgs.contains("--version")) {
        QTextStream so(stdout);
        so << "Qwired Server Core " << QWSERVER_VERSION << endl;
        return 0;
    }

    /*! Print help if requested. */
    if (cliArgs.contains("-h") || cliArgs.contains("--help")) {
        QTextStream so(stdout);
        so << a.tr("Usage: %1 [options]\n\n").arg(cliArgs.value(0));
        so << a.tr("Server-mode commands:\n"
                   "    -daemon       Run in daemon mode.\n"
                   "    -remote       Enable console socket for GUI.\n"
                   "    -root <file>  Override default root path (current directory).\n"
                   "\nAdministration commands:\n"
                   "    -sc <key>=<value>\n"
                   "                  Set a configuration parameter. See documentation for\n"
                   "                  more information about available configuration keys.\n"
                   "    -v\n"
                   "    -version      Print version and exit.\n"
                   "\n");
        return 0;
    }


    QwsServerController controller;

    // Override the root directory if needed
    if (int index = cliArgs.indexOf("-root") > -1) {
        QDir newRootPath(cliArgs.value(index+1));
        if (!newRootPath.exists() || !newRootPath.isReadable()) {
            QTextStream so(stderr);
            so << a.tr("Fatal: Root directory does not exist or is unreadable.") << endl;
            return 1;
        }

        QString newWorkingDir = QDir::current().absoluteFilePath(cliArgs.value(index+1));
        bool ok = QDir::setCurrent(newWorkingDir);
        qDebug() << "Setting current working directory to:" << newWorkingDir << "ok:" << ok;
//        controller.serverRootDirectory = "";
    }


    // Don't print to std when in remote mode!
    controller.qwLog(a.tr("Switching to remote console mode..."));
    controller.logToStdout = !cliArgs.contains("-remote");

    if (!controller.loadConfiguration()) {
        return 1;
    }


    // Attempt to start the server socket
    if (!controller.startServer()) {
        controller.qwLog(a.tr("Warning: Unable to start session listener socket."));
        return 1;
    }

    return a.exec();
}



