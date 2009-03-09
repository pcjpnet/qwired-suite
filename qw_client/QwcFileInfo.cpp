//#include <QtCore>

#include "QwcFileInfo.h"

#include <QHash>

// Load data from the server file listing responses.
QwcFileInfo::QwcFileInfo(QList<QByteArray> theParams)
{
    path = QString::fromUtf8( theParams.value(0) );
    type = (WiredTransfer::FileType)theParams.value(1).toInt();
    size = theParams.value(2).toInt();
    created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
    modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
    //qDebug() << "[files] created QwcFileInfo for"<<path<<"type="<<type<<"size="<<size;
}


QwcFileInfo::~QwcFileInfo()
{ }


QwcFileInfo::QwcFileInfo()
{
    path = "";
    type = WiredTransfer::RegularFile;
    size = 0;
    isIndexed = false;
}

// Return the name of the file without all the slashes.
QString QwcFileInfo::fileName() const
{
    return path.section("/", -1);
}

void QwcFileInfo::setFromStat(QList<QByteArray> theParams)
{
    path = QString::fromUtf8( theParams.value(0) );
    type = (WiredTransfer::FileType)theParams.value(1).toInt();
    size = theParams.value(2).toInt();
    created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
    modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
    checksum = QString::fromUtf8( theParams.value(5) );
    comment = QString::fromUtf8( theParams.value(6) );
}



/**
 * Return a human readable representation of the size of a file.
 * @param theBytes The raw amount of octets.
 * @return A human readable string representing the number of bytes.
 */
QString QwcFileInfo::humanReadableSize(qlonglong theBytes)
{
    qlonglong a=1024;
    float b=1024;

    if(theBytes<0) {
        return QString("-");
    } else if(theBytes < a) {
        return QString("%1").arg(theBytes);
    } else if(theBytes < a*a) {
        return QString("%1 KB").arg(float(theBytes/b), 0, 'f', 2);
    } else if(theBytes < a*a*a) {
        return QString("%1 MB").arg(float(theBytes/b/b), 0, 'f', 2);
    } else if(theBytes < a*a*a*a) {
        return QString("%1 GB").arg(float(theBytes/b/b/b), 0, 'f', 2);
    } else if(theBytes < a*a*a*a*a) {
        return QString("%1 TB").arg(float(theBytes/b/b/b/b), 0, 'f', 2);
    }
    return "?";
}


/**
 * Return a small icon for this file/type.
 */
QIcon QwcFileInfo::fileIcon() const
{
    QHash<QString,QString> tmpTypes;
    tmpTypes["jpg"] = ":/icons/files/files-image.png";
    tmpTypes["jpeg"] = ":/icons/files/files-image.png";
    tmpTypes["gif"] = ":/icons/files/files-image.png";
    tmpTypes["psd"] = ":/icons/files/files-image.png";
    tmpTypes["svg"] = ":/icons/files/files-image.png";
    tmpTypes["pdf"] = ":/icons/files/files-image.png";
    tmpTypes["tif"] = ":/icons/files/files-image.png";
    tmpTypes["tiff"] = ":/icons/files/files-image.png";

    tmpTypes["zip"] = ":/icons/files/files-archive.png";
    tmpTypes["tar"] = ":/icons/files/files-archive.png";
    tmpTypes["gz"] = ":/icons/files/files-archive.png";
    tmpTypes["sit"] = ":/icons/files/files-archive.png";
    tmpTypes["dmg"] = ":/icons/files/files-archive.png";
    tmpTypes["7z"] = ":/icons/files/files-archive.png";
    tmpTypes["rar"] = ":/icons/files/files-archive.png";

    tmpTypes["doc"] = ":/icons/files/files-office.png";
    tmpTypes["xls"] = ":/icons/files/files-office.png";
    tmpTypes["odt"] = ":/icons/files/files-office.png";
    tmpTypes["ppt"] = ":/icons/files/files-office.png";

    tmpTypes["h"] = ":/icons/files/files-script.png";
    tmpTypes["cpp"] = ":/icons/files/files-script.png";
    tmpTypes["c"] = ":/icons/files/files-script.png";

    tmpTypes["txt"] = ":/icons/files/files-text.png";
    tmpTypes["rdf"] = ":/icons/files/files-text.png";
    tmpTypes["log"] = ":/icons/files/files-text.png";

    tmpTypes["avi"] = ":/icons/files/files-media.png";
    tmpTypes["mpg"] = ":/icons/files/files-media.png";
    tmpTypes["mpeg"] = ":/icons/files/files-media.png";
    tmpTypes["mov"] = ":/icons/files/files-media.png";
    tmpTypes["ogg"] = ":/icons/files/files-media.png";
    tmpTypes["ts"] = ":/icons/files/files-media.png";

    tmpTypes["mp3"] = ":/icons/files/files-audio.png";
    tmpTypes["m4u"] = ":/icons/files/files-audio.png";
    tmpTypes["m4a"] = ":/icons/files/files-audio.png";
    tmpTypes["m4r"] = ":/icons/files/files-audio.png";
    tmpTypes["m4r"] = ":/icons/files/files-audio.png";

    tmpTypes["app"] = ":/icons/files/files-executable.png";
    tmpTypes["exe"] = ":/icons/files/files-executable.png";
    tmpTypes["bin"] = ":/icons/files/files-executable.png";
    tmpTypes["run"] = ":/icons/files/files-executable.png";
    tmpTypes["jar"] = ":/icons/files/files-executable.png";

    QString tmpSuffix = this->fileName().section(".",-1,-1);
    switch(this->type) {
                case WiredTransfer::Directory:
        return QIcon(":/icons/files/files-folder.png"); break;

                case WiredTransfer::DropBox:
        return QIcon(":/icons/files/files-dropbox.png"); break;

                case WiredTransfer::Uploads:
        return QIcon(":/icons/files/files-uploads.png"); break;

                case WiredTransfer::RegularFile:
                default:
        if(tmpTypes.contains(tmpSuffix)) {
            return QIcon(tmpTypes[tmpSuffix]);
        } else {
            return QIcon(":/icons/files/files-regular.png");
        }
        break;
    }

    return QIcon();
}


