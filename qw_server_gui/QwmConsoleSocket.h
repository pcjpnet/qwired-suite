#ifndef QWMCONSOLESOCKET_H
#define QWMCONSOLESOCKET_H

#include "QwTransferInfo.h"

#include <QTcpSocket>
#include <QTimer>
#include <QStringList>

class QwmConsoleSocket : public QObject
{
    Q_OBJECT

public:
    QwmConsoleSocket(QObject *parent = 0);
    void connectToConsole(QString host="127.0.0.1", quint16 port=2003);
    void resetSocket();
    QString authSecret;

    const QTcpSocket* tcpSocket() const
    { return socket; }

public slots:
    void sendCommandLOG(bool logEnabled);
    void sendCommandSTATS();
    void sendCommandTRANSFERS();

private slots:
    void sendCommand(QString command);
    void handleSocketConnected();
    void handleSocketReadyRead();
    void handleSocketError(QAbstractSocket::SocketError error);

signals:
    void commandCompleted(const QString commandName);
    void commandError(const QString commandName);
    void receivedResponseSTAT(QHash<QString,QString> data);
    void receivedResponseTRANSFERS(QList<QwTransferInfo> transfers);
    void receivedLogMessage(const QString logMessage);

private:
    void checkCommandQueue();

    QStringList commandQueue;
    QStringList inputBuffer;
    /*! A basic timer responsible for requesting STATS regulary. */
    QTimer statTimer;
    /*! Contains the name of the last, active command. This is cleared when a final +OK is recieved. */
    QString activeCommand;
    /*! The plain socket responsible for data transmission. */
    QTcpSocket *socket;

};

#endif // QWMCONSOLESOCKET_H
