#ifndef QWCFILETRANSFERINFO_H
#define QWCFILETRANSFERINFO_H

#include "QwTransferInfo.h"
#include "QwcFileInfo.h"

#include <QDebug>
#include <QDir>

class QwcTransferInfo :
        public QwTransferInfo
{
public:
    QwcTransferInfo();

    bool isIndexingComplete() const;
    void setIndexingComplete(bool complete);

    void applyNextFile();
    bool hasNextFile() const { return !recursiveFiles.isEmpty(); };
    void updateFolderTransferInfo();
    bool prepareFolderUpload();

    /*! The position of the transfer in the server queue. */
    int queuePosition;
    /*! A list of files that need to be transferred in the context of a folder download/upload. */
    QList<QwcFileInfo> recursiveFiles;
    /*! A file information that contains information about the folder that is transferred. */
    QwcFileInfo folder;

    /*! This overrides the file member to have the client-specific information, too. */
    QwcFileInfo file;

protected:
    /*! Contains the status of the indexing of a folder transfer. When true, the indexing was
        completed, and the process should be used like a normal transfer. This can be safely
        ignored in normal transfers and is just relevant for handling folder transfers. */
    bool m_indexingCompleteFlag;

};

Q_DECLARE_METATYPE(QwcTransferInfo);

#endif

