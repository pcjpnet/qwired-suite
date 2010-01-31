#include "QwcTransferInfo.h"

#include <QDirIterator>

QwcTransferInfo::QwcTransferInfo() : QwTransferInfo()
{
    queuePosition = 0;
    m_indexingCompleteFlag = false;
}

bool QwcTransferInfo::isIndexingComplete() const
{ return m_indexingCompleteFlag; }

void QwcTransferInfo::setIndexingComplete(bool complete)
{ m_indexingCompleteFlag = complete; }



/*! Apply the next queued file information to this object and reset the status.
    */
void QwcTransferInfo::applyNextFile()
{
    qDebug() << this << "Applying next file from internal queueue and resetting transfer.";
    file = recursiveFiles.takeFirst();

    if (this->type == Qw::TransferTypeFolderDownload) {
        QString newLocalPath = file.remotePath();
        if (newLocalPath.startsWith(folder.remotePath())) {
            newLocalPath.remove(0, folder.remotePath().length());

            // Create the parent directory
            newLocalPath = QDir::cleanPath(folder.localPath() + newLocalPath);
            QDir newDir;
            newDir.mkpath(newLocalPath.section("/", 0, -2));

            file.localPath() = newLocalPath;
            qDebug() << this << "New local path:" << file.localPath();
        }

    } else if (this->type == Qw::TransferTypeFolderUpload) {
        QString newRemotePath = file.localPath();
        if (newRemotePath.startsWith(folder.localPath())) {
            newRemotePath.remove(0, folder.localPath().length());
            newRemotePath.prepend(folder.remotePath());
            qDebug() << "New remote path:" << newRemotePath;
            file.setRemotePath(newRemotePath);
        }

    }

    state = Qw::TransferInfoStateNone;
    hash.clear();
    bytesTransferred = 0;
    currentTransferSpeed = 0;
}


/*! Update information about the folder transfer. This will update the \a folder member of the
    object to have the \a size property set.
*/
void QwcTransferInfo::updateFolderTransferInfo()
{
    // Calculate the total size of all files
    folder.setSize(0);
    QListIterator<QwcFileInfo> i(recursiveFiles);
    while (i.hasNext()) {
        const QwcFileInfo &item = i.next();
        folder.setSize(folder.size() + item.size());
    }
    // Also store the total number of files
    folder.folderCount = recursiveFiles.count();
}


/*! Index all local files for a folder upload. The \a folder member has to be set properly.
    \returns true if the indexing was successful.
*/
bool QwcTransferInfo::prepareFolderUpload()
{
    QDir targetFolder(folder.localPath());
    if (!targetFolder.exists()) { return false; }
    QDirIterator i(targetFolder.absolutePath(),
                   QDir::AllEntries | QDir::NoDotAndDotDot,
                   QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

    // Add the folder itself
    recursiveFiles.append(folder);

    // Add sub-files and directories
    while (i.hasNext()) {
        QString itemPath = i.next();
        QFileInfo itemInfo = i.fileInfo();
        if (!itemInfo.exists()) { continue; }
        QwcFileInfo fileInfo;
        fileInfo.setLocalPath(itemInfo.absoluteFilePath());
        fileInfo.type = itemInfo.isDir() ? Qw::FileTypeFolder : Qw::FileTypeRegular;
        fileInfo.setSize(itemInfo.size());
        fileInfo.modified = itemInfo.lastModified();
        fileInfo.created = itemInfo.created();
        folder.setSize(folder.size() + fileInfo.size());
        folder.folderCount += 1;
        recursiveFiles.append(fileInfo);
    }
    applyNextFile();
    qDebug() << this << "Prepared folder upload with" << folder.folderCount << "items";
    return true;
}



