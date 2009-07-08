#ifndef QWCFILETRANSFERINFO_H
#define QWCFILETRANSFERINFO_H

#include "QwTransferInfo.h"
#include "QwcFileInfo.h"

#include <QDebug>

class QwcTransferInfo : public QwTransferInfo
{
public:
    QwcTransferInfo()
    {
        queuePosition = 0;
    }


    /*! Apply the next queued file information to this object and reset the status.
    */
    void applyNextFile()
    {
        qDebug() << this << "Applying next file from internal queueue.";
        file = recursiveFiles.takeFirst();
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

};

Q_DECLARE_METATYPE(QwcTransferInfo);

#endif

