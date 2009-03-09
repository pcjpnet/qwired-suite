#ifndef QWCTRACKERSERVERINFO_H
#define QWCTRACKERSERVERINFO_H

//#include <QtCore>

class QwcTrackerServerInfo
{

public:
    void loadFromTrackerResponse(QList<QByteArray> theParams)
    {
        category = QString::fromUtf8(theParams.value(0));
        address = QString::fromUtf8( theParams.value(1));
        name = QString::fromUtf8( theParams.value(2) );
        userCount = theParams.value(3).toInt();
        bandwidth = theParams.value(4).toInt();
        guestAllowed = theParams.value(5).toInt();
        downloadAllowed = theParams.value(6).toInt();
        fileCount = theParams.value(7).toInt();
        fileTotalSize = theParams.value(8).toULongLong();
        description = QString::fromUtf8(theParams.value(9));
    };


    QString name;
    QString category;
    QString address;
    int userCount;
    int bandwidth;
    bool guestAllowed;
    bool downloadAllowed;
    int fileCount;
    qulonglong fileTotalSize;
    QString description;
};

#endif
