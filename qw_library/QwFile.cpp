#include "QwFile.h"

#include <QtCore/QDebug>
#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

QwFile::QwFile()
{
    m_size = 0;
    m_transferredSize = 0;
    m_type = Qw::FileTypeRegular;
}


/* Read file information from a local file. You need to use setLocalPath() before trying to call
   this function.
*/
bool QwFile::loadFromLocalFile()
{
    QFileInfo localInfo(m_localPath);

    if (localInfo.exists()) {
        this->setModified(localInfo.lastModified());
        this->setCreated(localInfo.created());

        if (localInfo.isDir()) {
            // Read directory information
            QDir localPathDir(m_localPath);
            localPathDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
            this->m_size = localPathDir.count();
            this->setType(Qw::FileTypeFolder);
            this->setChecksum(QString());
        } else {
            // Read regular file
            this->setType(Qw::FileTypeRegular);
            this->m_size = localInfo.size();
            this->m_checksum = calculateLocalChecksum();
        }
        return true;
    }
    return false;
}

/*! Returns the base name of the file (last element of the remotePath()). */
QString QwFile::fileName() const
{
    return m_remotePath.section("/", -1, -1, QString::SectionSkipEmpty);
}


/*! Returns the path to the containing directory of this file/folder of the remotePath() (basename).
*/
QString QwFile::directoryPath() const
{
    return m_remotePath.section("/", 0, -2,
                                QString::SectionIncludeLeadingSep
                                | QString::SectionIncludeTrailingSep
                                | QString::SectionSkipEmpty);
}


/*! Calculate the checksum of the local path and return it. Please note that you need to use
    setLocalPath() before calling this function. If the file could not be read, a Null-QString is
    returned.
*/
QString QwFile::calculateLocalChecksum() const
{
    QFile targetFile(m_localPath);
    if (!targetFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Unable to calculate hash for file:" << localPath() << " - " << targetFile.error();
        return QString();
    }
    targetFile.seek(0);
    QByteArray hashData = targetFile.read(1024*1024);
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(hashData);
    return hash.result().toHex();
}


QString QwFile::remotePath() const
{
    QString path(m_remotePath);

    if (m_type == Qw::FileTypeFolder || m_type == Qw::FileTypeDropBox || m_type == Qw::FileTypeUploadsFolder)
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

QString QwFile::checksum() const
{ return m_checksum; }

void QwFile::setChecksum(const QString &checksum)
{ m_checksum = checksum; }

void QwFile::setComment(const QString &comment)
{ m_comment = comment; }

QString QwFile::comment() const
{ return m_comment; }

void QwFile::setCreated(QDateTime created)
{ m_created = created; }

QDateTime QwFile::created() const
{ return m_created; }

void QwFile::setModified(QDateTime modified)
{ m_modified = modified; }

QDateTime QwFile::modified() const
{ return m_modified; }

Qw::FileType QwFile::type() const
{ return m_type; }

void QwFile::setType(Qw::FileType type)
{ m_type = type; }

/*! Returns the file sizes (data amouont) as a human readable string in bytes, kilobytes, megabytes
    and so on.
*/
QString QwFile::humanReadableSize(qint64 size)
{
    qint64 a=1000;
    float b=1000;

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

