#ifndef QWTRACKERSERVERINFO_H
#define QWTRACKERSERVERINFO_H

class QwTrackerServerInfo
{
public:
    QwTrackerServerInfo() {
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

    /*! The registration hash received from the tracker server when registering our own server.
        For normal client usage this one is always empty. */
    QString registrationHash;

};

#endif // QWTRACKERSERVERINFO_H
