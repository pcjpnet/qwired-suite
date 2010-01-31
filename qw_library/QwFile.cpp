#include "QwFile.h"

#include <QtDebug>
#include <QCryptographicHash>
#include <QFile>

QwFile::QwFile()
{
    m_size = 0;
}



/*! Returns the base name of the file (last element of path).
*/
QString QwFile::fileName() const
{
    return m_remotePath.section("/", -1);
}


/*! Returns the path to the containing directory of this file/folder.
*/
QString QwFile::directoryPath() const
{
    return m_remotePath.section("/", 0, -2, QString::SectionIncludeLeadingSep | QString::SectionIncludeTrailingSep);
}


/*! Calculate the checksum of the local path.
*/
void QwFile::updateLocalChecksum()
{
    QFile targetFile(m_localPath);
    if (!targetFile.open(QIODevice::ReadOnly)) {
        qDebug() << this << "Unable to calculate hash for file:" << localPath() << " - " << targetFile.error();
        this->checksum = "";
        return;
    }
    targetFile.seek(0);
    QByteArray hashData = targetFile.read(1024*1024);
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(hashData);
    this->checksum = hash.result().toHex();
    qDebug() << this << "Calculated hash for file" << this->localPath() << "Bytes =" << hashData.size() << "=" << this->checksum;
}


QString QwFile::remotePath() const
{
    QString path(m_remotePath);

    if (type == Qw::FileTypeFolder
        || type == Qw::FileTypeDropBox
        || type == Qw::FileTypeUploadsFolder)
    {
        if (!path.endsWith("/")) {
            path.append("/");
        }
    }

    return path;
}

void QwFile::setRemotePath(const QString &path)
{ m_remotePath = path; }

QString QwFile::localPath() const
{ return m_localPath; }

void QwFile::setLocalPath(const QString &path)
{ m_localPath = path; }

qint64 QwFile::size() const
{ return m_size; }

void QwFile::setSize(qint64 size)
{ m_size = size; }


void QwFile::setTransferredSize(qint64 size)
{ m_transferredSize = size; }

qint64 QwFile::transferredSize() const
{ return m_transferredSize; }



/*! Returns the file sizes (data amouont) as a human readable string in bytes, kilobytes, megabytes
    and so on.
*/
QString QwFile::humanReadableSize(qint64 size)
{
    qint64 a=1024;
    float b=1024;

    if (size<0) {
        return QString("-");
    } else if (size < a) {
        return QString("%1").arg(size);
    } else if (size < a*a) {
        return QString("%1 KB").arg(float(size/b), 0, 'f', 2);
    } else if (size < a*a*a) {
        return QString("%1 MB").arg(float(size/b/b), 0, 'f', 2);
    } else if (size < a*a*a*a) {
        return QString("%1 GB").arg(float(size/b/b/b), 0, 'f', 2);
    } else if (size < a*a*a*a*a) {
        return QString("%1 TB").arg(float(size/b/b/b/b), 0, 'f', 2);
    } else if (size < a*a*a*a*a*a) {
        return QString("%1 PB").arg(float(size/b/b/b/b/b), 0, 'f', 2);
    }
    return "?";
}

