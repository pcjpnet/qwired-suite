#ifndef CPSOCKET_H
#define CPSOCKET_H

#include "QwSocket.h"
#include "QwMessage.h"

#include <QObject>
#include <QSslSocket>

/*!
    \class QwSocket
    \author Bastian Bense
    \date 2009-01-23
    \brief This is the base class for all sockets used in the Qwired servers and clients. It provides
        basic protocol support and wrappers for the most important functions.
*/


namespace Qw {
    enum ProtocolError { ErrorCommandFailed = 500,
                         ErrorCommandNotRecognized,
                         ErrorCommandNotImplemented,
                         ErrorSyntaxError,
                         ErrorLoginFailed = 510,
                         ErrorBanned,
                         ErrorClientNotFound,
                         ErrorAccountNotFound,
                         ErrorAccountExists,
                         ErrorCannotBeDisconnected,
                         ErrorPermissionDenied,
                         ErrorFileOrDirectoryNotFound = 520,
                         ErrorFileOrDirectoryExists,
                         ErrorChecksumMismatch,
                         ErrorQueueLimitExceeded
                     };
}

class QwSocket : public QObject
{
    Q_OBJECT

public:
    QwSocket(QObject *parent = 0);
    ~QwSocket();
    void setSslSocket(QSslSocket *socket);
    void sendMessage(const QwMessage &message);
    const QSslSocket* sslSocket() const;

protected:
    /*! This property holds a pointer to the responsible (SSL) socket over which all data is to be
        sent or received. */
    QSslSocket *socket;

private:
    QByteArray tcpBuffer;

private slots:
    void socketDataReceived();

signals:
    /*! This signal is emitted when a new message frame has been received. */
    void messageReceived(const QwMessage &message);
    /*! This signal is emitted when the connection to the remote peer is lost. */
    void connectionLost();
;

};

#endif
