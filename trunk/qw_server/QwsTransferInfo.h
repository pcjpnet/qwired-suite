#ifndef QWSTRANSFERINFO_H
#define QWSTRANSFERINFO_H

#include "QwTransferInfo.h"
#include "QwsFile.h"


class QwsTransferInfo : public QwTransferInfo
{
public:
    QwsTransferInfo() : QwTransferInfo()
    {
    }

    /*! The file this transfer is refering to. This overrides the default implementation in
        QwTransferInfo. */
    QwsFile file;

};


#endif // QWSTRANSFERINFO_H
