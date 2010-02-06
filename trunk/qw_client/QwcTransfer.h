#ifndef QWCTRANSFER_H
#define QWCTRANSFER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "QwFile.h"

class QwcFileInfo;

namespace Qwc {
    enum TransferType {
        TransferTypeFileDownload,
        TransferTypeFileUpload,
        TransferTypeFolderDownload,
        TransferTypeFolderUpload
    };

    /*! This is the state of a the transfer task visible to the outside of the transfer itself. */
    enum TransferState {
        /*! The transfer is not configured completely yet. */
        TransferStateInactive,
        /*! The transfer is queued on the client. */
        TransferStateQueuedOnClient,
        /*! The transfer is queued on the remote server. */
        TransferStateQueuedOnServer,
        /*! The transfer is active, thus transferring data to or from the server. */
        TransferStateActive,
        /*! The transfer is manually paused/stopped. */
        TransferStatePaused,
        /*! The transfer is complete. */
        TransferStateComplete,
        /*! The transfer is stopped due to an error. */
        TransferStateError
    };

    /*! This is the internal state of the transfer task. It is used to make sure that we say in
        synchronization with the server. */
    enum TransferInternalState {
        /*! The transfer is running (socket is transferring data). */
        TransferInternalStateActive,
        /*! We are receiving the list of files for a file transfer. */
        TransferInternalStateWaitingForFileInformation,
        /*! We are waiting for a response to a STAT command when downloading files. */
        TransferInternalStateWaitingForFileListing
    };

    /*! A unique ID to identify transfers. */
    typedef int TransferId;
}

class QwcSocket;
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

    QwcSocket* socket();
    void setSocket(QwcSocket *socket);

    QString localPath() const;
    void setLocalPath(const QString &path);

    QString remotePath() const;
    void setRemotePath(const QString &path);

    const QList<QwFile> & transferFiles() const;
    void setTransferFiles(const QList<QwFile> &files);

    int serverQueuePosition() const;
    qint64 completedTransferSize() const;
    qint64 totalTransferSize() const;
    qint64 currentTransferSpeed() const;

private slots:
    void handleFileListItem(const QwcFileInfo &file);
    void handleFileListDone(const QString &path, qint64 freeSpace);
    void handleTransferQueued(const QString &path, int position);
    void handleTransferReady(const QString &path, qint64 offset, const QString &hash);
    void handleFileInformation(const QwFile &file);

    void handleFileTransferFinished();
    void handleFileTransferError();

protected:
    /*! The type of the transfer. */
    Qwc::TransferType m_type;
    /*! The current state of the transfer. */
    Qwc::TransferState m_state;
    /*! The path to the file or directory being transferred on the client system. */
    QString m_localPath;
    /*! The path to the file or directory being transferred on the server system. This is not the
        file currently being transmitted, but the path to the topmost directory. */
    QString m_remotePath;
    /*! The unique ID for this transfer. */
    Qwc::TransferId m_id;
    /*! The list of files associated with this transfer. */
    QList<QwFile> m_transferFiles;
    /*! The transfer socket. */
    QwcTransferSocket *m_transferSocket;
    /*! The server connection socket. */
    QwcSocket *m_socket;

    /*! The ID of the timer, which regulary informs about the current progress for active transfers. */
    int m_updateTimerId;

    /*! The internal state of the transfer task. We use this to ensure that we stay in-sync with
        the server. */
    Qwc::TransferInternalState m_internalState;

    /*! The total amount of data to be transferred (for informational purposes!). */
    qint64 m_totalTransferSize;
    /*! The amount of data transferred so far (for informational purposes!). */
    qint64 m_completedTransferSize;
    /*! The position for the transfer in the server's queue. */
    int m_serverQueuePosition;



    void changeState(Qwc::TransferState newState);
    bool prepareNextFile();
    void timerEvent(QTimerEvent *event);

signals:

    /*! The transfer has changed. */
    void transferChanged();

    /*! The transfer has successfully completed. */
    void finished();


};

Q_DECLARE_METATYPE(QwcTransfer*);

#endif
