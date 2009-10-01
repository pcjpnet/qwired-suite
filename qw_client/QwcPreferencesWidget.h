#ifndef QWCPREFERENCESWIDGET_H
#define QWCPREFERENCESWIDGET_H

#include <QWidget>
#include "ui_QwcPreferencesWidget.h"

class QwcPreferencesWidget : public QWidget, private Ui::QwcPreferencesWidget
{
    Q_OBJECT

public:
    QwcPreferencesWidget(QWidget *parent = 0);
    ~QwcPreferencesWidget();

private:
    void initPrefs();
    void initEventsMenu();
    bool pNickChanged;
    bool pStatusChanged;
    bool pIconChanged;

    
private slots:
    void savePrefs();

    // General
    void on_btnGeneralSetIcon_clicked();
    void on_btnGeneralDefaultIcon_clicked();
    void on_fNickname_editingFinished();
    void on_fStatus_editingFinished();

    // Interface, Chat
    void on_fBtnIfChatFont_clicked();
    void on_fBtnIfUserlistFont_clicked();
    void on_fBtnIfUserlistCompactLarge_toggled(bool checked);
    void on_fBtnIfUserlistCompactSmall_toggled(bool checked);
    void on_fBtnIfUserlistAlternateRow_stateChanged(int state);

    // Interface, News
    void on_fBtnIfNewsFont_clicked();

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
    void on_fEventsList_currentRowChanged(int);
    void on_fEventsSysCmd_editingFinished();
    void on_fEventsSound_clicked();
    void on_fBtnOk_clicked();
    void on_fEventsPostChat_stateChanged(int state);
    void on_fEventsDisplayTrayMsg_stateChanged(int state);
    void on_fEventsSysCmdEnabled_stateChanged(int state);
};

#endif
