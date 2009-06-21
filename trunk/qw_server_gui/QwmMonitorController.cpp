#include "QwmMonitorController.h"
#include <QApplication>
#include <QMessageBox>
#include <QDir>

QwmMonitorController::QwmMonitorController(QObject *parent) : QObject(parent)
{
    // Daemon console socket
    socket = new QwmConsoleSocket(this);
    connect(socket, SIGNAL(commandCompleted(QString)),
            this, SLOT(handleCommandCompleted(QString)));
    connect(socket, SIGNAL(receivedResponseSTAT(QHash<QString,QString>)),
            this, SLOT(handleCommandSTAT(QHash<QString,QString>)));
    connect(socket, SIGNAL(receivedResponseTRANSFERS(QList<QwTransferInfo>)),
            this, SLOT(handleCommandTRANSFERS(QList<QwTransferInfo>)));
    connect(socket, SIGNAL(receivedResponseUSERS(QList<QwUser>)),
            this, SLOT(handleCommandUSERS(QList<QwUser>)));
    connect(socket, SIGNAL(receivedLogMessage(const QString)),
            this, SLOT(handleLogMessage(const QString)));


    // Daemon Process
    connect(&daemonProcess, SIGNAL(started()),
            this, SLOT(handleDaemonStarted()));
    connect(&daemonProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(handleDaemonFinished(int,QProcess::ExitStatus)));
    connect(&daemonProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(handleDaemonError(QProcess::ProcessError)));
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
    connect(monitorWindow, SIGNAL(requestedTransferAbort(QString)),
            socket, SLOT(sendCommandABORT(QString)));
    connect(monitorWindow, SIGNAL(requestedUserKick(int)),
            socket, SLOT(sendCommandKICK(int)));

    connect(monitorWindow->btnRebuildIndex, SIGNAL(clicked()),
            this, SLOT(handle_btnRebuildIndex_clicked()));
}


//void QwmMonitorController::connectToConsole()
//{
//    socket->connectToConsole("127.0.0.1", 2010);
//}


void QwmMonitorController::startDaemonProcess()
{
    QStringList procArguments;
    procArguments << "-remote"; // Enable remote-mode
    procArguments << "-root" << QDir(qApp->applicationDirPath()).cleanPath("../../../");

    QString command;

#ifdef Q_OS_WIN32
    command = "./qwired_server.exe";
#elif defined(Q_OS_LINUX)
    command = "./qwired_server";
#elif defined(Q_OS_MAC)
    // On Mac OS X the server binary is within the bundle.
    command = QDir(qApp->applicationDirPath()).absoluteFilePath("qwired_server");
#endif

    daemonProcess.start(command, procArguments);
}


void QwmMonitorController::stopDaemonProcess()
{
    if (QMessageBox::question(monitorWindow, tr("Stop Server"),
                              tr("Are you sure you want to shut down the server?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::Yes) {
        daemonProcess.terminate();
    }
}


void QwmMonitorController::handleDaemonStarted()
{
    monitorWindow->btnStartServer->setEnabled(false);
    monitorWindow->btnStopServer->setEnabled(true);

    monitorWindow->btnRebuildIndex->setEnabled(true);
    //QTimer::singleShot(500, this, SLOT(connectToConsole()));
    //handleLogMessage(tr("Server daemon started with PID %1.").arg(daemonProcess.pid()));
}


void QwmMonitorController::handleDaemonFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);
    handleLogMessage(tr("Server daemon terminated with a status of %1.").arg(exitCode));
    monitorWindow->btnStartServer->setEnabled(true);
    monitorWindow->btnStopServer->setEnabled(false);

    monitorWindow->btnRebuildIndex->setEnabled(false);
    socket->resetSocket();
}



void QwmMonitorController::handleDaemonError(QProcess::ProcessError error)
{
    Q_UNUSED(error);


    //handleLogMessage(tr("Server daemon raised an error: %1").arg(daemonProcess.errorString()));
}


void QwmMonitorController::handleDaemonReadyReadStdout()
{
    // I know, I know. This works for now, though. :-)
    static quint16 remotePort = 2002;

    while (daemonProcess.canReadLine()) {
        QString lineData = daemonProcess.readLine().trimmed();
        if (lineData.startsWith("+REMOTE_PORT=")) {
            remotePort = lineData.section("=", 1, 1).toInt();
        } else if (lineData.startsWith("+REMOTE_AUTH_CODE=")) {
            socket->authSecret = lineData.section(": ", 1, 1);
        } else if (lineData.startsWith("+REMOTE_READY")) {
            socket->connectToConsole("127.0.0.1", remotePort);
            handleLogMessage(tr("Connecting to remote console on port %1...").arg(remotePort));
        } else {
            // Otherwise print to the log
            handleLogMessage(lineData);
        }
    }
}

void QwmMonitorController::handleCommandCompleted(QString command)
{
    if (command == "REINDEX") {
        monitorWindow->btnRebuildIndex->setEnabled(true);
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

    monitorWindow->fTransfersStatus->setText(tr("Transfers: %1, Download: %2/s, Upload: %3/s")
                                 .arg(monitorWindow->fTransfersList->count())
                                 .arg(humanReadableSize(parameters["SPEED_DOWNLOADS"].toLongLong()))
                                 .arg(humanReadableSize(parameters["SPEED_UPLOADS"].toLongLong())));
}


void QwmMonitorController::handleCommandTRANSFERS(QList<QwTransferInfo> transfers)
{
    int currentIndex = monitorWindow->fTransfersList->currentRow();
    monitorWindow->fTransfersList->clear();
    QListIterator<QwTransferInfo> i(transfers);
    while (i.hasNext()) {
        QwTransferInfo item = i.next();
        QListWidgetItem *listItem = new QListWidgetItem;
        listItem->setData(Qt::UserRole, QVariant::fromValue(item));
        listItem->setText(tr("%1\nUser: %2, Speed: %3/s")
                          .arg(item.file.fileName())
                          .arg(item.targetUserId)
                          .arg(humanReadableSize(item.currentTransferSpeed)));
        if (item.type == Qw::TransferTypeDownload) {
            listItem->setIcon(QIcon(":/icons/32x32/go-down.png"));
        } else {
            listItem->setIcon(QIcon(":/icons/32x32/go-up.png"));
        }

        monitorWindow->fTransfersList->addItem(listItem);
    }
    monitorWindow->fTransfersList->setCurrentRow(currentIndex);

}


void QwmMonitorController::handleCommandUSERS(QList<QwUser> users)
{
    QString selectedUserId;
    if (monitorWindow->fUsersList->currentItem()) {
        selectedUserId = monitorWindow->fUsersList->currentItem()->text(0);
    }
    monitorWindow->fUsersList->clear();
    QListIterator<QwUser> i(users);
    while (i.hasNext()) {
        QwUser item = i.next();
        QTreeWidgetItem *listItem = new QTreeWidgetItem;
        listItem->setData(0, Qt::UserRole, QVariant::fromValue(item));
        listItem->setText(0, QString::number(item.pUserID));
        listItem->setText(1, item.userNickname);
        listItem->setText(2, item.name);
        if (item.userHostName.isEmpty()) {
            listItem->setText(3, item.userIpAddress);
        } else {
            listItem->setText(3, QString("%1 (%2)").arg(item.userHostName).arg(item.userIpAddress));
        }
        int idleTimeSeconds = item.pIdleTime.secsTo(QDateTime::currentDateTime())/60;
        listItem->setText(4, tr("%1h %2m").arg(idleTimeSeconds/60).arg(idleTimeSeconds % 60));
        listItem->setText(5, item.userStatus);
        monitorWindow->fUsersList->addTopLevelItem(listItem);
    }

    QList<QTreeWidgetItem*> results = monitorWindow->fUsersList->findItems(selectedUserId, Qt::MatchExactly);
    if (!results.isEmpty()) {
        monitorWindow->fUsersList->setCurrentItem(results.takeFirst());
    }

}


/*! Handle a new log message from the remote console and display it in the log window.
*/
void QwmMonitorController::handleLogMessage(const QString logMessage)
{
    monitorWindow->fStatsLog->append(logMessage);
}


void QwmMonitorController::handle_btnRebuildIndex_clicked()
{
    monitorWindow->btnRebuildIndex->setEnabled(false);
    socket->sendCommand("REINDEX");
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
    } else if(theBytes < a*a*a*a*a*a) {
        return QString("%1 PB").arg(float(theBytes/b/b/b/b/b), 0, 'f', 2);
    }
    return "?";
}
