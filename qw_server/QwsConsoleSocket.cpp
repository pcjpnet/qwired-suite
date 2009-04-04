#include "QwsConsoleSocket.h"


QwsConsoleSocket::QwsConsoleSocket(QObject *parent) : QObject(parent)
{
    state = Qws::ConsoleSocketStateNormal;
    receiveLogMessages = false;
}


void QwsConsoleSocket::handleConsoleReadyRead()
{
    //QTcpSocket *target = qobject_cast<QTcpSocket*>(sender());
    //if (!target) { return; }
    while (socket->canReadLine()) {
        QString lineData = socket->readLine().trimmed();
        if (lineData.isEmpty()) { continue; }
        handleClientCommand(lineData);
    }
}


void QwsConsoleSocket::writeLine(const QByteArray data)
{
    socket->write(data + "\n");
}


void QwsConsoleSocket::handleClientCommand(const QString commandLine)
{
    qDebug() << "Console:" << commandLine;
    QString commandTrimmed = commandLine.trimmed();

    int spacePos = commandLine.indexOf(" ");
    QString commandName;
    QString commandArg;
    if (spacePos == -1) {
        commandName = commandTrimmed;
    } else {
        commandName = commandTrimmed.left(spacePos);
        commandArg = commandTrimmed.mid(spacePos+1);
        qDebug() << commandName << commandArg;
    }

    if (commandName == "HELP") {
        if (commandArg == "AUTH") {
            writeLine("AUTH <secret>  Enables administrator commands for the current connection.");
            writeLine("+OK");
            return;

        } else if (commandArg == "ABORT") {
            writeLine("ABORT <id>  stops a running file transfer by disconnecting the transfer\n"
                      "socket. A <id> is required, which can be obtained by sending the\n"
                      "TRANSFERS command.");
            writeLine("+OK");
            return;

        } else if (commandArg == "LOG") {
            writeLine("LOG  Enables (1) or disables (0) live-transmission of log messages.");
            writeLine("+OK");
            return;

        } else if (commandArg == "USER") {
            writeLine("USER <id>  Lists detailed information about a connected user.\n"
                      "Possible values are: ID for the current ID of the user, LOGIN for the login\n"
                      "name of the user account, NICKNAME for the current user nickname, STATUS for\n"
                      "the current status of the user, IP for the user IP-address, HOST for the\n"
                      "user's hostname, CLIENT_VERSION for the name and version of the client used,\n"
                      "IDLE_TIME for amount of idle time in seconds.");
            writeLine("+OK");
            return;

        } else if (commandArg == "USERS") {
            writeLine("USERS  Lists all currently connected users, one user per line in the\n"
                      "following format:");
            writeLine("<id>;<nickname>;<login>;<status>;<idle_secs>;<ip>;<hostname>");
            writeLine("+OK");
            return;

        } else if (commandArg == "SHUTDOWN") {
            writeLine("SHUTDOWN  causes the server daemon to shut down immediately. This can be\n"
                      "used to shut down the server remotely in emergency cases.");
            writeLine("+OK");
            return;

        } else if (commandArg == "STATS") {
            writeLine("STATS  returns some basic information about the running server manager.\n"
                      "Values are one-per-line separated by '='. Possible values are:\n"
                      "USERS for the number of currently connected users, TRANSFERS for the number\n"
                      "of currently active transfers, SPEED_UPLOADS and SPEED_DOWNLOADS for the\n"
                      "total speed of all up-/downloads. TOTAL_BYTES_SENT for the total amount of\n"
                      "data sent -to- the clients and TOTAL_BYTES_RECEIVED for the total amount of\n"
                      "data received -from- the clients during file transfers.");
            writeLine("+OK");
            return;

        } else if (commandArg == "TRANSFERS") {
            writeLine("TRANSFERS  lists all currently active transfers, one-per-line in the\n"
                      "following format. <R|Q> defines if the transfer is active (R) or queued\n"
                      "on the server (Q). Queued transfers have no <id>, because there is no way\n"
                      "of modifying them before the actual transfer starts. <type> defines the\n"
                      "type of the transfer: D for download, U for upload.\n"
                      "<R|Q>;<id>;<type>;<user_id>;<path>;<bytes_done>;<bytes_total>;<bytes_per_sec>");
            writeLine("+OK");
            return;

        } else if (commandArg == "REINDEX") {
            writeLine("REINDEX  starts the file indexer thread on the server. The file index allows\n"
                      "the server to quickly find files and calculate total sums. By default the\n"
                      "file indexer is started every 24 hours.");
            writeLine("+OK");
            return;
        }

        writeLine("ABORT <id>     (admin) Abort a specific file transfer.");
        writeLine("AUTH <secret>  Authenticate session to gain administration privileges.");
        writeLine("HELP [cmd]     Display this help screen or help about a specific command.");
        writeLine("LOG <1|0>      (admin) Enable/Disable live log transmission.");
        writeLine("KICK <id>      (admin) Kick a user off the server.");
        writeLine("QUIT           Close the connection.");
        writeLine("REINDEX        (admin) Re-index the file search database.");
//        writeLine("RELOAD         (admin) Reload configuration from the database.");
        writeLine("SHUTDOWN       (admin) Shut down the server daemon.");
        writeLine("STATS          (admin) Print general statistics about the server.");
        writeLine("TRANSFERS      (admin) Print currently active and queued transfers.");
        writeLine("USER <id>      (admin) Display information about a specific user.");
        writeLine("USERS          (admin) Print currently connected users.");
        writeLine("+OK");

    } else if (commandName == "TRANSFERS") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }

        // Active transfers
        QListIterator<QPointer<QwsClientTransferSocket> > i(controller->serverController->transferSockets);
        //int indexCounter = 0;
        while (i.hasNext()) {
            QwsClientTransferSocket *item = i.next();
            if (!item) { continue; }

            QStringList paramItems;
            paramItems << item->info().hash
                    << QString(item->info().type == Qw::TransferTypeDownload ? "D" : "U")
                    << QString::number(item->info().targetUserId)
                    << item->info().file.path
                    << QString::number(item->info().bytesTransferred)
                    << QString::number(item->info().file.size)
                    << QString::number(item->info().currentTransferSpeed);
            paramItems.replaceInStrings(";", ":");
            writeLine(paramItems.join(";").toUtf8());

//            writeLine(QString("R;%1;%2;%3;%4;%5;%6;%7")
//                      .arg(item->info().hash)
//                      .arg(item->info().type == Qw::TransferTypeDownload ? "D" : "U")
//                      .arg(item->info().targetUserId)
//                      .arg(item->info().file.path)
//                      .arg(item->info().bytesTransferred)
//                      .arg(item->info().file.size)
//                      .arg(item->info().currentTransferSpeed)
//                      .toUtf8());
        }

        // Queued transfers
        QList<QwsTransferInfo> queuedTransfers = controller->serverController->transferPool->allTransfers();
        QListIterator<QwsTransferInfo> iq(queuedTransfers);
        while (iq.hasNext()) {
            QwsTransferInfo item = iq.next();
            writeLine(QString("Q;;%1;%2;%3;%4;%5")
                      .arg(item.targetUserId)
                      .arg(item.file.path)
                      .arg(item.bytesTransferred)
                      .arg(item.file.size)
                      .arg(item.currentTransferSpeed)
                      .toUtf8());
        }

        writeLine("+OK");


    } else if (commandName == "ABORT") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }

        QListIterator<QPointer<QwsClientTransferSocket> > i(controller->serverController->transferSockets);
        while (i.hasNext()) {
            QwsClientTransferSocket *item = i.next();
            if (!item) { continue; }
            if (item->info().hash == commandArg) {
                item->abortTransfer();
                writeLine("+OK");
                return;
            }
        }
        writeLine("+ERROR");


    } else if (commandName == "REINDEX") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }
        controller->serverController->reindexFiles();
        writeLine("+OK");


    } else if (commandName == "AUTH") {
        if (controller->authSecret == commandArg) {
            state = Qws::ConsoleSocketStateAuthenticated;
            writeLine("+OK");
        } else {
            writeLine("+ERROR");
        }


    } else if (commandName == "KICK") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }
        if (!controller->serverController->sockets.contains(commandArg.toInt())) {
            writeLine("+ERROR");
        }
        QwsClientSocket *user = controller->serverController->sockets.value(commandArg.toInt());
        if (!user) { return; }
        user->disconnectClient();
        writeLine("+OK");

    } else if (commandName == "LOG") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }
        receiveLogMessages = commandArg.toInt();
        writeLine("+OK");

    } else if (commandName == "STATS") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }
        qint64 speedDownloadSum = 0;
        qint64 speedUploadSum = 0;

        // Calculate total speed utilization
        QListIterator<QPointer<QwsClientTransferSocket> > i(controller->serverController->transferSockets);
        while (i.hasNext()) {
            QwsClientTransferSocket *item = i.next();
            if (!item) { continue; }
            if (item->info().type == Qw::TransferTypeDownload) {
                speedDownloadSum += item->info().currentTransferSpeed;
            } else {
                speedUploadSum += item->info().currentTransferSpeed;
            }
        }


        writeLine(QString("USERS=%1\nTRANSFERS=%2\nSPEED_DOWNLOADS=%3\nSPEED_UPLOADS=%4")
                  .arg(controller->serverController->sockets.count())
                  .arg(controller->serverController->transferSockets.count())
                  .arg(speedDownloadSum).arg(speedUploadSum)
                  .toUtf8());
        writeLine(QString("TOTAL_BYTES_SENT=%1\nTOTAL_BYTES_RECEIVED=%2")
                  .arg(controller->serverController->statsTotalSent)
                  .arg(controller->serverController->statsTotalReceived)
                  .toUtf8());
        writeLine("+OK");


    } else if (commandName == "USER") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }

        if (!controller->serverController->sockets.contains(commandArg.toInt())) {
            writeLine("+ERROR");
        }

        QwsClientSocket *user = controller->serverController->sockets.value(commandArg.toInt());
        if (!user) { return; }

        writeLine(QString("ID=%1\nLOGIN=%2\nNICKNAME=%3\nSTATUS=%4\nIP=%5")
                  .arg(user->user.pUserID).arg(user->user.name).arg(user->user.userNickname)
                  .arg(user->user.userStatus).arg(user->user.userIpAddress).toUtf8());
        writeLine(QString("HOST=%1\nCLIENT_VERSION=%2\nIDLE_TIME=%3")
                  .arg(user->user.userHostName).arg(user->user.pClientVersion)
                  .arg(user->user.pIdleTime.secsTo(QDateTime::currentDateTime()))
                  .toUtf8());

        writeLine("+OK");


    } else if (commandName == "USERS") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }

        QHashIterator<int, QwsClientSocket*> i(controller->serverController->sockets);
        while (i.hasNext()) {
            i.next();
            QwsClientSocket *item = i.value();
            QStringList paramItems = QStringList()
                        << QString::number(item->user.pUserID)
                        << item->user.userNickname
                        << item->user.name
                        << item->user.userStatus
                        << QString::number(item->user.pIdleTime.secsTo(QDateTime::currentDateTime()))
                        << item->user.userIpAddress
                        << item->user.userHostName;
            paramItems.replaceInStrings(";", ":");
            writeLine(paramItems.join(";").toUtf8());
        }
        writeLine("+OK");


    } else if (commandName == "SHUTDOWN") {
        if (state != Qws::ConsoleSocketStateAuthenticated) { writeLine("+ERROR"); return; }
        writeLine("+OK");
        qApp->quit();


    } else if (commandName == "QUIT") {
        writeLine("+OK");
        socket->disconnectFromHost();
        emit error(QAbstractSocket::RemoteHostClosedError);

    } else {
        writeLine("+ERROR");
    }

}


void QwsConsoleSocket::setSocket(QTcpSocket *newSocket)
{
    socket = newSocket;
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(handleConsoleReadyRead()));

    if (newSocket->isOpen()) {
        handleConnected();
    }

}


void QwsConsoleSocket::handleConnected()
{
    writeLine("+READY Qwired Server 0.1.0");
    writeLine("+VERSION 1");
}


QwsConsoleSocketController::QwsConsoleSocketController(QObject *parent) : QObject(parent)
{

}


QwsConsoleSocketController::~QwsConsoleSocketController()
{
    qDebug() << "Destroying QwsConsoleSocketController";
}


/*! Start the local GUI console.
*/
bool QwsConsoleSocketController::startConsole(QHostAddress listenHost, int listenPort)
{
    consoleServer = new QTcpServer(this);
    connect(consoleServer, SIGNAL(newConnection()),
            this, SLOT(handleNewConsoleConnection()));
    return consoleServer->listen(listenHost, listenPort);
}


void QwsConsoleSocketController::setServerController(QwsServerController *serverController)
{
    this->serverController = serverController;
    connect(serverController, SIGNAL(serverLogMessage(const QString)),
            this, SLOT(handleServerLogMessage(const QString)));

}


/*! Handle a new connection from the console and add it to the list of currently active console
    applications.
*/
void QwsConsoleSocketController::handleNewConsoleConnection()
{
    QTcpSocket *incomingSocket = consoleServer->nextPendingConnection();
    qDebug() << "New console connection.";
    QwsConsoleSocket *newSocket = new QwsConsoleSocket(this);
    newSocket->setSocket(incomingSocket);
    newSocket->controller = this;
    consoleSockets.append(newSocket);
    connect(newSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleConsoleError(QAbstractSocket::SocketError)));

}


void QwsConsoleSocketController::handleConsoleError(QAbstractSocket::SocketError)
{
    QwsConsoleSocket *target = qobject_cast<QwsConsoleSocket*>(sender());
    if (!target) { return; }
    consoleSockets.removeOne(target);
    target->deleteLater();
    qDebug() << "Removed old console connection";
}


void QwsConsoleSocketController::handleServerLogMessage(const QString message)
{
    QListIterator<QPointer<QwsConsoleSocket> > i(consoleSockets);
    while (i.hasNext()) {
        QwsConsoleSocket *item = i.next();
        if (!item) { continue; }
        if (!item->receiveLogMessages) { continue; }
        item->writeLine(QString("+LOG %1").arg(message).toUtf8());
    }
}
