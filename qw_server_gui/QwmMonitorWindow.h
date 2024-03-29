#ifndef QWMMONITORWINDOW_H
#define QWMMONITORWINDOW_H

#include "ui_QwmMonitorWindow.h"

class QwmMonitorWindow : public QWidget, public Ui_QwmMonitorWindow
{
    Q_OBJECT

public:
    QwmMonitorWindow(QWidget *parent = 0);

private slots:
    //void on_myButton_clicked();
    void on_fTransfersList_currentRowChanged(int index);
    void on_btnTransfersAbort_clicked();
    void on_btnTransfersDisconnectUser_clicked();
    void on_fUsersList_itemSelectionChanged();

    void on_btnConfigurationSetBanner_clicked();

    void on_configurationTrackersList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_btnConfigurationTrackersAdd_clicked();
    void on_btnConfigurationTrackersRemove_clicked();
    void handleTrackersChanged();


signals:
    void requestedTransferAbort(QString transferId);
    void requestedUserKick(int userId);
    /*! A new banner file has been selected by the user. */
    void selectedNewBanner(QImage data);
    /*! The user has modified the list of tracker servers. */
    void updatedTrackerList(QStringList trackers);


};

#endif // QwmMonitorWindow_H
