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

