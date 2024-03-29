#ifndef QWTRANSFERINFO_H
#define QWTRANSFERINFO_H

#include "QwFile.h"

#include <QString>
#include <QList>

namespace Qw {
    /*! The type of the transfer, from the perspective of the client. */
    enum TransferType { TransferTypeDownload,
                        TransferTypeUpload,
                        /*! Indicates the transfer of a folder. Technically it's the same as a normal
                            download, but it has to be pre-processed properly before. */
                        TransferTypeFolderDownload,
                        TransferTypeFolderUpload
                    };

    enum TransferInfoState {
        /*! The transfer has no status or is in a local queue, waiting to be processed. */
        TransferInfoStateNone,
        /*! The transfer info is queued and listed on the client.
            On the client this indicates that the transfer is server-queued. */
        TransferInfoStateQueued,
        /*! The transfer info is waiting to be accepted by the client (waiting for transfer connection).
            On the client this means that a STAT has been requested. */
        TransferInfoStateRequested,
        /*! The transfer is active and running. (not used in the server!)
            This might indicate that the client is receiving a recursive listing of files.
            \todo Use this state in server mode, too - currently we are using a separate socket state there. */
        TransferInfoStateActive,
        /*! The transfer is paused. (used only in client) */
        TransferInfoStatePaused,
        /*! The transfer information is still being indexed (folder transfers). */
        TransferInfoStateIndexing
    };
}

class QwTransferInfo
{

public:
    QwTransferInfo();

    /*! The state of the transfer info. */
    Qw::TransferInfoState state;
    /*! The unique hash used to identify the transfer. */
    QString hash;
    /*! The number of bytes (from the total length of the file) that have been transferred already. */
    qint64 bytesTransferred;
    /*! The maximum number of bytes that are allowed to be transmitted within a second. */
    qint64 transferSpeedLimit;
    /*! The current transfer speed in bytes/second. */
    int currentTransferSpeed;
    /*! The ID of the user who requested the transfer. */
    int targetUserId;
    /*! The type of the transfer. (Default: TransferTypeDownload) */
    Qw::TransferType type;
    /*! A file information object for keeping information about the file locally or remotely. */
    QwFile file;
};

Q_DECLARE_METATYPE(QwTransferInfo);

#endif // QWSTRANSFERINFO_H
