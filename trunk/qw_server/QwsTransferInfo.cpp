#include "QwsTransferInfo.h"

QwsTransferInfo::QwsTransferInfo()
{
    offset = 0;
    null = true;
    state = Qws::TransferInfoStateQueued;
}
