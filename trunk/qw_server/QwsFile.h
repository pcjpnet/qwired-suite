#ifndef QWSFILE_H
#define QWSFILE_H

#include "QwFile.h"

#include <QMetaType>
#include <QFileInfo>
#include <QString>
#include <QList>

class QwsFile : public QwFile
{

public:
    QwsFile();
    
    bool isWithinLocalRoot();
    bool updateLocalPath(bool quickCheck=false);

    /*! A offset value within a file - used during partial transfers. */
    qint64 offset;
    /*! The local root of the files tree. (normally the Files directory of the server)
        Example: "/home/username/qwired_server/files" */
    QString localFilesRoot;
};

Q_DECLARE_METATYPE(QwsFile);

#endif
