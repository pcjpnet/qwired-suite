#include "QwsFile.h"

#include <QtDebug>
#include <QDir>
#include <QCryptographicHash>


QwsFile::QwsFile() : QwFile()
{
}


/*! This method returns true if the path is within the current folder root - and thus secure.
    localAbsolutePath is updated after this method has been called.
*/
bool QwsFile::isWithinLocalRoot()
{
    QString localPath = this->localFilesRoot + "/" + this->path;
    QFileInfo localFilesRootInfo(this->localFilesRoot);
    QFileInfo localPathInfo(localPath);
    localAbsolutePath = localPathInfo.absoluteFilePath();

    // Check if the file is part of the files root
    qDebug() << "LocalRoot:" << localFilesRoot << "---" << localAbsolutePath;
    qDebug() << "LocalAbsoluteRoot:" << localFilesRootInfo.absoluteFilePath();
    if (!localAbsolutePath.startsWith(localFilesRootInfo.absoluteFilePath())) {
        qDebug() << this << "Blocking access above files root:" << path;
        return false;
    }
    return true;
}


/*! This method uses \a localFilesRoot and \a path to find a file on the local file system. If the
    file or directory exists, this method returns true and loads available information of the path
    into its properties.
    This method also returns false if the targeted file attempts to access a path above the defined
    \a localFilesRoot path.
    If \a quickCheck is true, no checksum will be calculated for the file.
*/
bool QwsFile::updateLocalPath(bool quickCheck)
{
    QString localPath = this->localFilesRoot + "/" + this->path;
    QFileInfo localFilesRootInfo(this->localFilesRoot);
    QFileInfo localPathInfo(localPath);
    localAbsolutePath = localPathInfo.absoluteFilePath();


    // Check if the file is part of the files root
//    qDebug() << "LocalRoot:" << localFilesRootInfo.absoluteFilePath() << "---" << localAbsolutePath;
    if (!localAbsolutePath.startsWith(localFilesRootInfo.absoluteFilePath())) {
        qDebug() << this << "Blocking access above files root:" << path;
        return false;
    }

    // Check if the local path exists
    if (!localPathInfo.exists() && !localPathInfo.isReadable()) {
        // The file does not exist
        qDebug() << this << "Path is not readable:" << path;
        return false;
    }

    // Exists - now read some info

    this->modified = localPathInfo.lastModified();
    this->created = localPathInfo.created();

    if (localPathInfo.isDir()) {
        // Read directory information
        QDir localPathDir(localAbsolutePath);
        this->type = Qw::FileTypeFolder;
        this->size = localPathDir.count();
        this->checksum = "";
    } else {
        // Read regular file
        this->type = Qw::FileTypeRegular;
        this->size = localPathInfo.size();
        if (!quickCheck) {
            updateLocalChecksum();
        }
    }

    return true;
}






