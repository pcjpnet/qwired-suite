#ifndef QWCFILETRANSFERINFO_H
#define QWCFILETRANSFERINFO_H

#include "QwTransferInfo.h"

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


};

Q_DECLARE_METATYPE(QwcTransferInfo);

#endif

