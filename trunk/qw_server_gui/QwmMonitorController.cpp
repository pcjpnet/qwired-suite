#include "QwmMonitorController.h"

QwmMonitorController::QwmMonitorController(QObject *parent) : QObject(parent)
{
    socket = new QwmConsoleSocket(this);
    connect(socket, SIGNAL(receivedResponseSTAT(QHash<QString,QString>)),
            this, SLOT(handleCommandSTAT(QHash<QString,QString>)));
    connect(socket, SIGNAL(receivedLogMessage(const QString)),
            this, SLOT(handleLogMessage(const QString)));


    connect(&daemonProcess, SIGNAL(started()),
            this, SLOT(handleDaemonStarted()));
    connect(&daemonProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(handleDaemonFinished(int,QProcess::ExitStatus)));
    connect(&daemonProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(handleDaemonReadyReadStdout()));
}


/*! Initialize the monitor window and display it to the user.
*/
void QwmMonitorController::startMonitor()
{
    monitorWindow = new QwmMonitorWindow();
    monitorWindow->show();

    connect(monitorWindow->btnStartServer, SIGNAL(clicked()),
            this, SLOT(startDaemonProcess()));
    connect(monitorWindow->btnStopServer, SIGNAL(clicked()),
            this, SLOT(stopDaemonProcess()));


}

void QwmMonitorController::connectToConsole()
{
    socket->connectToConsole("127.0.0.1", 2011);
}


void QwmMonitorController::startDaemonProcess()
{
    QStringList procArguments("-r");
    daemonProcess.start("./qwired_server", procArguments);
}

void QwmMonitorController::stopDaemonProcess()
{
    daemonProcess.terminate();
}


void QwmMonitorController::handleDaemonStarted()
{
    monitorWindow->btnStartServer->setEnabled(false);
    monitorWindow->btnStopServer->setEnabled(true);
    QTimer::singleShot(500, this, SLOT(connectToConsole()));
    handleLogMessage(tr("Server daemon started with PID %1.").arg(daemonProcess.pid()));
}


void QwmMonitorController::handleDaemonFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    handleLogMessage(tr("Server daemon terminated with a status of %1.").arg(exitCode));
    monitorWindow->btnStartServer->setEnabled(true);
    monitorWindow->btnStopServer->setEnabled(false);
    socket->resetSocket();
}

void QwmMonitorController::handleDaemonReadyReadStdout()
{
    while (daemonProcess.canReadLine()) {
        QByteArray lineData = daemonProcess.readLine().trimmed();
        handleLogMessage(lineData);
    }
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
