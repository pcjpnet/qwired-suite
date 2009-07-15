#ifndef QWSTRACKERCLIENTSOCKET_H
#define QWSTRACKERCLIENTSOCKET_H

#include <QDateTime>
#include <QTimer>

#include "QwSocket.h"
#include "QwServerInfo.h"

namespace Qw {
    enum TrackerClientAutoCommands { TrackerClientAutoCommandNone,
                                     TrackerClientAutoCommandSERVERS,
                                     TrackerClientAutoCommandREGISTER
                                   };
};

class QwTrackerClientSocket : public QwSocket
{
    Q_OBJECT

public:
    QwTrackerClientSocket(QObject *parent = 0);

    /*! The command specified in this member is automatically sent to the tracker after connecting. */
    Qw::TrackerClientAutoCommands autoCommand;

    /*! A pointer to the server information object. */
    QwServerInfo *localServerInfo;

    // Information about the tracker
    QString trackerVersion;
    QString trackerProtocolVersion;
    QString trackerName;
    QString trackerDescription;
    QDateTime trackerStartTime;

    // Connectivity
    quint16 trackerPort;
    QString trackerHost;


private:
    /*! A temporary list to store the names of all categories during 710 and 711 messages. */
    QStringList categoryListingBuffer;
    /*! A temporary list to store information about servers during 720 and 721 messages. */
    QList<QwServerInfo> serverListingBuffer;

public slots:
    void connectToTracker(const QString host = "", const int port = 2002);
    void sendCommandCLIENT();
    void sendCommandREGISTER();
    void sendCommandUPDATE();
    void sendCommandCATEGORIES();
    void sendCommandSERVERS();


private slots:
    void handleSocketConnected();
    void handleSocketError(QAbstractSocket::SocketError error);
    void handleMessageReceived(const QwMessage &message);
    void handleMessage200(const QwMessage &message);
    void handleMessage700(const QwMessage &message);
    void handleMessage710(const QwMessage &message);
    void handleMessage711(const QwMessage &message);
    void handleMessage720(const QwMessage &message);
    void handleMessage721(const QwMessage &message);

signals:
    void receivedRegistrationHash(QString hash);
    /*! This signal is emitted in response to \a setCommandCATEGORIES() and contains a list of all
        available categories on the remote tracker. */
    void receivedCategories(QStringList categories);
    /*! This signal is emitted in response to \a sendCommandSERVERS() and contains a list of all
        returned servers on the tracker. */
    void receivedServers(QList<QwServerInfo> &servers);
    /*! This signal is emitted after the connection to the remote tracker has been closed. */
    void disconnectedFromTracker();
    /*! This signal is emitted when a TCP error occurred. */
    void socketError(QAbstractSocket::SocketError socketError);

};

#endif // QWSTRACKERCLIENTSOCKET_H
