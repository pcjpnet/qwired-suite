#ifndef QWSTRANSFERINFO_H
#define QWSTRANSFERINFO_H

#include "QwsFile.h"

namespace Qws {
    /*! The type of the transfer, from the perspective of the client. */
    enum TransferType { TransferTypeDownload, TransferTypeUpload };
    enum TransferInfoState {
        /*! The transfer info is queued and listed on the client. */
        TransferInfoStateQueued,
        /*! The transfer info is waiting to be accepted by the client (waiting for transfer connection. */
        TransferInfoStateWaiting };
}

class QwsTransferInfo
{

public:
    QwsTransferInfo();

    /*! If true, this transfer info is unitialized. */
    bool null;

    /*! The file this transfer is refering to. */
    QwsFile file;

    /*! The type of the transfer (up-/download). */
    Qws::TransferType type;

    /*! The unique hash used to identify the transfer. */
    QString hash;

    /*! The offset within the file. */
    qlonglong offset;

    /*! The number of bytes (from the total length of the file) that have been transferred already. */
    qint64 bytesTransferred;

    /*! The current transfer speed in bytes/second. */
    int currentTransferSpeed;

    /*! The ID of the user who requested the transfer. */
    int targetUserId;

    /*! The state of the transfer info. */
    Qws::TransferInfoState state;


};

#endif // QWSTRANSFERINFO_H
