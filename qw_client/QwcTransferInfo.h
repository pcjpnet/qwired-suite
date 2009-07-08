#ifndef QWCFILETRANSFERINFO_H
#define QWCFILETRANSFERINFO_H

#include "QwTransferInfo.h"
#include "QwcFileInfo.h"

#include <QDebug>
#include <QDir>

class QwcTransferInfo : public QwTransferInfo
{
public:
    QwcTransferInfo()
    {
        queuePosition = 0;
        indexingComplete = false;
    }


    /*! Apply the next queued file information to this object and reset the status.
    */
    void applyNextFile()
    {
        qDebug() << this << "Applying next file from internal queueue.";
        file = recursiveFiles.takeFirst();

        QString newLocalPath = file.path;
        if (newLocalPath.startsWith(folder.path)) {
            newLocalPath.remove(0, folder.path.length());
//            qDebug() << this << "Folder local:" << folder.localAbsolutePath;
//            qDebug() << this << "Path folder:" << folder.path << "Path file:" << file.path;

            // Create the parent directory
            newLocalPath = QDir::cleanPath(folder.localAbsolutePath + newLocalPath);
            QDir newDir;
            newDir.mkpath(newLocalPath.section("/", 0, -2));

            file.localAbsolutePath = newLocalPath;
            qDebug() << this << "New local path:" << file.localAbsolutePath;
        }

        state = Qw::TransferInfoStateNone;
        hash.clear();
        bytesTransferred = 0;
        currentTransferSpeed = 0;
    }

    /*! The position of the transfer in the server queue. */
    int queuePosition;
    /*! A list of files that need to be transferred in the context of a folder download/upload. */
    QList<QwcFileInfo> recursiveFiles;
    /*! A file information that contains information about the folder that is transferred. */
    QwFile folder;
    /*! Contains the status of the indexing of a folder transfer. When true, the indexing was
        completed, and the process should be used like a normal transfer. This can be safely
        ignored in normal transfers and is just relevant for handling folder transfers. */
    bool indexingComplete;

};

Q_DECLARE_METATYPE(QwcTransferInfo);

#endif

