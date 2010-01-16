#ifndef QwcConnectWidget_H
#define QwcConnectWidget_H

#include "ui_QwcConnectWidget.h"
#include "QwcGlobals.h"

#include "QwTrackerClientSocket.h"

class QwcConnectWidget :
        public QWidget,
        protected Ui::QwcConnectWidget
{
    Q_OBJECT

public:
    QwcConnectWidget(QWidget *parent = 0);

    void connectAutomatically(const QString &address, const QString &login, const QString &password);

public slots:
    void stopReconnecting();
    void startReconnecting();
    void resetForm();
    void setProgressBar(int value, int max);
    void setStatus(const QString &status);

private slots:
    void updateTrackerMenu();

    // Connect Page
    void on_btnConnectTrackers_clicked();

    void on_btnConnectCancel_clicked();

    // Tracker Page
    void on_trackerServerList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_trackerServerList_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_btnTrackerManual_clicked();
    void on_btnTrackerRefresh_clicked();
    void on_btnTrackerConnect_clicked();

    // Tracker Subsystem
    void handleTrackerServers(QList<QwServerInfo> &servers);
    void handleTrackerSocketError(QAbstractSocket::SocketError error);

    void on_btnConnect_clicked();

    void bookmarkSelected(QAction *action);
    void loadBookmarks();


protected:
    void timerEvent(QTimerEvent *);
    QwTrackerClientSocket *trackerSocket;
    int pReconnectTimerId;


signals:
    /// The user clicked 'connect' and is ready to wait for a connection.
    void userFinished(QString theHost, QString theLogin, QString thePassword);
    /// The user clicked the cancel button.
    void onConnectAborted();
    
};

#endif
