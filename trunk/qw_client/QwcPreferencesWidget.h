#ifndef QWCPREFERENCESWIDGET_H
#define QWCPREFERENCESWIDGET_H

#include <QWidget>
#include "ui_QwcPreferencesWidget.h"

class QwcPreferencesWidget :
        public QWidget,
        private Ui::QwcPreferencesWidget
{
    Q_OBJECT

public:
    QwcPreferencesWidget(QWidget *parent = 0);
    ~QwcPreferencesWidget();

    void savePreferences();
    void loadPreferences();


private:
    void initEventsMenu();
    void initChatStyleMenu();

    
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    // General
    void on_btnGeneralSetIcon_clicked();
    void on_btnGeneralDefaultIcon_clicked();


    // Interface
    void on_btnChatFont_clicked();
    void on_btnUserlistFont_clicked();
    void on_btnNewsFont_clicked();

    // Files
    void on_btnFilesFolderSet_clicked();

    // Trackers
    void on_fBtnTrackerNew_clicked();
    void on_fBtnTrackerDelete_clicked();
    void on_fTrackerList_currentRowChanged(int);
    void on_fTrackerName_editingFinished();
    void on_fTrackerAddress_editingFinished();

    // Bookmarks
    void on_fBookmarksList_currentRowChanged(int);
    void on_btnBMAdd_clicked();
    void on_btnBMRemove_clicked();
    void on_fBMName_editingFinished();
    void on_fBMAddr_editingFinished();
    void on_fBMLogin_editingFinished();
    void on_fBMPassword_editingFinished();
    void on_fBMAutoConnect_stateChanged(int);

    // Events
    void on_fEventsList_currentIndexChanged(int index);
    void on_fEventsSysCmd_editingFinished();
    void on_fEventsSound_clicked();
    void on_fEventsPostChat_stateChanged(int state);
    void on_fEventsDisplayTrayMsg_stateChanged(int state);
    void on_fEventsSysCmdEnabled_stateChanged(int state);
};

#endif
