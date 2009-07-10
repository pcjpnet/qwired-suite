#ifndef QWSERVERINFO_H
#define QWSERVERINFO_H

#include <QDateTime>
#include <QMetaType>

class QwServerInfo
{
public:
    QwServerInfo() {
        bandwidth = 0;
        canDownload = false;
        canGuests = false;
        filesCount = 0;
        filesSize = 0;
    };

    /*! The name of the category in which this server should be listed in. */
    QString category;
    /*! The URL which should be used to connect to this server. */
    QString url;
    /*! The name of the server. */
    QString name;
    /*! The available bandwidth for downloads on this server in bytes per second. */
    qint64 bandwidth;
    /*! A brief description of the server. */
    QString description;

    /*! The number of currently connected users. */
    int userCount;
    /*! When true, the server allows guests to download files. */
    bool canDownload;
    /*! When true, the server allows guests to log in. */
    bool canGuests;
    /*! The number of files stored on the server. */
    qint64 filesCount;
    /*! The total size of all files on the server. */
    qint64 filesSize;

    /*! The software release version of the server. */
    QString serverVersion;
    /*! The protocol version used by the server. */
    QString protocolVersion;
    /*! The date and time when the server was started. */
    QDateTime startTime;


    /*! The registration hash received from the tracker server when registering our own server.
        For normal client usage this one is always empty. */
    QString registrationHash;

};

Q_DECLARE_METATYPE(QwServerInfo);

#endif // QWTRACKERSERVERINFO_H
