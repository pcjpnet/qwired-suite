#include "QwTransferInfo.h"


QwTransferInfo::QwTransferInfo()
{
    bytesTransferred = 0;
    transferSpeedLimit = 0;
    currentTransferSpeed = 0;
    state = Qw::TransferInfoStateNone;
    targetUserId = 0;
    type = Qw::TransferTypeDownload;
}
