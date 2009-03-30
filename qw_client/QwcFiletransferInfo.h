#ifndef QWCFILETRANSFERINFO_H
#define QWCFILETRANSFERINFO_H

//#include <QtCore>
#include <QMetaType>

#include <QFile>

#include "QwcFileInfo.h"
#include <QCryptographicHash>

namespace WiredTransfer {
    enum TransferStatus { StatusWaitingForStat, StatusQueued, StatusActive, StatusDone, StatusQueuedLocal, StatusStopping };
    enum TransferType { TypeDownload, TypeUpload, TypeFolderDownload, TypeFolderUpload };
}


class QwcFiletransferInfo
{

public:
    QwcFiletransferInfo()
    {
        pStatus = WiredTransfer::StatusWaitingForStat;
        pTotalSize = 0;
        pDoneSize = 0;
        pOffset = 0;
        pTransferType = WiredTransfer::TypeDownload;
        pQueuePosition = 0;
        pEncryptTransfer = true;
        pCurrentSpeed = 0;
        pFilesDone = 0;
        pFilesCount = 0;
        pFolderSize = 0;
        pFolderDone = 0;
    };

    ~QwcFiletransferInfo()
    { };

    QwcFiletransferInfo(const QwcFiletransferInfo &orig)
    {
        pStatus = orig.pStatus;
        pLocalPath = orig.pLocalPath;
        pRemotePath = orig.pRemotePath;
        pHash = orig.pHash;
        pChecksum = orig.pChecksum;
        pTransferType = orig.pTransferType;
        pOffset = orig.pOffset;
        pTotalSize = orig.pTotalSize;
        pDoneSize = orig.pDoneSize;
        pQueuePosition = orig.pQueuePosition;

        pRemoteFolder = orig.pRemoteFolder;
        pLocalRoot = orig.pLocalRoot;
        fileList = orig.fileList;
        pFileStatus = orig.pFileStatus;
        pFilesDone = orig.pFilesDone;
        pFilesCount = orig.pFilesCount;
        pFolderSize = orig.pFolderSize;
        pFolderDone = orig.pFolderDone;
        fileListLocal = orig.fileListLocal;

        pEncryptTransfer = orig.pEncryptTransfer;
        pCurrentSpeed = orig.pCurrentSpeed;
    };


    void calcLocalChecksum() {
        QFile tmpFile(pLocalPath);
        if(!tmpFile.open(QIODevice::ReadOnly) ) return;
        // If file exists, calculate the hash.
        QByteArray tmpData = tmpFile.read(1024*1024); // 1MB for the hash
        QByteArray tmpHash = QCryptographicHash::hash(tmpData, QCryptographicHash::Sha1);
        pChecksum = tmpHash.toHex();
        tmpFile.close();
    };


    QString fileName() const {
        return pRemotePath.section("/", -1);
    };


    WiredTransfer::TransferStatus pStatus; // 0=waiting for stat, 1=queued/waiting, 2=running, 3=done/end

    QString pLocalPath; // path to file on local disk
    QString pRemotePath; // path to file on server
    QString pHash; // hash for the file transfer
    QString pChecksum; // checksum for the file
    WiredTransfer::TransferType pTransferType; // 0=download, 1=upload
    qlonglong pOffset; // first byte for transfer
    qlonglong pTotalSize; // total length of file
    qlonglong pDoneSize; // transferred size of data
    int pQueuePosition; // position within the queue
    bool pEncryptTransfer; // if true, transfer will be encrypted
    int pCurrentSpeed; // the current speed, updated during transfer, otherwise 0

    QString pRemoteFolder; // remote folder path for folder transfers
    QString pLocalRoot; // local root dir for folder transfers
    QList<QwcFileInfo> fileList; // for folder downloads
    QStringList fileListLocal; // for folder uploads
    WiredTransfer::TransferStatus pFileStatus; // Status for folder transfers

    int pFilesCount; // finished file counter for folder transfers
    int pFilesDone; // total number of files for folder transfer
    qlonglong pFolderSize; // total size of all files in the folder
    qlonglong pFolderDone; // size of finished files of the folder




    
};

Q_DECLARE_METATYPE(QwcFiletransferInfo)



#endif
