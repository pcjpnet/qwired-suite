#include "QwcTransfer.h"
#include "QwcTransferSocket.h"
#include "QwcSocket.h"

QwcTransfer::QwcTransfer(Qwc::TransferType type, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_state = Qwc::TransferStateInactive;
    m_id = qrand();
    m_transferSocket = new QwcTransferSocket(this);
    m_socket = NULL;
}


/*! Returns the type of the transfer object. */
Qwc::TransferType QwcTransfer::type() const
{ return m_type; }

/*! Returns the current state of the transfer. */
Qwc::TransferState QwcTransfer::state() const
{ return m_state; }

/*! Returns the unique ID for this transfer. */
Qwc::TransferId QwcTransfer::id() const
{ return m_id; }

/*! Starts the transfer.
*/
void QwcTransfer::start()
{
    m_transferSocket->reset();
    if (m_type == Qwc::TransferTypeFolderDownload) {
        qDebug() << "Transfer requesting list of files from server";
        changeState(Qwc::TransferStateIndexing);
        m_socket->getFileListRecusive(remotePath());
    }


}

/*! Stops the running transfer.
*/
void QwcTransfer::stop()
{

}


QwcSocket* QwcTransfer::socket()
{ return m_socket; }

void QwcTransfer::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }
    m_socket = socket;
    if (!m_socket) { return; }

    connect(m_socket, SIGNAL(onFilesListItem(QwcFileInfo)),
            this, SLOT(handleFileListItem(QwcFileInfo)));
    connect(m_socket, SIGNAL(onFilesListDone(QString,qint64)),
            this, SLOT(handleFileListDone(QString,qint64)));

    connect(m_socket, SIGNAL(transferReady(QString,qint64,QString)),
            this, SLOT(handleTransferReady(QString,qint64,QString)));

    m_transferSocket->setServer(m_socket->sslSocket()->peerAddress().toString(),
                                m_socket->sslSocket()->peerPort());

}


QString QwcTransfer::localPath() const
{ return m_localPath; }

void QwcTransfer::setLocalPath(const QString &path)
{ m_localPath = path; }

QString QwcTransfer::remotePath() const
{ return m_remotePath; }

void QwcTransfer::setRemotePath(const QString &path)
{ m_remotePath = path; }


const QList<QwFile> & QwcTransfer::transferFiles() const
{ return m_transferFiles; }

void QwcTransfer::setTransferFiles(const QList<QwFile> &files)
{ m_transferFiles = files; }


void QwcTransfer::handleFileListItem(const QwcFileInfo &file)
{
    if (m_state == Qwc::TransferStateIndexing) {
        qDebug() << this << "Received file listing item" << file.remotePath();
        m_transferFiles << file;
    }
}

void QwcTransfer::handleFileListDone(const QString &path, qint64 freeSpace)
{
    Q_UNUSED(freeSpace);
    if (m_state == Qwc::TransferStateIndexing) {
        qDebug() << this << "Remote indexing complete for path:" << path;
        prepareNextFile();
    }
}

void QwcTransfer::handleTransferReady(const QString &path, qint64 offset, const QString &hash)
{
    if (path == m_transferSocket->fileInfo().remotePath()) {
        qDebug() << "Transfer is ready:" << path << offset << hash;
        m_transferSocket->setTransferHash(hash);
        m_transferSocket->beginTransfer();
    }
}


void QwcTransfer::changeState(Qwc::TransferState newState)
{
    m_state = newState;
    emit stateChanged(newState);
}


bool QwcTransfer::prepareNextFile()
{
    if (m_transferFiles.isEmpty()) { return false; }
    QwFile file = m_transferFiles.takeFirst();
    qDebug() << "Loading next file:" << file.remotePath();

    m_transferSocket->setTransferDirection(Qwc::TransferDirectionDownload);
    m_transferSocket->setFileInfo(file);
    m_socket->getFile(file.remotePath(), 0);


    return true;
}
