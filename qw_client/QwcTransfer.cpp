#include "QwcTransfer.h"
#include "QwcTransferSocket.h"
#include "QwcSocket.h"

#include <QtCore/QDirIterator>

const int updateTimerInterval = 50;

QwcTransfer::QwcTransfer(Qwc::TransferType type, QObject *parent) :
    QObject(parent)
{
    m_type = type;
    m_state = Qwc::TransferStateQueuedOnClient;
    m_transferSocket = NULL;
    m_socket = NULL;
    m_totalTransferSize = 0;
    m_completedTransferSize = 0;
    m_internalState = Qwc::TransferInternalStateActive;
    m_serverQueuePosition = 0;

    connect(&m_updateTimer, SIGNAL(timeout()),
            this, SLOT(handleUpdateTimerFired()));

}


/*! Returns the type of the transfer object. */
Qwc::TransferType QwcTransfer::type() const
{ return m_type; }

/*! Returns the current state of the transfer. */
Qwc::TransferState QwcTransfer::state() const
{ return m_state; }



/*! Starts the transfer.
*/
void QwcTransfer::start()
{
    if (m_state == Qwc::TransferStateRunning) { return; }
    changeState(Qwc::TransferStateRunning);
    m_lastProgress = 0;
    m_updateTimer.start(100);

    if (m_state == Qwc::TransferStatePaused) {
        prepareNextFile();

    } else {
        if (m_type == Qwc::TransferTypeFolderDownload) {
            m_internalState = Qwc::TransferInternalStateWaitingForFileListing;
            m_socket->getFileListRecusive(remotePath());

        } else if (m_type == Qwc::TransferTypeFileDownload) {
            m_internalState = Qwc::TransferInternalStateActive;

            QwFile newFile;
            newFile.setRemotePath(m_remotePath);
            newFile.setLocalPath(m_localPath);
            newFile.setType(Qw::FileTypeRegular);
            m_transferFiles << newFile;

            prepareNextFile();

        } else if (m_type == Qwc::TransferTypeFileUpload) {
            m_internalState = Qwc::TransferInternalStateActive;

            QwFile newFile;
            newFile.setRemotePath(m_remotePath);
            newFile.setLocalPath(m_localPath);
            newFile.setType(Qw::FileTypeRegular);
            newFile.loadFromLocalFile();
            m_transferFiles << newFile;
            m_totalTransferSize = newFile.size();

            prepareNextFile();

        } else if (m_type == Qwc::TransferTypeFolderUpload) {
            m_internalState = Qwc::TransferInternalStateActive;

            // First, ensure the root folder exists
            m_socket->createFolder(remotePath());

            QDirIterator i(m_localPath, QStringList(),
                           QDir::NoDotAndDotDot | QDir::AllEntries | QDir::NoSymLinks,
                           QDirIterator::Subdirectories);
            while (i.hasNext()) {
                i.next();
                QFileInfo fileInfo = i.fileInfo();
                QString compactPath = fileInfo.absoluteFilePath().remove(m_localPath);

                QwFile newFile;
                newFile.setRemotePath(QDir::cleanPath(remotePath() + "/" + compactPath));
                newFile.setLocalPath(fileInfo.absoluteFilePath());

                if (fileInfo.isDir()) {
                    newFile.setType(Qw::FileTypeFolder);
                } else {
                    newFile.loadFromLocalFile();
                    newFile.setType(Qw::FileTypeRegular);
                    m_totalTransferSize += newFile.size();
                }

                m_transferFiles << newFile;
            }
            prepareNextFile();
        }
    }
}

/*! Stops the running transfer.
*/
void QwcTransfer::stop()
{
    if (m_state == Qwc::TransferStateRunning) {
        changeState(Qwc::TransferStatePaused);
        m_transferSocket->haltTransfer();
        m_transferFiles.prepend(m_transferSocket->fileInfo());
    }
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
    connect(m_socket, SIGNAL(transferQueued(QString,int)),
            this, SLOT(handleTransferQueued(QString,int)));
    connect(m_socket, SIGNAL(transferReady(QString,qint64,QString)),
            this, SLOT(handleTransferReady(QString,qint64,QString)));
    connect(m_socket, SIGNAL(fileInformation(QwFile)),
            this, SLOT(handleFileInformation(QwFile)));

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

int QwcTransfer::serverQueuePosition() const
{ return m_serverQueuePosition; }

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
    if (m_state != Qwc::TransferStateRunning) { return; }

    if (m_internalState == Qwc::TransferInternalStateWaitingForFileListing) {
        m_transferFiles << file;
        m_totalTransferSize += file.size();
    }
}


void QwcTransfer::handleFileListDone(const QString &path, qint64 freeSpace)
{
    if (m_state != Qwc::TransferStateRunning) { return; }

    Q_UNUSED(freeSpace);
    if (m_internalState == Qwc::TransferInternalStateWaitingForFileListing
        && QDir::cleanPath(m_remotePath) == QDir::cleanPath(path)) {
        prepareNextFile();
    }
}


void QwcTransfer::handleTransferQueued(const QString &path, int position)
{
    if (!m_transferSocket) { return; }
    if (m_state == Qwc::TransferStateRunning || m_state == Qwc::TransferStateQueuedOnServer) {

        if (QDir::cleanPath(path) == QDir::cleanPath(m_transferSocket->fileInfo().remotePath())) {
            m_serverQueuePosition = position;
            changeState(Qwc::TransferStateQueuedOnServer);
        }
    }
}


void QwcTransfer::handleTransferReady(const QString &path, qint64 offset, const QString &hash)
{
    if (!m_transferSocket) { return; }
    if (m_state == Qwc::TransferStateRunning || m_state == Qwc::TransferStateQueuedOnServer) {
        if (QDir::cleanPath(path) == QDir::cleanPath(m_transferSocket->fileInfo().remotePath())) {
            qDebug() << "Comp:" << QDir::cleanPath(path) << QDir::cleanPath(m_transferSocket->fileInfo().remotePath());
            if (m_type == Qwc::TransferTypeFolderUpload || m_type == Qwc::TransferTypeFileUpload) {
                // Update the offset as the file might be partially trasnferred already.
                QwFile transferFile = m_transferSocket->fileInfo();
                transferFile.setTransferredSize(offset);
                m_transferSocket->setFileInfo(transferFile);
            }

            // Start the transfer
            m_serverQueuePosition = 0;
            changeState(Qwc::TransferStateRunning);
            m_transferSocket->setTransferHash(hash);
            m_transferSocket->beginTransfer();
        }
    }
}


void QwcTransfer::handleFileInformation(const QwFile &file)
{
    if (m_internalState != Qwc::TransferInternalStateWaitingForFileInformation) { return; }
    m_internalState = Qwc::TransferInternalStateActive;

    QwFile newFile = file;

    if (m_type == Qwc::TransferTypeFolderDownload) {
        // For folder transfers we need to append to the folder path locally
        QString localPathTest = file.remotePath();
        localPathTest.remove(m_remotePath);
        newFile.setLocalPath(QDir(m_localPath).absoluteFilePath(localPathTest));

    } else if (m_type == Qwc::TransferTypeFileDownload) {
        // Files have a direct path set
        newFile.setLocalPath(m_localPath);
        m_totalTransferSize = newFile.size();
    }

    QFileInfo localFileInfo(newFile.localPath());
    if (newFile.size() == localFileInfo.size()
        && newFile.checksum() == newFile.calculateLocalChecksum()) {
        // Skip - the file is complete already
        m_completedTransferSize += localFileInfo.size();
        prepareNextFile();
    } else {
        if (localFileInfo.exists() && localFileInfo.size() > 0
            && newFile.calculateLocalChecksum() != file.checksum()) {
            // The file is not complete, nor does it match the checksum - delete it.
            QFile::remove(file.localPath());
        }
        newFile.setTransferredSize(localFileInfo.size());
        m_transferSocket->setFileInfo(newFile);
        m_socket->getFile(newFile.remotePath(), localFileInfo.size());
    }
}


void QwcTransfer::changeState(Qwc::TransferState newState)
{
    if (newState == Qwc::TransferStateRunning) {
        // Start the update timer. The timer will automatically stop as soon as we change to another
        // state.
        QTimer::singleShot(updateTimerInterval, this, SLOT(handleUpdateTimerFired()));
    }

    m_state = newState;
    emit transferChanged();
}


bool QwcTransfer::prepareNextFile()
{
    while (m_transferFiles.size()) {
        QwFile file = m_transferFiles.takeFirst();

        if (file.type() == Qw::FileTypeRegular) {
            // Set up the transfer socket
            if (m_transferSocket) { delete m_transferSocket; }
            m_transferSocket = new QwcTransferSocket(this);
            connect(m_transferSocket, SIGNAL(fileTransferFinished()),
                    this, SLOT(handleFileTransferFinished()));
            connect(m_transferSocket, SIGNAL(fileTransferError()),
                    this, SLOT(handleFileTransferError()));

            m_transferSocket->setServer(m_socket->sslSocket()->peerAddress().toString(),
                                        m_socket->sslSocket()->peerPort());

            if (m_type == Qwc::TransferTypeFileDownload || m_type == Qwc::TransferTypeFolderDownload) {
                // For downloads we first must STAT the target file to get some more details
                // about it.
                m_transferSocket->setTransferDirection(Qwc::TransferDirectionDownload);
                m_internalState = Qwc::TransferInternalStateWaitingForFileInformation;
                m_socket->getFileInformation(file.remotePath());

            } else if (m_type == Qwc::TransferTypeFileUpload || m_type == Qwc::TransferTypeFolderUpload) {
                // File uploads can happen directly. We assume that the target file does not exist
                // or is at least incomplete.
                m_transferSocket->setTransferDirection(Qwc::TransferDirectionUpload);
                m_transferSocket->setFileInfo(file);
                m_socket->putFile(file.remotePath(), file.size(), file.checksum());
            }

            return true;

        } else if (file.type() == Qw::FileTypeFolder) {
            // Create folders if needed
            if (m_type == Qwc::TransferTypeFileUpload || m_type == Qwc::TransferTypeFolderUpload) {
                m_socket->createFolder(file.remotePath());
                m_socket->setFolderType(file.remotePath(), file.type());
                continue;
            }
        }
    }

    // Update the state to complete
    changeState(Qwc::TransferStateComplete);

    emit transferEnded();

    // We are done - disconnect all the signals
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
    if (m_state == Qwc::TransferStateRunning) {
        changeState(Qwc::TransferStateError);
    }
}


void QwcTransfer::handleUpdateTimerFired()
{
    qint64 newProgress = completedTransferSize();
    if (newProgress != m_lastProgress) {
        emit transferChanged();
    }
    m_lastProgress = newProgress;

    if (m_state != Qwc::TransferStateRunning) {
        m_updateTimer.stop();
    }
}
