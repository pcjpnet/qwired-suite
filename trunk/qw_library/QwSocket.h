#ifndef CPSOCKET_H
#define CPSOCKET_H

#include "QwSocket.h"
#include "QwMessage.h"

#include <QObject>
#include <QSslSocket>

/*!
    \author Bastian Bense
    \date 2009-01-23

    Dies ist die Basisklasse für alle CentralPark Sockets. Hier ist das Grundprotokoll implementiert
    und wird entsprechend behandelt.
*/


class QwSocket : public QObject
{

    Q_OBJECT

public:
    QwSocket(QObject *parent = 0);
    ~QwSocket();
    void setSslSocket(QSslSocket *socket);
    void sendMessage(const QwMessage &message);

private:
    QSslSocket *socket;
    QByteArray tcpBuffer;

private slots:
    void socketDataReceived();

signals:
    void messageReceived(const QwMessage &message);
    void disconnected();

};

#endif
