#ifndef QwcConnectWidget_H
#define QwcConnectWidget_H

#include "ui_QwcConnectWidget.h"
#include "QwcGlobals.h"

#include "QwTrackerClientSocket.h"

class QwcConnectWidget : public QWidget, public Ui::QwcConnectWidget
{
    Q_OBJECT

public:
    QwcConnectWidget(QWidget *parent = 0);

private:


    QwTrackerClientSocket *trackerSocket;
    int pReconnectTimerId;




private slots:
    void updateTrackerMenu();

    // Connect Page
    void on_btnConnectTrackers_clicked();

    // Tracker Page
    void on_btnTrackerManual_clicked();
    void on_btnTrackerRefresh_clicked();

    // Tracker Subsystem
    void handleTrackerServers(QList<QwServerInfo> &servers);

    void on_btnConnect_clicked();
    void on_fBtnCancel_clicked();
    void bookmarkSelected(QAction *action);
    void loadBookmarks();


protected:
    void timerEvent(QTimerEvent *);


public slots:
    void stopReconnecting();
    void startReconnecting();
    void resetForm();
    void setProgressBar(int value, int max);
    void setStatus(const QString &status);


signals:
    /// The user clicked 'connect' and is ready to wait for a connection.
    void userFinished(QString theHost, QString theLogin, QString thePassword);
    /// The user clicked the cancel button.
    void onConnectAborted();
    
};

#endif
