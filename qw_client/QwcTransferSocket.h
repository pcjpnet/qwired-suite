#ifndef QWCFILETRANSFERSOCKET_H
#define QWCFILETRANSFERSOCKET_H

#include "QwFile.h"

#include <QtNetwork/QSslSocket>
#include <QtCore/QTimer>
#include <QtCore/QFile>

namespace Qwc {
    enum TransferDirection {
        TransferDirectionDownload,
        TransferDirectionUpload
    };
}

class QwcTransferSocket :
        public QObject
{
    Q_OBJECT

public:
    QwcTransferSocket(QObject *parent=0);
    ~QwcTransferSocket();

    void setServer(QString theServer, int thePort);
    void beginTransfer();
    void haltTransfer();

    void setFileInfo(const QwFile &file);
    QwFile fileInfo() const;

    void setTransferHash(const QString &hash);
    QString transferHash() const;

    void setTransferDirection(Qwc::TransferDirection direction);
    Qwc::TransferDirection transferDirection() const;

    qint64 currentTransferSpeed() const;

private slots:
    void handleSocketEncrypted();
    void transmitFileChunk(qint64 amountWritten = -1);
    void finishTransfer();

    
protected:
    /*! The transfer hash which is required for the \c TRANSFER command.
        \sa setTransferHash() transferHash() */
    QString m_transferHash;
    /*! The transfer direction (download or upload).
        \sa setTransferDirection() transferDirection() */
    Qwc::TransferDirection m_transferDirection;
    /*! The timer responsible for sending and reading chunks of data from the connection. */
    QTimer m_transferTimer;
    /*! The real host name of the remote server. */
    QString m_serverHost;
    /*! The real port of the remote server. */
    int m_serverPort;
    /*! A timer used to measure the time between data chunks. */
    QTimer m_transferSpeedTimer;
    /*! The low-level socket for data transmission. */
    QSslSocket *m_socket;
    /*! The file writer/reader. */
    QFile m_fileReader;
    /*! Information about the file being transferred. Also contains some information about the
        transfer, such as already transferred bytes. */
    QwFile m_fileInfo;
    /*! The text of the last error which occurred. */
    QString m_errorString;
    /*! The amount of data per second which is currently sent over the socket. */
    qint64 m_currentTransferSpeed;
    /*! The amount of data transferred when the timerEvent() was fired the last time. */
    qint64 m_lastTransferSpeedProgress;

    void timerEvent(QTimerEvent *event);
    void createNewSocket();

signals:
    /*! The connection was established and the data transfer begins. */
    void fileTransferStarted(QwcTransferSocket *transferSocket);
    /*! The transfer was successfully completed and the socket can be deleted. */
    void fileTransferFinished();
    /*! The transfer was not successfully completed and the socket can be deleted.
        \sa errorString() */
    void fileTransferError();
};

#endif
