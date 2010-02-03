#include "QwcTransfer.h"
#include "QwcTransferSocket.h"
#include "QwcSocket.h"

QwcTransfer::QwcTransfer(Qwc::TransferType type, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_state = Qwc::TransferStateInactive;
    m_id = qrand();
    m_transferSocket = NULL;
    m_socket = NULL;
    m_totalTransferSize = 0;
    m_completedTransferSize = 0;
    m_updateTimerId = -1;
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
    if (m_type == Qwc::TransferTypeFolderDownload) {
        qDebug() << "Transfer requesting list of files from server";
        changeState(Qwc::TransferStateIndexing);
        m_socket->getFileListRecusive(remotePath());
    }
}

/*! Stops the running transfer.
*/
void QwcTransfer::stop()
{ }


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

    connect(this, SIGNAL(transferChanged()),
            m_socket, SLOT(handleTransferChanged()));
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


qint64 QwcTransfer::completedTransferSize() const
{ return m_completedTransferSize; }

qint64 QwcTransfer::totalTransferSize() const
{ return m_totalTransferSize; }


void QwcTransfer::handleFileListItem(const QwcFileInfo &file)
{
    if (m_state == Qwc::TransferStateIndexing) {
        m_transferFiles << file;
        m_totalTransferSize += file.size();
    }
}

void QwcTransfer::handleFileListDone(const QString &path, qint64 freeSpace)
{
    Q_UNUSED(freeSpace);
    if (m_state == Qwc::TransferStateIndexing) {
        changeState(Qwc::TransferStateActive);
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
    emit transferChanged();

    if (newState == Qwc::TransferStateActive) {
        m_updateTimerId = startTimer(100);
    } else if (m_updateTimerId > -1) {
        killTimer(m_updateTimerId);
        m_updateTimerId = -1;
    }
}


bool QwcTransfer::prepareNextFile()
{
    if (m_transferFiles.isEmpty()) { return false; }

    while (m_transferFiles.size()) {
        QwFile file = m_transferFiles.takeFirst();
        qDebug() << "------------------------------------ PREPARING FOR NEW FILE:" << file.remotePath();

        if (file.type == Qw::FileTypeRegular) {

            // Set up the transfer socket
            if (m_transferSocket) { delete m_transferSocket; }
            m_transferSocket = new QwcTransferSocket(this);
            connect(m_transferSocket, SIGNAL(fileTransferFinished()),
                    this, SLOT(handleFileTransferFinished()));

            m_transferSocket->setServer(m_socket->sslSocket()->peerAddress().toString(),
                                        m_socket->sslSocket()->peerPort());

            QString localPathTest = file.remotePath();
            localPathTest.remove(remotePath());
            file.setLocalPath(QDir(m_localPath).absoluteFilePath(localPathTest));
            m_transferSocket->setTransferDirection(Qwc::TransferDirectionDownload);
            m_transferSocket->setFileInfo(file);
            m_socket->getFile(file.remotePath(), 0);
            return true;
        }
    }

    qDebug() << "------------------------------------ COMPLETED";

    changeState(Qwc::TransferStateComplete);
    emit finished();
    return false;
}


/*! File transfer socket has successfully finished a file transfer. Call \e prepareNextFile() to
    check if there are more files to be transferred.
*/
void QwcTransfer::handleFileTransferFinished()
{
    // Update the amount of data transferred
    QwcTransferSocket *transferSocket = dynamic_cast<QwcTransferSocket*>(sender());
    if (transferSocket) {
        m_completedTransferSize += transferSocket->fileInfo().size();
        emit transferChanged();
    }

    prepareNextFile();
}


void QwcTransfer::timerEvent(QTimerEvent *event)
{
    emit transferChanged();
}
