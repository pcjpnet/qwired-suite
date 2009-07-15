#include "QwmMonitorController.h"
#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QtCore/QBuffer>

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
    connect(socket, SIGNAL(receivedResponseVERSION(QString)),
            this, SLOT(handleCommandVERSION(QString)));

    connect(socket, SIGNAL(receivedResponseCONFIG_READ(QString,QByteArray)),
            this, SLOT(handleCommandCONFIG_READ(QString,QByteArray)));


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


QwmMonitorController::~QwmMonitorController()
{
    daemonProcess.kill();
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
    connect(monitorWindow, SIGNAL(updatedTrackerList(QStringList)),
            this, SLOT(handleUpdatedTrackerList(QStringList)));

    connect(monitorWindow, SIGNAL(selectedNewBanner(QImage)),
            this, SLOT(handleSelectedNewBanner(QImage)));

    connect(monitorWindow->btnRebuildIndex, SIGNAL(clicked()),
            this, SLOT(handle_btnRebuildIndex_clicked()));
}


//void QwmMonitorController::connectToConsole()
//{
//    socket->connectToConsole("127.0.0.1", 2010);
//}


void QwmMonitorController::startDaemonProcess()
{
    QString command;
    QStringList procArguments;


#ifdef Q_OS_WIN32
    command = "qwired_server.exe";
#endif

#ifdef Q_OS_LINUX
    command = "./qwired_server";
#endif

#ifdef Q_OS_MAC
    // On Mac OS X the server binary is within the bundle.
    command = QDir(qApp->applicationDirPath()).absoluteFilePath("qwired_server");
    // Root is usually outside of the bundle
    procArguments << "-root" << QDir(qApp->applicationDirPath()).absoluteFilePath("../../../");
#endif

    procArguments << "-remote"; // Enable remote-mode

    qDebug() << command << procArguments;
    daemonProcess.start(command, procArguments);
}


void QwmMonitorController::stopDaemonProcess()
{
    if (QMessageBox::question(monitorWindow, tr("Stop Server"),
                              tr("Are you sure you want to shut down the server?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::Yes) {
        qDebug() << "Terminating daemon.";
        handleLogMessage(tr("Waiting for daemon to terminate..."));
        daemonProcess.kill();
        daemonProcess.waitForFinished();
        qDebug() << "Terminated";
    }
}


void QwmMonitorController::handleDaemonStarted()
{
    monitorWindow->btnStartServer->setEnabled(false);
    monitorWindow->btnStopServer->setEnabled(true);

    monitorWindow->btnRebuildIndex->setEnabled(true);
    monitorWindow->tabConfiguration->setEnabled(true);
    monitorWindow->tabTransfers->setEnabled(true);
    monitorWindow->tabUsers->setEnabled(true);
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
    monitorWindow->tabConfiguration->setEnabled(false);
    monitorWindow->tabTransfers->setEnabled(false);
    monitorWindow->tabUsers->setEnabled(false);
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
    } else if (command == "AUTH") {
        qDebug() << "Requesting server banner...";
        socket->sendCommandCONFIG_READ("server/banner");
        socket->sendCommandCONFIG_READ("server/trackers");
        socket->sendCommandVERSION();
    }
}


/*! Handle the result of a STAT command and display the parameters to the user.
*/
void QwmMonitorController::handleCommandSTAT(QHash<QString,QString> parameters)
{
    monitorWindow->fStatsUsers->setText(parameters["USERS"]);
    monitorWindow->fStatsTransfers->setText(parameters["TRANSFERS"]);
    monitorWindow->fStatsDownloadTotal->setText(QwFile::humanReadableSize(parameters["TOTAL_BYTES_SENT"].toLongLong()));
    monitorWindow->fStatsUploadTotal->setText(QwFile::humanReadableSize(parameters["TOTAL_BYTES_RECEIVED"].toLongLong()));

    monitorWindow->fTransfersStatus->setText(tr("Transfers: %1, Download: %2/s, Upload: %3/s")
                                 .arg(monitorWindow->fTransfersList->count())
                                 .arg(QwFile::humanReadableSize(parameters["SPEED_DOWNLOADS"].toLongLong()))
                                 .arg(QwFile::humanReadableSize(parameters["SPEED_UPLOADS"].toLongLong())));
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
                          .arg(QwFile::humanReadableSize(item.currentTransferSpeed)));
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


void QwmMonitorController::handleCommandVERSION(QString version)
{
    monitorWindow->fStatsServerVersion->setText(version);
}


void QwmMonitorController::handleCommandCONFIG_READ(QString configName, QByteArray configValue)
{
    qDebug() << this << "Received CONFIG param:" << configName << configValue.size();
    if (configName == "server/banner") {
        QPixmap serverBanner;
        if (serverBanner.loadFromData(configValue)) {
            monitorWindow->fConfigurationBanner->setPixmap(serverBanner);
        } else {
            QMessageBox::warning(monitorWindow, tr("Corrupted server banner image"),
                                 tr("The server banner image seems to be corrupted and can not be "
                                    "displayed. Please select a new banner image."));
        }

    } else if (configName == "server/trackers") {
        monitorWindow->configurationTrackersList->clear();
        QStringList trackerLines = QString::fromUtf8(configValue).split(";");
        foreach (QString trackerItem, trackerLines) {
            QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/icn_tracker.png"),
                                                        trackerItem,
                                                        monitorWindow->configurationTrackersList);
        }
    }
}



/*! Handle a new log message from the remote console and display it in the log window.
*/
void QwmMonitorController::handleLogMessage(const QString logMessage)
{

    monitorWindow->fStatsLog->append(QString("%1 --- %2")
                                     .arg(QDateTime::currentDateTime().toString(Qt::TextDate))
                                     .arg(logMessage));
}


/*! A new banner was selected and it has to be stored in the database.
*/
void QwmMonitorController::handleSelectedNewBanner(QImage banner)
{
    QByteArray bannerData;
    QBuffer bannerDataBuffer(&bannerData);
    bannerDataBuffer.open(QIODevice::WriteOnly);
    banner.save(&bannerDataBuffer, "PNG");

    socket->sendCommandCONFIG_WRITE("server/banner", bannerData);
}


void QwmMonitorController::handle_btnRebuildIndex_clicked()
{
    monitorWindow->btnRebuildIndex->setEnabled(false);
    socket->sendCommand("REINDEX");
}


/*! Handle the updated tracker list and send the new configuration the server.
*/
void QwmMonitorController::handleUpdatedTrackerList(QStringList trackerUrls)
{
    qDebug() << this << "Updated tracker configuration.";
    QString configValue = trackerUrls.join(";");
    socket->sendCommandCONFIG_WRITE("server/trackers", configValue.toUtf8());
    socket->sendCommand("RELOAD");
}

