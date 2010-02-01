#ifndef QWCFILEINFO_H
#define QWCFILEINFO_H

#include <QIcon>

#include "QwFile.h"
#include "QwMessage.h"

class QwcFileInfo :
        public QwFile
{

public:
    QwcFileInfo();

    void setFromMessage402(const QwMessage &message);
    void setFromMessage410(const QwMessage &message);

    QIcon fileIcon() const;

    /*! Contains the total number of all files to be transferred during a folder transfer. */
    qint64 folderCount;


};

Q_DECLARE_METATYPE(QwcFileInfo);

#endif
