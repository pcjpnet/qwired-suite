#include "QwFile.h"

#include <QtDebug>
#include <QCryptographicHash>
#include <QFile>

QwFile::QwFile()
{
    size = 0;
}



/*! Returns the base name of the file (last element of path).
*/
QString QwFile::fileName() const
{
    return path.section("/", -1);
}

/*! Calculate the checksum of the local path.
*/
void QwFile::updateLocalChecksum()
{
    QFile targetFile(localAbsolutePath);
    if (!targetFile.open(QIODevice::ReadOnly)) {
        qDebug() << this << "Unable to calculate hash for file:" << localAbsolutePath << " - " << targetFile.error();
        this->checksum = "";
        return;
    }
    qDebug() << "Position:" << targetFile.pos();
    targetFile.seek(0);
    QByteArray hashData = targetFile.read(1024*1024);
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(hashData);
    this->checksum = hash.result().toHex();
    qDebug() << this << "Calculated hash for file" << this->localAbsolutePath << "Bytes =" << hashData.size() << "=" << this->checksum;
}


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
