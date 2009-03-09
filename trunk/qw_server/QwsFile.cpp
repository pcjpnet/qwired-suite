#include "QwsFile.h"



/*
#include <QtDebug>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
  */

// Load data from the server file listing responses.
QwsFile::QwsFile(QList<QByteArray> theParams)
{
	path = QString::fromUtf8( theParams.value(0) );
        type = (Qws::FileType)theParams.value(1).toInt();
	size = theParams.value(2).toInt();
	created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
}


QwsFile::~QwsFile()
{
}


/*! This method returns true if the path is within the current folder root - and thus secure.
*/
bool QwsFile::isWithinLocalRoot()
{
    QString localPath = this->localFilesRoot + "/" + this->path;
    QFileInfo localFilesRootInfo(this->localFilesRoot);
    QFileInfo localPathInfo(localPath);
    localAbsolutePath = localPathInfo.absoluteFilePath();

    // Check if the file is part of the files root
    qDebug() << "LocalRoot:" << localFilesRoot << "---" << localAbsolutePath;
    qDebug() << "LocalAbsoluteRoot:" << localFilesRootInfo.absolutePath();
    if (!localAbsolutePath.startsWith(localFilesRootInfo.absolutePath())) {
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
    qDebug() << "LocalRoot:" << localFilesRoot << "---" << localAbsolutePath;
    if (!localAbsolutePath.startsWith(localFilesRootInfo.absolutePath())) {
        qDebug() << this << "Blocking access above files root:" << path;
        return false;
    }

    // Check if the local path exists
    if (!localPathInfo.exists() && !localPathInfo.isReadable()) {
        qDebug() << this << "Path is not readable:" << path;
        return false;
    }

    // Exists - now read some info

    this->modified = localPathInfo.lastModified();
    this->created = localPathInfo.created();

    if (localPathInfo.isDir()) {
        // Read directory information
        QDir localPathDir(localAbsolutePath);
        this->type = Qws::FileTypeFolder;
        this->size = localPathDir.count();
        this->checksum = "";
    } else {
        // Read regular file
        this->type = Qws::FileTypeRegular;
        this->size = localPathInfo.size();
        if (!quickCheck) {
            updateLocalChecksum();
        }
    }

    return true;
}


/*! Calculate the checksum of the local path.
*/
void QwsFile::updateLocalChecksum()
{
    QFile targetFile(localAbsolutePath);
    if (!targetFile.open(QIODevice::ReadOnly)) {
        return;
    }
    QByteArray hashData = targetFile.read(1024*1024);
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(hashData);
    this->checksum = hash.result().toHex();
    qDebug() << this << "Hash for path" << this->path << "=" << this->checksum;
}


QwsFile::QwsFile() {
        path = "";
        type = Qws::FileTypeRegular;
        size = 0;
}

// Return the name of the file without all the slashes.
QString QwsFile::fileName() const
{
	return path.section("/", -1);
}

void QwsFile::setFromStat(QList<QByteArray> theParams)
{
	path = QString::fromUtf8( theParams.value(0) );
        type = (Qws::FileType)theParams.value(1).toInt();
	size = theParams.value(2).toInt();
	created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
	checksum = QString::fromUtf8( theParams.value(5) );
	comment = QString::fromUtf8( theParams.value(6) );
}
