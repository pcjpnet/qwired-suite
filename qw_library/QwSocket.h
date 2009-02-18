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
    /*! This property holds a pointer to the responsible (SSL) socket over which all data is to be
        sent or received. */
    QSslSocket *socket;
    QByteArray tcpBuffer;

private slots:
    void socketDataReceived();

signals:
    /*! This signal is emitted when a new message frame has been received. */
    void messageReceived(const QwMessage &message);
    /*! This signal is emitted when the connection to the remote peer is lost. */
    void connectionLost();

};

#endif
