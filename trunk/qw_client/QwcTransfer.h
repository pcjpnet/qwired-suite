#ifndef QWCTRANSFER_H
#define QWCTRANSFER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "QwFile.h"


namespace Qwc {
    enum TransferType {
        TransferTypeFileDownload,
        TransferTypeFileUpload,
        TransferTypeFolderDownload,
        TransferTypeFolderUpload
    };

    enum TransferState {
        /*! The transfer is not configured completely yet. */
        TransferStateInactive,
        /*! The transfer is queued on the client. */
        TransferStateQueuedOnClient,
        /*! The transfer is queued on the remote server. */
        TransferStateQueuedOnServer,
        /*! The transfer has been requested and is waiting for the server response. */
        TransferStateWaitingForServer,
        /*! The transfer is active, thus transferring data to or from the server. */
        TransferStateActive,
        /*! The transfer is gathering a list of files from the remote server. */
        TransferStateIndexing,
        /*! The transfer is manually paused/stopped. */
        TransferStatePaused
    };

    /*! A unique ID to identify transfers. */
    typedef int TransferId;
}

class QwcTransferSocket;

class QwcTransfer :
        public QObject
{
    Q_OBJECT

public:
    explicit QwcTransfer(Qwc::TransferType type, QObject *parent = 0);

    Qwc::TransferType type() const;
    Qwc::TransferState state() const;
    Qwc::TransferId id() const;

    void start();
    void stop();

    QString localPath() const;
    void setLocalPath(const QString &path);

    QString remotePath() const;
    void setRemotePath(const QString &path);

    const QList<QwFile> & transferFiles() const;
    void setTransferFiles(const QList<QwFile> &files);

protected:
    /*! The type of the transfer. */
    Qwc::TransferType m_type;
    /*! The current state of the transfer. */
    Qwc::TransferState m_state;
    /*! The path to the file or directory being transferred on the client system. */
    QString m_localPath;
    /*! The path to the file or directory being transferred on the server system. */
    QString m_remotePath;
    /*! The unique ID for this transfer. */
    Qwc::TransferId m_id;
    /*! The list of files associated with this transfer. */
    QList<QwFile> m_transferFiles;
    /*! The transfer socket. */
    QwcTransferSocket *m_transferSocket;

signals:
    /*! The state of the transfer has changed. */
    void stateChanged(Qwc::TransferState newState);


};

#endif
