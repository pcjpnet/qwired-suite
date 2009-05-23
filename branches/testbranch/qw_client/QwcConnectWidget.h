#ifndef QwcConnectWidget_H
#define QwcConnectWidget_H

#include "ui_QwcConnectWidget.h"
#include "QwcGlobals.h"

class QwcConnectWidget : public QWidget, public Ui::QwcConnectWidget
{

    Q_OBJECT

private:
    int pReconnectTimerId;


public:
    QwcConnectWidget(QWidget *parent = 0);

private slots:
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
