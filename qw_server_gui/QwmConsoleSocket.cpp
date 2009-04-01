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
}


void QwmConsoleSocket::connectToConsole(QString host, quint16 port)
{
    qDebug() << this << "Connecting to remote console:" << host << port;
    socket->connectToHost(host, port);
}


void QwmConsoleSocket::handleSocketConnected()
{
    qDebug() << this << "Connected to remote console. Sending AUTH.";
    socket->write(QString("AUTH %1\n").arg(authSecret).toUtf8());
    activeCommand = "AUTH";

}


void QwmConsoleSocket::handleSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << this << "Socket error:" << error;
}


void QwmConsoleSocket::handleSocketReadyRead()
{
    while (socket->canReadLine()) {
        QString lineData = socket->readLine().trimmed();
        qDebug() << lineData;

        if (lineData == "+OK") {

            if (activeCommand == "AUTH") {
                activeCommand.clear();
                statTimer.start(5000);
                sendCommandLOG(true);

            } else if (activeCommand == "STATS") {
                QHash<QString,QString> resultHash;
                QStringListIterator i(inputBuffer);
                while (i.hasNext()) {
                    QString item = i.next();
                    int separatorPos = item.indexOf(": ");
                    resultHash[item.left(separatorPos)] = item.mid(separatorPos+1);
                }
                emit receivedResponseSTAT(resultHash);
            }

            emit commandCompleted(activeCommand);
            activeCommand.clear();

        } else if (lineData == "+ERROR") {
            emit commandError(activeCommand);
            activeCommand.clear();

        } else if (lineData.startsWith("+LOG")) {
            qDebug() << "Received log:" << lineData;
            emit receivedLogMessage(lineData.mid(5));

        } else {
            // Append new data to the input buffer for later retrival
            inputBuffer << lineData;
        }


    }
}


void QwmConsoleSocket::sendCommandSTATS()
{
    qDebug() << this << "Requesting STATS command";
    if (!activeCommand.isEmpty()) { return; }
    socket->write("STATS\n");
    activeCommand = "STATS";
}


void QwmConsoleSocket::sendCommandLOG(bool logEnabled)
{
    qDebug() << this << "Requesting LOG command";
    if (!activeCommand.isEmpty()) { return; }
    socket->write(logEnabled ? "LOG 1\n" : "LOG 0\n");
    activeCommand = "LOG";
}
