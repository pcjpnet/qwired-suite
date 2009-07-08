#ifndef QWCFILETRANSFERINFO_H
#define QWCFILETRANSFERINFO_H

#include "QwTransferInfo.h"
#include "QwcFileInfo.h"

//namespace Qwc {
//    enum TransferState {
//        /*! The transfer is paused and might be continued later. */
//        TransferPaused
//    };
//}

class QwcTransferInfo : public QwTransferInfo
{
public:
    QwcTransferInfo()
    {
        queuePosition = 0;
    }

    /*! The position of the transfer in the server queue. */
    int queuePosition;
    /*! A list of files that need to be transferred in the context of a folder download/upload. */
    QList<QwcFileInfo> recursiveFiles;


};

Q_DECLARE_METATYPE(QwcTransferInfo);

#endif

