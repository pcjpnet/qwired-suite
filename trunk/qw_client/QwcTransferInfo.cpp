#include "QwcTransferInfo.h"

#include <QDirIterator>

QwcTransferInfo::QwcTransferInfo() : QwTransferInfo()
{
    queuePosition = 0;
    indexingComplete = false;
}


/*! Apply the next queued file information to this object and reset the status.
    */
void QwcTransferInfo::applyNextFile()
{
    qDebug() << this << "Applying next file from internal queueue and resetting transfer.";
    file = recursiveFiles.takeFirst();

    if (this->type == Qw::TransferTypeFolderDownload) {
        QString newLocalPath = file.path;
        if (newLocalPath.startsWith(folder.path)) {
            newLocalPath.remove(0, folder.path.length());

            // Create the parent directory
            newLocalPath = QDir::cleanPath(folder.localAbsolutePath + newLocalPath);
            QDir newDir;
            newDir.mkpath(newLocalPath.section("/", 0, -2));

            file.localAbsolutePath = newLocalPath;
            qDebug() << this << "New local path:" << file.localAbsolutePath;
        }

    } else if (this->type == Qw::TransferTypeFolderUpload) {
        QString newRemotePath = file.localAbsolutePath;
        if (newRemotePath.startsWith(folder.localAbsolutePath)) {
            newRemotePath.remove(0, folder.localAbsolutePath.length());
            newRemotePath.prepend(folder.path);
            qDebug() << "New remote path:" << newRemotePath;
            file.path = newRemotePath;
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
    folder.size = 0;
    QListIterator<QwcFileInfo> i(recursiveFiles);
    while (i.hasNext()) {
        const QwcFileInfo &item = i.next();
        folder.size += item.size;
    }
    // Also store the total number of files
    folder.folderCount = recursiveFiles.count();
}


/*! Index all local files for a folder upload. The \a folder member has to be set properly.
    \returns true if the indexing was successful.
*/
bool QwcTransferInfo::prepareFolderUpload()
{
    QDir targetFolder(folder.localAbsolutePath);
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
        fileInfo.localAbsolutePath = itemInfo.absoluteFilePath();
        fileInfo.type = itemInfo.isDir() ? Qw::FileTypeFolder : Qw::FileTypeRegular;
        fileInfo.size = itemInfo.size();
        fileInfo.modified = itemInfo.lastModified();
        fileInfo.created = itemInfo.created();
        folder.size += fileInfo.size;
        folder.folderCount += 1;
        recursiveFiles.append(fileInfo);
    }
    applyNextFile();
    qDebug() << this << "Prepared folder upload with" << folder.folderCount << "items";
    return true;
}



