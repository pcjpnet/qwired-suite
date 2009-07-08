#ifndef QWCFILEINFO_H
#define QWCFILEINFO_H

#include <QIcon>

#include "QwFile.h"
#include "QwMessage.h"

class QwcFileInfo : public QwFile
{

public:
    QwcFileInfo();

    void setFromMessage402(const QwMessage &message);
    void setFromMessage410(const QwMessage &message);

    QIcon fileIcon() const;
};

Q_DECLARE_METATYPE(QwcFileInfo);

#endif
