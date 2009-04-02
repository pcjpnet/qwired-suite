#include "QwmMonitorController.h"


QwmMonitorController::QwmMonitorController(QObject *parent) : QObject(parent)
{
    socket = new QwmConsoleSocket(this);
    connect(socket, SIGNAL(receivedResponseSTAT(QHash<QString,QString>)),
            this, SLOT(handleCommandSTAT(QHash<QString,QString>)));
    connect(socket, SIGNAL(receivedLogMessage(const QString)),
            this, SLOT(handleLogMessage(const QString)));

}


/*! Initialize the monitor window and display it to the user.
*/
void QwmMonitorController::startMonitor()
{
    monitorWindow = new QwmMonitorWindow();
    monitorWindow->show();

    connect(monitorWindow->btnStartServer, SIGNAL(clicked()),
            this, SLOT(startDaemonProcess()));

    socket->connectToConsole("127.0.0.1", 2010);
}


void QwmMonitorController::startDaemonProcess()
{
    QStringList procArguments("-r");
    daemonProcess.start("./qwired_server", procArguments);
    qDebug() << "pid:" << daemonProcess.pid() << daemonProcess.errorString();

    daemonProcess.waitForFinished();
    if (!daemonProcess.isOpen()) {
        qDebug() << "Argh";
    }
    qDebug() << "Done:" << daemonProcess.readAll();

}


/*! Handle the result of a STAT command and display the parameters to the user.
*/
void QwmMonitorController::handleCommandSTAT(QHash<QString,QString> parameters)
{
    monitorWindow->fStatsUsers->setText(parameters["USERS"]);
    monitorWindow->fStatsTransfers->setText(parameters["TRANSFERS"]);
    monitorWindow->fStatsDownloadTotal->setText(humanReadableSize(parameters["TOTAL_BYTES_SENT"].toLongLong()));
    monitorWindow->fStatsUploadTotal->setText(humanReadableSize(parameters["TOTAL_BYTES_RECEIVED"].toLongLong()));
}


/*! Handle a new log message from the remote console and display it in the log window.
*/
void QwmMonitorController::handleLogMessage(const QString logMessage)
{
    qDebug() << "Got log Message:" << logMessage;
    monitorWindow->fStatsLog->append(logMessage);
}


/*! Return a human readable presentation of a information size.
*/
QString QwmMonitorController::humanReadableSize(qlonglong theBytes)
{
        qlonglong a=1024;
        float b=1024;

        if(theBytes<0) {
            return QString("-");
        } else if(theBytes < a) {
            return QString("%1").arg(theBytes);
        } else if(theBytes < a*a) {
            return QString("%1 KB").arg(float(theBytes/b), 0, 'f', 2);
        } else if(theBytes < a*a*a) {
            return QString("%1 MB").arg(float(theBytes/b/b), 0, 'f', 2);
        } else if(theBytes < a*a*a*a) {
            return QString("%1 GB").arg(float(theBytes/b/b/b), 0, 'f', 2);
        } else if(theBytes < a*a*a*a*a) {
            return QString("%1 TB").arg(float(theBytes/b/b/b/b), 0, 'f', 2);
        }
        return "?";
    }
