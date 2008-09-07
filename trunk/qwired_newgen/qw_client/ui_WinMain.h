/********************************************************************************
** Form generated from reading ui file 'WinMain.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WINMAIN_H
#define UI_WINMAIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

class Ui_MainWindow
{
public:
    QAction *actionNewConnection;
    QAction *actionDisconnect;
    QAction *actionReconnect;
    QAction *actionServerInfo;
    QAction *actionChat;
    QAction *actionNews;
    QAction *actionMessages;
    QAction *actionFiles;
    QAction *actionTransfers;
    QAction *actionAccounts;
    QAction *actionInformation;
    QAction *actionSetTopic;
    QAction *actionNewsPost;
    QAction *actionBroadcast;
    QAction *actionSearch;
    QAction *actionPreferences;
    QAction *actionAbout;
    QAction *actionTrackers;
    QWidget *centralwidget;
    QHBoxLayout *hboxLayout;
    QMenuBar *menubar;
    QMenu *menuQwired;
    QStatusBar *statusbar;
    QToolBar *fToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(800, 600);
    actionNewConnection = new QAction(MainWindow);
    actionNewConnection->setObjectName(QString::fromUtf8("actionNewConnection"));
    actionDisconnect = new QAction(MainWindow);
    actionDisconnect->setObjectName(QString::fromUtf8("actionDisconnect"));
    actionDisconnect->setEnabled(false);
    actionDisconnect->setIcon(QIcon(QString::fromUtf8(":/icons/disconnect.png")));
    actionReconnect = new QAction(MainWindow);
    actionReconnect->setObjectName(QString::fromUtf8("actionReconnect"));
    actionReconnect->setEnabled(false);
    actionServerInfo = new QAction(MainWindow);
    actionServerInfo->setObjectName(QString::fromUtf8("actionServerInfo"));
    actionServerInfo->setEnabled(false);
    actionChat = new QAction(MainWindow);
    actionChat->setObjectName(QString::fromUtf8("actionChat"));
    actionChat->setEnabled(false);
    actionNews = new QAction(MainWindow);
    actionNews->setObjectName(QString::fromUtf8("actionNews"));
    actionNews->setEnabled(false);
    actionNews->setIcon(QIcon(QString::fromUtf8(":/icons/tb_news.png")));
    actionMessages = new QAction(MainWindow);
    actionMessages->setObjectName(QString::fromUtf8("actionMessages"));
    actionMessages->setEnabled(false);
    actionMessages->setIcon(QIcon(QString::fromUtf8(":/icons/icon_msg_normal.png")));
    actionFiles = new QAction(MainWindow);
    actionFiles->setObjectName(QString::fromUtf8("actionFiles"));
    actionFiles->setEnabled(false);
    actionFiles->setIcon(QIcon(QString::fromUtf8(":/icons/files.png")));
    actionTransfers = new QAction(MainWindow);
    actionTransfers->setObjectName(QString::fromUtf8("actionTransfers"));
    actionTransfers->setEnabled(false);
    actionTransfers->setIcon(QIcon(QString::fromUtf8(":/icons/transfers.png")));
    actionAccounts = new QAction(MainWindow);
    actionAccounts->setObjectName(QString::fromUtf8("actionAccounts"));
    actionAccounts->setEnabled(false);
    actionAccounts->setIcon(QIcon(QString::fromUtf8(":/icons/accounts.png")));
    actionInformation = new QAction(MainWindow);
    actionInformation->setObjectName(QString::fromUtf8("actionInformation"));
    actionInformation->setEnabled(false);
    actionSetTopic = new QAction(MainWindow);
    actionSetTopic->setObjectName(QString::fromUtf8("actionSetTopic"));
    actionSetTopic->setEnabled(false);
    actionNewsPost = new QAction(MainWindow);
    actionNewsPost->setObjectName(QString::fromUtf8("actionNewsPost"));
    actionNewsPost->setEnabled(false);
    actionBroadcast = new QAction(MainWindow);
    actionBroadcast->setObjectName(QString::fromUtf8("actionBroadcast"));
    actionBroadcast->setEnabled(false);
    actionSearch = new QAction(MainWindow);
    actionSearch->setObjectName(QString::fromUtf8("actionSearch"));
    actionSearch->setEnabled(false);
    actionSearch->setIcon(QIcon(QString::fromUtf8(":/icons/filesearch.png")));
    actionPreferences = new QAction(MainWindow);
    actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
    actionAbout = new QAction(MainWindow);
    actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
    actionTrackers = new QAction(MainWindow);
    actionTrackers->setObjectName(QString::fromUtf8("actionTrackers"));
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    hboxLayout = new QHBoxLayout(centralwidget);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    MainWindow->setCentralWidget(centralwidget);
    menubar = new QMenuBar(MainWindow);
    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 800, 25));
    menuQwired = new QMenu(menubar);
    menuQwired->setObjectName(QString::fromUtf8("menuQwired"));
    MainWindow->setMenuBar(menubar);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    MainWindow->setStatusBar(statusbar);
    fToolBar = new QToolBar(MainWindow);
    fToolBar->setObjectName(QString::fromUtf8("fToolBar"));
    fToolBar->setMovable(false);
    fToolBar->setIconSize(QSize(32, 32));
    fToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    fToolBar->setFloatable(true);
    MainWindow->addToolBar(Qt::TopToolBarArea, fToolBar);

    menubar->addAction(menuQwired->menuAction());
    menuQwired->addAction(actionNewConnection);
    menuQwired->addAction(actionDisconnect);
    menuQwired->addAction(actionReconnect);
    menuQwired->addSeparator();
    menuQwired->addAction(actionServerInfo);
    menuQwired->addAction(actionNews);
    menuQwired->addAction(actionMessages);
    menuQwired->addAction(actionFiles);
    menuQwired->addAction(actionTransfers);
    menuQwired->addSeparator();
    menuQwired->addAction(actionAccounts);
    menuQwired->addAction(actionBroadcast);
    menuQwired->addSeparator();
    menuQwired->addAction(actionTrackers);
    menuQwired->addAction(actionPreferences);
    menuQwired->addAction(actionAbout);
    fToolBar->addAction(actionNews);
    fToolBar->addAction(actionFiles);
    fToolBar->addAction(actionSearch);
    fToolBar->addAction(actionTransfers);
    fToolBar->addAction(actionMessages);
    fToolBar->addSeparator();
    fToolBar->addAction(actionDisconnect);

    retranslateUi(MainWindow);

    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Qwired", 0, QApplication::UnicodeUTF8));
    actionNewConnection->setText(QApplication::translate("MainWindow", "New Connection...", 0, QApplication::UnicodeUTF8));
    actionNewConnection->setShortcut(QApplication::translate("MainWindow", "Ctrl+K", 0, QApplication::UnicodeUTF8));
    actionDisconnect->setText(QApplication::translate("MainWindow", "Disconnect...", 0, QApplication::UnicodeUTF8));
    actionDisconnect->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0, QApplication::UnicodeUTF8));
    actionReconnect->setText(QApplication::translate("MainWindow", "Reconnect", 0, QApplication::UnicodeUTF8));
    actionReconnect->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+K", 0, QApplication::UnicodeUTF8));
    actionServerInfo->setText(QApplication::translate("MainWindow", "Server Information", 0, QApplication::UnicodeUTF8));
    actionServerInfo->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+I", 0, QApplication::UnicodeUTF8));
    actionChat->setText(QApplication::translate("MainWindow", "Chat", 0, QApplication::UnicodeUTF8));
    actionChat->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0, QApplication::UnicodeUTF8));
    actionNews->setText(QApplication::translate("MainWindow", "News", 0, QApplication::UnicodeUTF8));
    actionNews->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", 0, QApplication::UnicodeUTF8));
    actionMessages->setText(QApplication::translate("MainWindow", "Messages", 0, QApplication::UnicodeUTF8));
    actionMessages->setShortcut(QApplication::translate("MainWindow", "Ctrl+E", 0, QApplication::UnicodeUTF8));
    actionFiles->setText(QApplication::translate("MainWindow", "Files", 0, QApplication::UnicodeUTF8));
    actionFiles->setShortcut(QApplication::translate("MainWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
    actionTransfers->setText(QApplication::translate("MainWindow", "Transfers", 0, QApplication::UnicodeUTF8));
    actionTransfers->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
    actionAccounts->setText(QApplication::translate("MainWindow", "Accounts", 0, QApplication::UnicodeUTF8));
    actionAccounts->setShortcut(QApplication::translate("MainWindow", "Ctrl+U", 0, QApplication::UnicodeUTF8));
    actionInformation->setText(QApplication::translate("MainWindow", "Get Information", 0, QApplication::UnicodeUTF8));
    actionSetTopic->setText(QApplication::translate("MainWindow", "Set Topic...", 0, QApplication::UnicodeUTF8));
    actionNewsPost->setText(QApplication::translate("MainWindow", "Post News...", 0, QApplication::UnicodeUTF8));
    actionBroadcast->setText(QApplication::translate("MainWindow", "Broadcast Message...", 0, QApplication::UnicodeUTF8));
    actionBroadcast->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+B", 0, QApplication::UnicodeUTF8));
    actionSearch->setText(QApplication::translate("MainWindow", "Search", 0, QApplication::UnicodeUTF8));
    actionSearch->setShortcut(QApplication::translate("MainWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
    actionPreferences->setText(QApplication::translate("MainWindow", "Preferences...", 0, QApplication::UnicodeUTF8));
    actionAbout->setText(QApplication::translate("MainWindow", "About QWired...", 0, QApplication::UnicodeUTF8));
    actionTrackers->setText(QApplication::translate("MainWindow", "Trackers", 0, QApplication::UnicodeUTF8));
    actionTrackers->setToolTip(QApplication::translate("MainWindow", "Show a list of configured trackers and servers registered with them.", 0, QApplication::UnicodeUTF8));
    actionTrackers->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0, QApplication::UnicodeUTF8));
    menuQwired->setTitle(QApplication::translate("MainWindow", "Qwired", 0, QApplication::UnicodeUTF8));
    fToolBar->setWindowTitle(QApplication::translate("MainWindow", "Toolbar", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif // UI_WINMAIN_H
