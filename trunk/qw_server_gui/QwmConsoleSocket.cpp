#include "QwmConsoleSocket.h"

QwmConsoleSocket::QwmConsoleSocket(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),
            this, SLOT(handleSocketConnected()));
    connect(socket, SIGNAL(readyRead()),
            this, SLOT(handleSocketReadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleSocketError(QAbstractSocket::SocketError)));

    connect(&statTimer, SIGNAL(timeout()),
            this, SLOT(sendCommandSTATS()));
}


void QwmConsoleSocket::resetSocket()
{
    qDebug() << this << "Resetting socket.";
    statTimer.stop();
    socket->disconnectFromHost();
    socket->reset();
    commandQueue.clear();
    inputBuffer.clear();
}


void QwmConsoleSocket::connectToConsole(QString host, quint16 port)
{
    qDebug() << this << "Connecting to remote console:" << host << port;
    socket->connectToHost(host, port);
}


void QwmConsoleSocket::sendCommand(QString command)
{
    commandQueue.append(command);
    checkCommandQueue();
}


void QwmConsoleSocket::handleSocketConnected()
{
    emit receivedLogMessage(tr("Connected to remote console."));
    sendCommand(QString("AUTH %1\n").arg(authSecret));

    emit connectedToConsole();
}



void QwmConsoleSocket::handleSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << this << "Socket error:" << error;
}


void QwmConsoleSocket::handleSocketReadyRead()
{
    while (socket->canReadLine()) {
        QString lineData = QString::fromUtf8(socket->readLine()).trimmed();
        qDebug() << lineData;

        if (lineData == "+OK") {
            if (activeCommand == "AUTH") {
                statTimer.start(2500);
                sendCommandSTATS();
                sendCommandLOG(true);

            } else if (activeCommand == "TRANSFERS") {
                QList<QwTransferInfo> results;
                QStringListIterator i(inputBuffer);
                while (i.hasNext()) {
                    QStringList itemParams = i.next().split(";");
                    QwTransferInfo item;
                    item.state = itemParams.value(0) == "R" ? Qw::TransferInfoStateActive : Qw::TransferInfoStateQueued;
                    item.hash = itemParams.value(1);
                    item.type = itemParams.value(2) == "D" ? Qw::TransferTypeDownload : Qw::TransferTypeUpload;
                    item.targetUserId = itemParams.value(3).toInt();
                    item.file.path = itemParams.value(4);
                    item.bytesTransferred = itemParams.value(5).toLongLong();
                    item.file.size = itemParams.value(6).toLongLong();
                    item.currentTransferSpeed = itemParams.value(7).toLongLong();
                    results.append(item);
                }
                emit receivedResponseTRANSFERS(results);

            } else if (activeCommand == "USERS") {
                QList<QwUser> results;
                QStringListIterator i(inputBuffer);
                while (i.hasNext()) {
                    QStringList itemParams = i.next().split(";");
                    QwUser item;
                    item.pUserID = itemParams.value(0).toInt();
                    item.userNickname = itemParams.value(1);
                    item.name = itemParams.value(2);
                    item.userStatus = itemParams.value(3);
                    item.pIdleTime = QDateTime::currentDateTime().addSecs(-itemParams.value(4).toInt());
                    item.userIpAddress = itemParams.value(5);
                    item.userHostName = itemParams.value(6);
                    results.append(item);
                }
                emit receivedResponseUSERS(results);


            } else if (activeCommand == "STATS") {
                QHash<QString,QString> resultHash;
                QStringListIterator i(inputBuffer);
                while (i.hasNext()) {
                    QString item = i.next();
                    int separatorPos = item.indexOf("=");
                    resultHash[item.left(separatorPos)] = item.mid(separatorPos+1);
                }
                emit receivedResponseSTAT(resultHash);
            }

            emit commandCompleted(activeCommand);
            inputBuffer.clear();
            activeCommand.clear();
            checkCommandQueue();

        } else if (lineData == "+ERROR") {
            emit commandError(activeCommand);
            activeCommand.clear();
            checkCommandQueue();

        } else if (lineData.startsWith("+LOG")) {
            qDebug() << "Received log:" << lineData;
            emit receivedLogMessage(lineData.mid(5));

        } else {
            // Append new data to the input buffer for later retrival
            inputBuffer << lineData;
        }


    }
}


/*! STATS - Request serv statistics
    Request general server statistics about the server.
*/
void QwmConsoleSocket::sendCommandSTATS()
{
    sendCommand("STATS");
    sendCommand("TRANSFERS");
    sendCommand("USERS");
}


/*! TRANSFERS - Request list of transfers
    Request a list of active and queued transfers from the console.
*/
void QwmConsoleSocket::sendCommandTRANSFERS()
{
    sendCommand("TRANSFERS");
}


void QwmConsoleSocket::sendCommandABORT(QString transferId)
{
    sendCommand("ABORT "+transferId);
}

void QwmConsoleSocket::sendCommandKICK(int userId)
{
    sendCommand(QString("KICK %1").arg(userId));
}


/*! Set a configuration parameter in the server configuration database.
*/
void QwmConsoleSocket::sendCommandCONFIG_WRITE(QString configName, QString configValue)
{
    sendCommand(QString("CONFIG WRITE %1:%2").arg(configName).arg(configValue));
}




/*! LOG - Enable/Disable log messages
    Enables/Disables log message from the server.
*/
void QwmConsoleSocket::sendCommandLOG(bool logEnabled)
{
    sendCommand(logEnabled ? "LOG 1" : "LOG 0");
}


void QwmConsoleSocket::checkCommandQueue()
{
    if (!activeCommand.isEmpty()) {
        qDebug() << "Queueing because of active command:" << activeCommand;
        return;
    }
    if (!commandQueue.isEmpty()) {
        QString item = commandQueue.takeFirst();
        activeCommand = item.section(" ", 0, 0);
        socket->write(QString(item+"\n").toUtf8());
        qDebug() << "Sending:" << item;
    }
}
