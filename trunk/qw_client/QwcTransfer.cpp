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
    m_internalState = Qwc::TransferInternalStateActive;
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
    if (m_state == Qwc::TransferStatePaused) {
        changeState(Qwc::TransferStateActive);
        prepareNextFile();

    } else if (m_state == Qwc::TransferStateActive) {
        // Do nothing
        return;

    } else {
        if (m_type == Qwc::TransferTypeFolderDownload) {
            qDebug() << "Transfer requesting list of files from server";
            changeState(Qwc::TransferStateActive);
            m_internalState = Qwc::TransferInternalStateWaitingForFileListing;
            qDebug() << "Listing:" << remotePath();
            m_socket->getFileListRecusive(remotePath());

        } else if (m_type == Qwc::TransferTypeFileDownload) {
            changeState(Qwc::TransferStateActive);
            m_internalState = Qwc::TransferInternalStateActive;
            QwFile newFile;
            newFile.setRemotePath(m_remotePath);
            newFile.setLocalPath(m_localPath);
            newFile.type = Qw::FileTypeRegular;
            m_transferFiles << newFile;
            prepareNextFile();
        }
    }
}

/*! Stops the running transfer.
*/
void QwcTransfer::stop()
{
    if (m_state == Qwc::TransferStateActive) {
        m_transferSocket->disconnect();
        m_transferFiles.prepend(m_transferSocket->fileInfo());
    }
    changeState(Qwc::TransferStatePaused);
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
    connect(m_socket, SIGNAL(fileInformation(QwFile)),
            this, SLOT(handleFileInformation(QwFile)));

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
{
    if (!m_transferSocket) { return -1; }
    return m_completedTransferSize + m_transferSocket->fileInfo().transferredSize();
}

qint64 QwcTransfer::totalTransferSize() const
{ return m_totalTransferSize; }

/*! Returns the current transfer speed in bytes per second, or -1 if no transfer is currently active. */
qint64 QwcTransfer::currentTransferSpeed() const
{
    if (!m_transferSocket) { return -1; }
    return m_transferSocket->currentTransferSpeed();
}

void QwcTransfer::handleFileListItem(const QwcFileInfo &file)
{
    if (m_internalState == Qwc::TransferInternalStateWaitingForFileListing) {
        m_transferFiles << file;
        m_totalTransferSize += file.size();
    }
}


void QwcTransfer::handleFileListDone(const QString &path, qint64 freeSpace)
{
    Q_UNUSED(freeSpace);
    if (m_internalState == Qwc::TransferInternalStateWaitingForFileListing
        && QDir::cleanPath(m_remotePath) == QDir::cleanPath(path)) {
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


void QwcTransfer::handleFileInformation(const QwFile &file)
{
    if (m_internalState != Qwc::TransferInternalStateWaitingForFileInformation) { return; }
    m_internalState = Qwc::TransferInternalStateActive;

    qDebug() << "got file information for" << file.remotePath();
    QwFile newFile = file;
    if (m_type == Qwc::TransferTypeFolderDownload) {
        // For folder transfers we need to append to the folder path locally
        QString localPathTest = file.remotePath();
        localPathTest.remove(m_remotePath);
        newFile.setLocalPath(QDir(m_localPath).absoluteFilePath(localPathTest));
    } else if (m_type == Qwc::TransferTypeFileDownload) {
        // Files have a direct path set
        newFile.setLocalPath(m_localPath);
    }

    QFileInfo localFileInfo(newFile.localPath());
    if (newFile.size() == localFileInfo.size()
        && newFile.checksum() == newFile.calculateLocalChecksum()) {
        // Skip - the file is complete already
        qDebug() << "Skipping file: already complete and checksums match";
        m_completedTransferSize += localFileInfo.size();
        prepareNextFile();
    } else {
        if (localFileInfo.exists() && localFileInfo.size() > 0
            && newFile.calculateLocalChecksum() != file.checksum()) {
            // The file is not complete, nor does it match the checksum - delete it.
            QFile::remove(file.localPath());
        }
        qDebug() << "Requesting file" << newFile.remotePath() << "off:" << localFileInfo.size();
        newFile.setTransferredSize(localFileInfo.size());
        m_transferSocket->setFileInfo(newFile);
        m_socket->getFile(newFile.remotePath(), localFileInfo.size());
    }
}


void QwcTransfer::changeState(Qwc::TransferState newState)
{

    if (newState == Qwc::TransferStateActive) {
        m_updateTimerId = startTimer(50);
    } else if (m_updateTimerId > -1) {
        killTimer(m_updateTimerId);
        m_updateTimerId = -1;
    }

    m_state = newState;
    emit transferChanged();
}


bool QwcTransfer::prepareNextFile()
{
    while (m_transferFiles.size()) {
        QwFile file = m_transferFiles.takeFirst();
        qDebug() << "------------------------------------ PREPARING FOR NEW FILE:" << file.remotePath();

        if (file.type == Qw::FileTypeRegular) {

            // Set up the transfer socket
            if (m_transferSocket) { delete m_transferSocket; }
            m_transferSocket = new QwcTransferSocket(this);
            connect(m_transferSocket, SIGNAL(fileTransferFinished()),
                    this, SLOT(handleFileTransferFinished()));
            connect(m_transferSocket, SIGNAL(fileTransferError()),
                    this, SLOT(handleFileTransferError()));

            m_transferSocket->setServer(m_socket->sslSocket()->peerAddress().toString(),
                                        m_socket->sslSocket()->peerPort());


            m_transferSocket->setTransferDirection(Qwc::TransferDirectionDownload);

            m_internalState = Qwc::TransferInternalStateWaitingForFileInformation;
            m_socket->getFileInformation(file.remotePath());
            qDebug() << "r'ing" << file.remotePath();
            return true;
        }
    }

    qDebug() << "------------------------------------ COMPLETED";

    // We are done - disconnect all the signals
    disconnect(m_socket, 0, this, 0);

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


void QwcTransfer::handleFileTransferError()
{
    changeState(Qwc::TransferStateError);
}

/*! This is the automatic update timer to have the interface show the recent information properly.
*/
void QwcTransfer::timerEvent(QTimerEvent *)
{
    emit transferChanged();
}
