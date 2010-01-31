#include "QwcFileInfo.h"
#include <QHash>

QwcFileInfo::QwcFileInfo() : QwFile()
{
    folderCount = 0;
    previewFileAfterTransfer = false;
}


/*! Set the file info properties to values from the 402-type message.
*/
void QwcFileInfo::setFromMessage402(const QwMessage &message)
{
    m_remotePath = message.stringArg(0);
    type = (Qw::FileType)message.stringArg(1).toInt();
    m_size = message.stringArg(2).toInt();
    created = QDateTime::fromString(message.stringArg(3), Qt::ISODate );
    modified = QDateTime::fromString(message.stringArg(4), Qt::ISODate );
    checksum = message.stringArg(5);
    comment = message.stringArg(6);
}


/*! Set the file info properties to values from the 410-type message.
*/
void QwcFileInfo::setFromMessage410(const QwMessage &message)
{
    m_remotePath = message.stringArg(0);
    type = (Qw::FileType)message.stringArg(1).toInt();
    m_size = message.stringArg(2).toInt();
    created = QDateTime::fromString(message.stringArg(3), Qt::ISODate );
    modified = QDateTime::fromString(message.stringArg(4), Qt::ISODate );
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
        case Qw::FileTypeFolder: return QIcon(":/icons/files/files-folder.png"); break;
        case Qw::FileTypeDropBox: return QIcon(":/icons/files/files-dropbox.png"); break;
        case Qw::FileTypeUploadsFolder: return QIcon(":/icons/files/files-uploads.png"); break;
        case Qw::FileTypeRegular:
        default:
            if (tmpTypes.contains(tmpSuffix)) {
                return QIcon(tmpTypes[tmpSuffix]);
            } else {
                return QIcon(":/icons/files/files-regular.png");
            }
        break;
    }

    return QIcon();
}


