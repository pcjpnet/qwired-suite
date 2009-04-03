#ifndef QWSCONSOLESOCKET_H
#define QWSCONSOLESOCKET_H

#include "QwsServerController.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>
#include <QList>

namespace Qws {
    enum ConsoleSocketState { ConsoleSocketStateNormal,
                              ConsoleSocketStateAuthenticated };
};

class QwsConsoleSocketController;

class QwsConsoleSocket : public QObject
{
    Q_OBJECT

public:
    QwsConsoleSocket(QObject *parent = 0);
    void setSocket(QTcpSocket *newSocket);
    QPointer<QwsConsoleSocketController> controller;
    bool receiveLogMessages;


private:
    Qws::ConsoleSocketState state;
    QPointer<QTcpSocket> socket;

public slots:
    void writeLine(const QByteArray data);

private slots:
    void handleClientCommand(const QString commandLine);
    void handleConsoleReadyRead();
    void handleConnected();

signals:
    void error(QAbstractSocket::SocketError);

};



class QwsConsoleSocketController : public QObject
{
    Q_OBJECT

public:
    QwsConsoleSocketController(QObject *parent = 0);
    ~QwsConsoleSocketController();
    QString authSecret;
    void setServerController(QwsServerController *serverController);
    bool startConsole(QHostAddress listenHost, int listenPort);
    QPointer<QwsServerController> serverController;
    const QTcpServer* tcpServer() const
    { return consoleServer; }

private:
    QPointer<QTcpServer> consoleServer;
    QList<QPointer<QwsConsoleSocket> > consoleSockets;

private slots:
    void handleNewConsoleConnection();
    void handleConsoleError(QAbstractSocket::SocketError);
    void handleServerLogMessage(const QString message);

};

#endif // QWSCONSOLESOCKET_H
