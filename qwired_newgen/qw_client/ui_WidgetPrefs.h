/********************************************************************************
** Form generated from reading ui file 'WidgetPrefs.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETPREFS_H
#define UI_WIDGETPREFS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "gui/WidgetColorButton.h"

class Ui_WidgetPrefs
{
public:
    QVBoxLayout *vboxLayout;
    QTabWidget *tabWidget;
    QWidget *tab_general;
    QHBoxLayout *hboxLayout;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *fNickname;
    QLabel *label_2;
    QLineEdit *fStatus;
    QLabel *label_3;
    QLabel *fIcon;
    QLabel *label_7;
    QHBoxLayout *hboxLayout1;
    QToolButton *btnGeneralDefaultIcon;
    QToolButton *btnGeneralSetIcon;
    QSpacerItem *spacerItem;
    QComboBox *fLanguage;
    QLabel *label_28;
    QSpacerItem *spacerItem1;
    QWidget *tab_interface;
    QHBoxLayout *hboxLayout2;
    QTabWidget *tabWidget_2;
    QWidget *fTabUiChat;
    QVBoxLayout *vboxLayout1;
    QSpacerItem *spacerItem2;
    QGridLayout *gridLayout1;
    QLabel *label_6;
    QLabel *label_12;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QHBoxLayout *hboxLayout3;
    WidgetColorButton *fBtnIfChatText;
    QLabel *label_13;
    WidgetColorButton *fBtnIfChatBack;
    QSpacerItem *spacerItem3;
    QHBoxLayout *hboxLayout4;
    WidgetColorButton *fBtnIfChatURLs;
    QLabel *label_14;
    WidgetColorButton *fBtnIfChatEvents;
    QSpacerItem *spacerItem4;
    QHBoxLayout *hboxLayout5;
    QRadioButton *fBtnIfUserlistCompactLarge;
    QRadioButton *fBtnIfUserlistCompactSmall;
    QSpacerItem *spacerItem5;
    QCheckBox *fBtnIfUserlistAlternateRow;
    QLineEdit *fIfChatFont;
    QLineEdit *fIfUserlistFont;
    QToolButton *fBtnIfChatFont;
    QToolButton *fBtnIfUserlistFont;
    QSpacerItem *spacerItem6;
    QWidget *fTabUiNews;
    QVBoxLayout *vboxLayout2;
    QSpacerItem *spacerItem7;
    QGridLayout *gridLayout2;
    QLabel *label_22;
    QLabel *label_24;
    QLabel *label_25;
    QHBoxLayout *hboxLayout6;
    WidgetColorButton *fBtnIfNewsText;
    QLabel *label_23;
    WidgetColorButton *fBtnIfNewsBack;
    QSpacerItem *spacerItem8;
    QHBoxLayout *hboxLayout7;
    WidgetColorButton *fBtnIfNewsTitles;
    QSpacerItem *spacerItem9;
    QLineEdit *fIfNewsFont;
    QToolButton *fBtnIfNewsFont;
    QSpacerItem *spacerItem10;
    QWidget *fTabUiMessages;
    QWidget *fTabUiFiles;
    QVBoxLayout *vboxLayout3;
    QSpacerItem *spacerItem11;
    QGridLayout *gridLayout3;
    QLabel *label_21;
    QCheckBox *fBtnIfFilesAlternateRow;
    QLineEdit *fIfFilesFont;
    QToolButton *fBtnIfFilesFont;
    QSpacerItem *spacerItem12;
    QWidget *fTabUiPreview;
    QVBoxLayout *vboxLayout4;
    QSpacerItem *spacerItem13;
    QGridLayout *gridLayout4;
    QLabel *label_18;
    QLabel *label_20;
    QHBoxLayout *hboxLayout8;
    QToolButton *toolButton_5;
    QLabel *label_19;
    QToolButton *toolButton_6;
    QSpacerItem *spacerItem14;
    QLineEdit *lineEdit_5;
    QToolButton *toolButton_14;
    QSpacerItem *spacerItem15;
    QWidget *fTabUiTrackers;
    QVBoxLayout *vboxLayout5;
    QSpacerItem *spacerItem16;
    QCheckBox *checkBox_2;
    QSpacerItem *spacerItem17;
    QWidget *tab_bookmarks;
    QHBoxLayout *hboxLayout9;
    QVBoxLayout *vboxLayout6;
    QListWidget *fBookmarksList;
    QHBoxLayout *hboxLayout10;
    QToolButton *btnBMAdd;
    QToolButton *btnBMRemove;
    QSpacerItem *spacerItem18;
    QFrame *fBMGroup;
    QHBoxLayout *hboxLayout11;
    QGridLayout *gridLayout5;
    QLabel *label_8;
    QLineEdit *fBMName;
    QLabel *label_9;
    QLineEdit *fBMAddr;
    QLabel *label_10;
    QLineEdit *fBMLogin;
    QLabel *label_11;
    QLineEdit *fBMPassword;
    QCheckBox *fBMAutoConnect;
    QSpacerItem *spacerItem19;
    QWidget *tab_chat;
    QGridLayout *gridLayout6;
    QSpacerItem *spacerItem20;
    QSpacerItem *spacerItem21;
    QGridLayout *gridLayout7;
    QLabel *label_29;
    QHBoxLayout *hboxLayout12;
    QRadioButton *fChatStyleQwired;
    QRadioButton *fChatStyleIRC;
    QSpacerItem *spacerItem22;
    QLabel *label_31;
    QVBoxLayout *vboxLayout7;
    QCheckBox *fChatTimestampLine;
    QHBoxLayout *hboxLayout13;
    QSpacerItem *spacerItem23;
    QLabel *label_30;
    WidgetColorButton *fChatTimeColor;
    QSpacerItem *spacerItem24;
    QSpacerItem *spacerItem25;
    QVBoxLayout *vboxLayout8;
    QCheckBox *fChatEmoticons;
    QHBoxLayout *hboxLayout14;
    QSpacerItem *spacerItem26;
    QLabel *label_32;
    QLabel *label_33;
    QLabel *label_34;
    QSpacerItem *spacerItem27;
    QSpacerItem *spacerItem28;
    QSpacerItem *spacerItem29;
    QSpacerItem *spacerItem30;
    QWidget *Seite;
    QHBoxLayout *hboxLayout15;
    QListWidget *fEventsList;
    QVBoxLayout *vboxLayout9;
    QSpacerItem *spacerItem31;
    QHBoxLayout *hboxLayout16;
    QLabel *label_35;
    QToolButton *fEventsSound;
    QCheckBox *fEventsDisplayTrayMsg;
    QCheckBox *fEventsPostChat;
    QVBoxLayout *vboxLayout10;
    QCheckBox *fEventsSysCmdEnabled;
    QLineEdit *fEventsSysCmd;
    QSpacerItem *spacerItem32;
    QWidget *tab_files;
    QGridLayout *gridLayout8;
    QLabel *label_4;
    QLineEdit *fFilesFolder;
    QToolButton *btnFilesFolderSet;
    QLabel *label_5;
    QCheckBox *fFilesQueue;
    QCheckBox *fFilesEncrypt;
    QSpacerItem *spacerItem33;
    QCheckBox *fFilesOpenInNewWindow;
    QWidget *tab_trackers;
    QHBoxLayout *hboxLayout17;
    QVBoxLayout *vboxLayout11;
    QListWidget *fTrackerList;
    QHBoxLayout *hboxLayout18;
    QToolButton *fBtnTrackerNew;
    QToolButton *fBtnTrackerDelete;
    QSpacerItem *spacerItem34;
    QGroupBox *fTrackerGroup;
    QGridLayout *gridLayout9;
    QLabel *label_26;
    QLineEdit *fTrackerName;
    QLabel *label_27;
    QLineEdit *fTrackerAddress;
    QSpacerItem *spacerItem35;
    QHBoxLayout *hboxLayout19;
    QSpacerItem *spacerItem36;
    QPushButton *fBtnOk;

    void setupUi(QWidget *WidgetPrefs)
    {
    if (WidgetPrefs->objectName().isEmpty())
        WidgetPrefs->setObjectName(QString::fromUtf8("WidgetPrefs"));
    WidgetPrefs->resize(640, 408);
    vboxLayout = new QVBoxLayout(WidgetPrefs);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    tabWidget = new QTabWidget(WidgetPrefs);
    tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
    tabWidget->setTabShape(QTabWidget::Rounded);
    tab_general = new QWidget();
    tab_general->setObjectName(QString::fromUtf8("tab_general"));
    hboxLayout = new QHBoxLayout(tab_general);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setHorizontalSpacing(6);
    gridLayout->setVerticalSpacing(4);
    gridLayout->setContentsMargins(4, 4, 4, 4);
    label = new QLabel(tab_general);
    label->setObjectName(QString::fromUtf8("label"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    fNickname = new QLineEdit(tab_general);
    fNickname->setObjectName(QString::fromUtf8("fNickname"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(4);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fNickname->sizePolicy().hasHeightForWidth());
    fNickname->setSizePolicy(sizePolicy1);

    gridLayout->addWidget(fNickname, 0, 1, 1, 2);

    label_2 = new QLabel(tab_general);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
    label_2->setSizePolicy(sizePolicy);
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    fStatus = new QLineEdit(tab_general);
    fStatus->setObjectName(QString::fromUtf8("fStatus"));
    sizePolicy1.setHeightForWidth(fStatus->sizePolicy().hasHeightForWidth());
    fStatus->setSizePolicy(sizePolicy1);

    gridLayout->addWidget(fStatus, 1, 1, 1, 2);

    label_3 = new QLabel(tab_general);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
    label_3->setSizePolicy(sizePolicy);
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_3, 2, 0, 1, 1);

    fIcon = new QLabel(tab_general);
    fIcon->setObjectName(QString::fromUtf8("fIcon"));
    QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(fIcon->sizePolicy().hasHeightForWidth());
    fIcon->setSizePolicy(sizePolicy2);
    fIcon->setMinimumSize(QSize(34, 34));
    fIcon->setMaximumSize(QSize(34, 34));
    fIcon->setFrameShape(QFrame::Panel);
    fIcon->setFrameShadow(QFrame::Sunken);
    fIcon->setPixmap(QPixmap());

    gridLayout->addWidget(fIcon, 2, 1, 1, 1);

    label_7 = new QLabel(tab_general);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    QFont font;
    font.setPointSize(10);
    label_7->setFont(font);

    gridLayout->addWidget(label_7, 2, 2, 1, 1);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    btnGeneralDefaultIcon = new QToolButton(tab_general);
    btnGeneralDefaultIcon->setObjectName(QString::fromUtf8("btnGeneralDefaultIcon"));
    btnGeneralDefaultIcon->setFont(font);

    hboxLayout1->addWidget(btnGeneralDefaultIcon);

    btnGeneralSetIcon = new QToolButton(tab_general);
    btnGeneralSetIcon->setObjectName(QString::fromUtf8("btnGeneralSetIcon"));
    btnGeneralSetIcon->setFont(font);

    hboxLayout1->addWidget(btnGeneralSetIcon);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);


    gridLayout->addLayout(hboxLayout1, 3, 2, 1, 1);

    fLanguage = new QComboBox(tab_general);
    fLanguage->setObjectName(QString::fromUtf8("fLanguage"));

    gridLayout->addWidget(fLanguage, 4, 1, 1, 2);

    label_28 = new QLabel(tab_general);
    label_28->setObjectName(QString::fromUtf8("label_28"));
    label_28->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_28, 4, 0, 1, 1);

    spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem1, 5, 1, 1, 1);


    hboxLayout->addLayout(gridLayout);

    tabWidget->addTab(tab_general, QString());
    tab_interface = new QWidget();
    tab_interface->setObjectName(QString::fromUtf8("tab_interface"));
    hboxLayout2 = new QHBoxLayout(tab_interface);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    tabWidget_2 = new QTabWidget(tab_interface);
    tabWidget_2->setObjectName(QString::fromUtf8("tabWidget_2"));
    fTabUiChat = new QWidget();
    fTabUiChat->setObjectName(QString::fromUtf8("fTabUiChat"));
    vboxLayout1 = new QVBoxLayout(fTabUiChat);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    spacerItem2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem2);

    gridLayout1 = new QGridLayout();
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    label_6 = new QLabel(fTabUiChat);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_6, 0, 0, 1, 1);

    label_12 = new QLabel(fTabUiChat);
    label_12->setObjectName(QString::fromUtf8("label_12"));
    label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_12, 1, 0, 1, 1);

    label_15 = new QLabel(fTabUiChat);
    label_15->setObjectName(QString::fromUtf8("label_15"));
    label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_15, 2, 0, 1, 1);

    label_16 = new QLabel(fTabUiChat);
    label_16->setObjectName(QString::fromUtf8("label_16"));
    label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_16, 3, 0, 1, 1);

    label_17 = new QLabel(fTabUiChat);
    label_17->setObjectName(QString::fromUtf8("label_17"));
    label_17->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_17, 4, 0, 1, 1);

    hboxLayout3 = new QHBoxLayout();
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    fBtnIfChatText = new WidgetColorButton(fTabUiChat);
    fBtnIfChatText->setObjectName(QString::fromUtf8("fBtnIfChatText"));

    hboxLayout3->addWidget(fBtnIfChatText);

    label_13 = new QLabel(fTabUiChat);
    label_13->setObjectName(QString::fromUtf8("label_13"));
    label_13->setMinimumSize(QSize(100, 0));
    label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout3->addWidget(label_13);

    fBtnIfChatBack = new WidgetColorButton(fTabUiChat);
    fBtnIfChatBack->setObjectName(QString::fromUtf8("fBtnIfChatBack"));

    hboxLayout3->addWidget(fBtnIfChatBack);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout3->addItem(spacerItem3);


    gridLayout1->addLayout(hboxLayout3, 0, 1, 1, 1);

    hboxLayout4 = new QHBoxLayout();
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    fBtnIfChatURLs = new WidgetColorButton(fTabUiChat);
    fBtnIfChatURLs->setObjectName(QString::fromUtf8("fBtnIfChatURLs"));
    fBtnIfChatURLs->setEnabled(false);

    hboxLayout4->addWidget(fBtnIfChatURLs);

    label_14 = new QLabel(fTabUiChat);
    label_14->setObjectName(QString::fromUtf8("label_14"));
    label_14->setMinimumSize(QSize(100, 0));
    label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout4->addWidget(label_14);

    fBtnIfChatEvents = new WidgetColorButton(fTabUiChat);
    fBtnIfChatEvents->setObjectName(QString::fromUtf8("fBtnIfChatEvents"));

    hboxLayout4->addWidget(fBtnIfChatEvents);

    spacerItem4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout4->addItem(spacerItem4);


    gridLayout1->addLayout(hboxLayout4, 1, 1, 1, 1);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    fBtnIfUserlistCompactLarge = new QRadioButton(fTabUiChat);
    fBtnIfUserlistCompactLarge->setObjectName(QString::fromUtf8("fBtnIfUserlistCompactLarge"));

    hboxLayout5->addWidget(fBtnIfUserlistCompactLarge);

    fBtnIfUserlistCompactSmall = new QRadioButton(fTabUiChat);
    fBtnIfUserlistCompactSmall->setObjectName(QString::fromUtf8("fBtnIfUserlistCompactSmall"));

    hboxLayout5->addWidget(fBtnIfUserlistCompactSmall);

    spacerItem5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem5);


    gridLayout1->addLayout(hboxLayout5, 4, 1, 1, 1);

    fBtnIfUserlistAlternateRow = new QCheckBox(fTabUiChat);
    fBtnIfUserlistAlternateRow->setObjectName(QString::fromUtf8("fBtnIfUserlistAlternateRow"));

    gridLayout1->addWidget(fBtnIfUserlistAlternateRow, 5, 1, 1, 1);

    fIfChatFont = new QLineEdit(fTabUiChat);
    fIfChatFont->setObjectName(QString::fromUtf8("fIfChatFont"));
    fIfChatFont->setReadOnly(true);

    gridLayout1->addWidget(fIfChatFont, 2, 1, 1, 1);

    fIfUserlistFont = new QLineEdit(fTabUiChat);
    fIfUserlistFont->setObjectName(QString::fromUtf8("fIfUserlistFont"));
    fIfUserlistFont->setReadOnly(true);

    gridLayout1->addWidget(fIfUserlistFont, 3, 1, 1, 1);

    fBtnIfChatFont = new QToolButton(fTabUiChat);
    fBtnIfChatFont->setObjectName(QString::fromUtf8("fBtnIfChatFont"));

    gridLayout1->addWidget(fBtnIfChatFont, 2, 2, 1, 1);

    fBtnIfUserlistFont = new QToolButton(fTabUiChat);
    fBtnIfUserlistFont->setObjectName(QString::fromUtf8("fBtnIfUserlistFont"));

    gridLayout1->addWidget(fBtnIfUserlistFont, 3, 2, 1, 1);


    vboxLayout1->addLayout(gridLayout1);

    spacerItem6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem6);

    tabWidget_2->addTab(fTabUiChat, QString());
    fTabUiNews = new QWidget();
    fTabUiNews->setObjectName(QString::fromUtf8("fTabUiNews"));
    vboxLayout2 = new QVBoxLayout(fTabUiNews);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    spacerItem7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout2->addItem(spacerItem7);

    gridLayout2 = new QGridLayout();
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    label_22 = new QLabel(fTabUiNews);
    label_22->setObjectName(QString::fromUtf8("label_22"));
    label_22->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_22, 0, 0, 1, 1);

    label_24 = new QLabel(fTabUiNews);
    label_24->setObjectName(QString::fromUtf8("label_24"));
    label_24->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_24, 1, 0, 1, 1);

    label_25 = new QLabel(fTabUiNews);
    label_25->setObjectName(QString::fromUtf8("label_25"));
    label_25->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout2->addWidget(label_25, 2, 0, 1, 1);

    hboxLayout6 = new QHBoxLayout();
    hboxLayout6->setObjectName(QString::fromUtf8("hboxLayout6"));
    fBtnIfNewsText = new WidgetColorButton(fTabUiNews);
    fBtnIfNewsText->setObjectName(QString::fromUtf8("fBtnIfNewsText"));

    hboxLayout6->addWidget(fBtnIfNewsText);

    label_23 = new QLabel(fTabUiNews);
    label_23->setObjectName(QString::fromUtf8("label_23"));
    label_23->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout6->addWidget(label_23);

    fBtnIfNewsBack = new WidgetColorButton(fTabUiNews);
    fBtnIfNewsBack->setObjectName(QString::fromUtf8("fBtnIfNewsBack"));

    hboxLayout6->addWidget(fBtnIfNewsBack);

    spacerItem8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout6->addItem(spacerItem8);


    gridLayout2->addLayout(hboxLayout6, 0, 1, 1, 1);

    hboxLayout7 = new QHBoxLayout();
    hboxLayout7->setObjectName(QString::fromUtf8("hboxLayout7"));
    fBtnIfNewsTitles = new WidgetColorButton(fTabUiNews);
    fBtnIfNewsTitles->setObjectName(QString::fromUtf8("fBtnIfNewsTitles"));

    hboxLayout7->addWidget(fBtnIfNewsTitles);

    spacerItem9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout7->addItem(spacerItem9);


    gridLayout2->addLayout(hboxLayout7, 1, 1, 1, 1);

    fIfNewsFont = new QLineEdit(fTabUiNews);
    fIfNewsFont->setObjectName(QString::fromUtf8("fIfNewsFont"));

    gridLayout2->addWidget(fIfNewsFont, 2, 1, 1, 1);

    fBtnIfNewsFont = new QToolButton(fTabUiNews);
    fBtnIfNewsFont->setObjectName(QString::fromUtf8("fBtnIfNewsFont"));

    gridLayout2->addWidget(fBtnIfNewsFont, 2, 2, 1, 1);


    vboxLayout2->addLayout(gridLayout2);

    spacerItem10 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout2->addItem(spacerItem10);

    tabWidget_2->addTab(fTabUiNews, QString());
    fTabUiMessages = new QWidget();
    fTabUiMessages->setObjectName(QString::fromUtf8("fTabUiMessages"));
    tabWidget_2->addTab(fTabUiMessages, QString());
    fTabUiFiles = new QWidget();
    fTabUiFiles->setObjectName(QString::fromUtf8("fTabUiFiles"));
    vboxLayout3 = new QVBoxLayout(fTabUiFiles);
    vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
    spacerItem11 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout3->addItem(spacerItem11);

    gridLayout3 = new QGridLayout();
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    label_21 = new QLabel(fTabUiFiles);
    label_21->setObjectName(QString::fromUtf8("label_21"));

    gridLayout3->addWidget(label_21, 0, 0, 1, 1);

    fBtnIfFilesAlternateRow = new QCheckBox(fTabUiFiles);
    fBtnIfFilesAlternateRow->setObjectName(QString::fromUtf8("fBtnIfFilesAlternateRow"));

    gridLayout3->addWidget(fBtnIfFilesAlternateRow, 1, 1, 1, 1);

    fIfFilesFont = new QLineEdit(fTabUiFiles);
    fIfFilesFont->setObjectName(QString::fromUtf8("fIfFilesFont"));

    gridLayout3->addWidget(fIfFilesFont, 0, 1, 1, 1);

    fBtnIfFilesFont = new QToolButton(fTabUiFiles);
    fBtnIfFilesFont->setObjectName(QString::fromUtf8("fBtnIfFilesFont"));

    gridLayout3->addWidget(fBtnIfFilesFont, 0, 2, 1, 1);


    vboxLayout3->addLayout(gridLayout3);

    spacerItem12 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout3->addItem(spacerItem12);

    tabWidget_2->addTab(fTabUiFiles, QString());
    fTabUiPreview = new QWidget();
    fTabUiPreview->setObjectName(QString::fromUtf8("fTabUiPreview"));
    vboxLayout4 = new QVBoxLayout(fTabUiPreview);
    vboxLayout4->setObjectName(QString::fromUtf8("vboxLayout4"));
    spacerItem13 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout4->addItem(spacerItem13);

    gridLayout4 = new QGridLayout();
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    label_18 = new QLabel(fTabUiPreview);
    label_18->setObjectName(QString::fromUtf8("label_18"));
    label_18->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout4->addWidget(label_18, 0, 0, 1, 1);

    label_20 = new QLabel(fTabUiPreview);
    label_20->setObjectName(QString::fromUtf8("label_20"));
    label_20->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout4->addWidget(label_20, 1, 0, 1, 1);

    hboxLayout8 = new QHBoxLayout();
    hboxLayout8->setObjectName(QString::fromUtf8("hboxLayout8"));
    toolButton_5 = new QToolButton(fTabUiPreview);
    toolButton_5->setObjectName(QString::fromUtf8("toolButton_5"));

    hboxLayout8->addWidget(toolButton_5);

    label_19 = new QLabel(fTabUiPreview);
    label_19->setObjectName(QString::fromUtf8("label_19"));
    label_19->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout8->addWidget(label_19);

    toolButton_6 = new QToolButton(fTabUiPreview);
    toolButton_6->setObjectName(QString::fromUtf8("toolButton_6"));

    hboxLayout8->addWidget(toolButton_6);

    spacerItem14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout8->addItem(spacerItem14);


    gridLayout4->addLayout(hboxLayout8, 0, 1, 1, 1);

    lineEdit_5 = new QLineEdit(fTabUiPreview);
    lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));

    gridLayout4->addWidget(lineEdit_5, 1, 1, 1, 1);

    toolButton_14 = new QToolButton(fTabUiPreview);
    toolButton_14->setObjectName(QString::fromUtf8("toolButton_14"));

    gridLayout4->addWidget(toolButton_14, 1, 2, 1, 1);


    vboxLayout4->addLayout(gridLayout4);

    spacerItem15 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout4->addItem(spacerItem15);

    tabWidget_2->addTab(fTabUiPreview, QString());
    fTabUiTrackers = new QWidget();
    fTabUiTrackers->setObjectName(QString::fromUtf8("fTabUiTrackers"));
    vboxLayout5 = new QVBoxLayout(fTabUiTrackers);
    vboxLayout5->setObjectName(QString::fromUtf8("vboxLayout5"));
    spacerItem16 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout5->addItem(spacerItem16);

    checkBox_2 = new QCheckBox(fTabUiTrackers);
    checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

    vboxLayout5->addWidget(checkBox_2);

    spacerItem17 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout5->addItem(spacerItem17);

    tabWidget_2->addTab(fTabUiTrackers, QString());

    hboxLayout2->addWidget(tabWidget_2);

    tabWidget->addTab(tab_interface, QString());
    tab_bookmarks = new QWidget();
    tab_bookmarks->setObjectName(QString::fromUtf8("tab_bookmarks"));
    hboxLayout9 = new QHBoxLayout(tab_bookmarks);
    hboxLayout9->setObjectName(QString::fromUtf8("hboxLayout9"));
    vboxLayout6 = new QVBoxLayout();
    vboxLayout6->setObjectName(QString::fromUtf8("vboxLayout6"));
    fBookmarksList = new QListWidget(tab_bookmarks);
    fBookmarksList->setObjectName(QString::fromUtf8("fBookmarksList"));
    fBookmarksList->setMaximumSize(QSize(150, 16777215));

    vboxLayout6->addWidget(fBookmarksList);

    hboxLayout10 = new QHBoxLayout();
    hboxLayout10->setObjectName(QString::fromUtf8("hboxLayout10"));
    btnBMAdd = new QToolButton(tab_bookmarks);
    btnBMAdd->setObjectName(QString::fromUtf8("btnBMAdd"));
    btnBMAdd->setFont(font);

    hboxLayout10->addWidget(btnBMAdd);

    btnBMRemove = new QToolButton(tab_bookmarks);
    btnBMRemove->setObjectName(QString::fromUtf8("btnBMRemove"));
    btnBMRemove->setFont(font);

    hboxLayout10->addWidget(btnBMRemove);

    spacerItem18 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout10->addItem(spacerItem18);


    vboxLayout6->addLayout(hboxLayout10);


    hboxLayout9->addLayout(vboxLayout6);

    fBMGroup = new QFrame(tab_bookmarks);
    fBMGroup->setObjectName(QString::fromUtf8("fBMGroup"));
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy3.setHorizontalStretch(2);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(fBMGroup->sizePolicy().hasHeightForWidth());
    fBMGroup->setSizePolicy(sizePolicy3);
    fBMGroup->setFrameShape(QFrame::StyledPanel);
    fBMGroup->setFrameShadow(QFrame::Raised);
    hboxLayout11 = new QHBoxLayout(fBMGroup);
    hboxLayout11->setObjectName(QString::fromUtf8("hboxLayout11"));
    gridLayout5 = new QGridLayout();
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    label_8 = new QLabel(fBMGroup);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout5->addWidget(label_8, 0, 0, 1, 1);

    fBMName = new QLineEdit(fBMGroup);
    fBMName->setObjectName(QString::fromUtf8("fBMName"));

    gridLayout5->addWidget(fBMName, 0, 1, 1, 1);

    label_9 = new QLabel(fBMGroup);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout5->addWidget(label_9, 1, 0, 1, 1);

    fBMAddr = new QLineEdit(fBMGroup);
    fBMAddr->setObjectName(QString::fromUtf8("fBMAddr"));

    gridLayout5->addWidget(fBMAddr, 1, 1, 1, 1);

    label_10 = new QLabel(fBMGroup);
    label_10->setObjectName(QString::fromUtf8("label_10"));
    label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout5->addWidget(label_10, 2, 0, 1, 1);

    fBMLogin = new QLineEdit(fBMGroup);
    fBMLogin->setObjectName(QString::fromUtf8("fBMLogin"));

    gridLayout5->addWidget(fBMLogin, 2, 1, 1, 1);

    label_11 = new QLabel(fBMGroup);
    label_11->setObjectName(QString::fromUtf8("label_11"));
    label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout5->addWidget(label_11, 3, 0, 1, 1);

    fBMPassword = new QLineEdit(fBMGroup);
    fBMPassword->setObjectName(QString::fromUtf8("fBMPassword"));
    fBMPassword->setEchoMode(QLineEdit::Password);

    gridLayout5->addWidget(fBMPassword, 3, 1, 1, 1);

    fBMAutoConnect = new QCheckBox(fBMGroup);
    fBMAutoConnect->setObjectName(QString::fromUtf8("fBMAutoConnect"));

    gridLayout5->addWidget(fBMAutoConnect, 4, 0, 1, 2);

    spacerItem19 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout5->addItem(spacerItem19, 5, 0, 1, 2);


    hboxLayout11->addLayout(gridLayout5);


    hboxLayout9->addWidget(fBMGroup);

    tabWidget->addTab(tab_bookmarks, QString());
    tab_chat = new QWidget();
    tab_chat->setObjectName(QString::fromUtf8("tab_chat"));
    gridLayout6 = new QGridLayout(tab_chat);
    gridLayout6->setObjectName(QString::fromUtf8("gridLayout6"));
    spacerItem20 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout6->addItem(spacerItem20, 0, 2, 1, 1);

    spacerItem21 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout6->addItem(spacerItem21, 1, 0, 1, 1);

    gridLayout7 = new QGridLayout();
    gridLayout7->setObjectName(QString::fromUtf8("gridLayout7"));
    label_29 = new QLabel(tab_chat);
    label_29->setObjectName(QString::fromUtf8("label_29"));
    label_29->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout7->addWidget(label_29, 0, 0, 1, 1);

    hboxLayout12 = new QHBoxLayout();
    hboxLayout12->setObjectName(QString::fromUtf8("hboxLayout12"));
    fChatStyleQwired = new QRadioButton(tab_chat);
    fChatStyleQwired->setObjectName(QString::fromUtf8("fChatStyleQwired"));

    hboxLayout12->addWidget(fChatStyleQwired);

    fChatStyleIRC = new QRadioButton(tab_chat);
    fChatStyleIRC->setObjectName(QString::fromUtf8("fChatStyleIRC"));

    hboxLayout12->addWidget(fChatStyleIRC);

    spacerItem22 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout12->addItem(spacerItem22);


    gridLayout7->addLayout(hboxLayout12, 0, 1, 1, 1);

    label_31 = new QLabel(tab_chat);
    label_31->setObjectName(QString::fromUtf8("label_31"));
    label_31->setAlignment(Qt::AlignRight|Qt::AlignTop|Qt::AlignTrailing);

    gridLayout7->addWidget(label_31, 2, 0, 1, 1);

    vboxLayout7 = new QVBoxLayout();
    vboxLayout7->setObjectName(QString::fromUtf8("vboxLayout7"));
    fChatTimestampLine = new QCheckBox(tab_chat);
    fChatTimestampLine->setObjectName(QString::fromUtf8("fChatTimestampLine"));

    vboxLayout7->addWidget(fChatTimestampLine);

    hboxLayout13 = new QHBoxLayout();
    hboxLayout13->setObjectName(QString::fromUtf8("hboxLayout13"));
    spacerItem23 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    hboxLayout13->addItem(spacerItem23);

    label_30 = new QLabel(tab_chat);
    label_30->setObjectName(QString::fromUtf8("label_30"));
    label_30->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout13->addWidget(label_30);

    fChatTimeColor = new WidgetColorButton(tab_chat);
    fChatTimeColor->setObjectName(QString::fromUtf8("fChatTimeColor"));

    hboxLayout13->addWidget(fChatTimeColor);

    spacerItem24 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout13->addItem(spacerItem24);


    vboxLayout7->addLayout(hboxLayout13);

    spacerItem25 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

    vboxLayout7->addItem(spacerItem25);

    vboxLayout8 = new QVBoxLayout();
    vboxLayout8->setSpacing(0);
    vboxLayout8->setObjectName(QString::fromUtf8("vboxLayout8"));
    fChatEmoticons = new QCheckBox(tab_chat);
    fChatEmoticons->setObjectName(QString::fromUtf8("fChatEmoticons"));

    vboxLayout8->addWidget(fChatEmoticons);

    hboxLayout14 = new QHBoxLayout();
    hboxLayout14->setSpacing(5);
    hboxLayout14->setObjectName(QString::fromUtf8("hboxLayout14"));
    spacerItem26 = new QSpacerItem(20, 16, QSizePolicy::Fixed, QSizePolicy::Minimum);

    hboxLayout14->addItem(spacerItem26);

    label_32 = new QLabel(tab_chat);
    label_32->setObjectName(QString::fromUtf8("label_32"));
    label_32->setPixmap(QPixmap(QString::fromUtf8(":/icons/emotes/face-grin.png")));

    hboxLayout14->addWidget(label_32);

    label_33 = new QLabel(tab_chat);
    label_33->setObjectName(QString::fromUtf8("label_33"));
    label_33->setPixmap(QPixmap(QString::fromUtf8(":/icons/emotes/face-wink.png")));

    hboxLayout14->addWidget(label_33);

    label_34 = new QLabel(tab_chat);
    label_34->setObjectName(QString::fromUtf8("label_34"));
    label_34->setPixmap(QPixmap(QString::fromUtf8(":/icons/emotes/face-cat.png")));

    hboxLayout14->addWidget(label_34);

    spacerItem27 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout14->addItem(spacerItem27);


    vboxLayout8->addLayout(hboxLayout14);


    vboxLayout7->addLayout(vboxLayout8);


    gridLayout7->addLayout(vboxLayout7, 2, 1, 1, 1);

    spacerItem28 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

    gridLayout7->addItem(spacerItem28, 1, 1, 1, 1);


    gridLayout6->addLayout(gridLayout7, 1, 1, 1, 2);

    spacerItem29 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout6->addItem(spacerItem29, 1, 3, 1, 1);

    spacerItem30 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout6->addItem(spacerItem30, 2, 1, 1, 1);

    tabWidget->addTab(tab_chat, QString());
    Seite = new QWidget();
    Seite->setObjectName(QString::fromUtf8("Seite"));
    hboxLayout15 = new QHBoxLayout(Seite);
    hboxLayout15->setObjectName(QString::fromUtf8("hboxLayout15"));
    fEventsList = new QListWidget(Seite);
    fEventsList->setObjectName(QString::fromUtf8("fEventsList"));
    fEventsList->setMaximumSize(QSize(200, 16777215));
    fEventsList->setIconSize(QSize(20, 20));

    hboxLayout15->addWidget(fEventsList);

    vboxLayout9 = new QVBoxLayout();
    vboxLayout9->setObjectName(QString::fromUtf8("vboxLayout9"));
    spacerItem31 = new QSpacerItem(20, 14, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout9->addItem(spacerItem31);

    hboxLayout16 = new QHBoxLayout();
    hboxLayout16->setObjectName(QString::fromUtf8("hboxLayout16"));
    label_35 = new QLabel(Seite);
    label_35->setObjectName(QString::fromUtf8("label_35"));

    hboxLayout16->addWidget(label_35);

    fEventsSound = new QToolButton(Seite);
    fEventsSound->setObjectName(QString::fromUtf8("fEventsSound"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(fEventsSound->sizePolicy().hasHeightForWidth());
    fEventsSound->setSizePolicy(sizePolicy4);
    fEventsSound->setMaximumSize(QSize(350, 16777215));

    hboxLayout16->addWidget(fEventsSound);


    vboxLayout9->addLayout(hboxLayout16);

    fEventsDisplayTrayMsg = new QCheckBox(Seite);
    fEventsDisplayTrayMsg->setObjectName(QString::fromUtf8("fEventsDisplayTrayMsg"));

    vboxLayout9->addWidget(fEventsDisplayTrayMsg);

    fEventsPostChat = new QCheckBox(Seite);
    fEventsPostChat->setObjectName(QString::fromUtf8("fEventsPostChat"));

    vboxLayout9->addWidget(fEventsPostChat);

    vboxLayout10 = new QVBoxLayout();
    vboxLayout10->setSpacing(0);
    vboxLayout10->setObjectName(QString::fromUtf8("vboxLayout10"));
    fEventsSysCmdEnabled = new QCheckBox(Seite);
    fEventsSysCmdEnabled->setObjectName(QString::fromUtf8("fEventsSysCmdEnabled"));

    vboxLayout10->addWidget(fEventsSysCmdEnabled);

    fEventsSysCmd = new QLineEdit(Seite);
    fEventsSysCmd->setObjectName(QString::fromUtf8("fEventsSysCmd"));

    vboxLayout10->addWidget(fEventsSysCmd);


    vboxLayout9->addLayout(vboxLayout10);

    spacerItem32 = new QSpacerItem(191, 31, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout9->addItem(spacerItem32);


    hboxLayout15->addLayout(vboxLayout9);

    tabWidget->addTab(Seite, QString());
    tab_files = new QWidget();
    tab_files->setObjectName(QString::fromUtf8("tab_files"));
    gridLayout8 = new QGridLayout(tab_files);
    gridLayout8->setObjectName(QString::fromUtf8("gridLayout8"));
    label_4 = new QLabel(tab_files);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout8->addWidget(label_4, 0, 0, 1, 1);

    fFilesFolder = new QLineEdit(tab_files);
    fFilesFolder->setObjectName(QString::fromUtf8("fFilesFolder"));
    fFilesFolder->setReadOnly(true);

    gridLayout8->addWidget(fFilesFolder, 0, 1, 1, 1);

    btnFilesFolderSet = new QToolButton(tab_files);
    btnFilesFolderSet->setObjectName(QString::fromUtf8("btnFilesFolderSet"));
    btnFilesFolderSet->setFont(font);

    gridLayout8->addWidget(btnFilesFolderSet, 0, 2, 1, 1);

    label_5 = new QLabel(tab_files);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout8->addWidget(label_5, 1, 0, 1, 1);

    fFilesQueue = new QCheckBox(tab_files);
    fFilesQueue->setObjectName(QString::fromUtf8("fFilesQueue"));

    gridLayout8->addWidget(fFilesQueue, 1, 1, 1, 2);

    fFilesEncrypt = new QCheckBox(tab_files);
    fFilesEncrypt->setObjectName(QString::fromUtf8("fFilesEncrypt"));

    gridLayout8->addWidget(fFilesEncrypt, 2, 1, 1, 1);

    spacerItem33 = new QSpacerItem(452, 31, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout8->addItem(spacerItem33, 4, 0, 1, 2);

    fFilesOpenInNewWindow = new QCheckBox(tab_files);
    fFilesOpenInNewWindow->setObjectName(QString::fromUtf8("fFilesOpenInNewWindow"));

    gridLayout8->addWidget(fFilesOpenInNewWindow, 3, 1, 1, 1);

    tabWidget->addTab(tab_files, QString());
    tab_trackers = new QWidget();
    tab_trackers->setObjectName(QString::fromUtf8("tab_trackers"));
    hboxLayout17 = new QHBoxLayout(tab_trackers);
    hboxLayout17->setObjectName(QString::fromUtf8("hboxLayout17"));
    vboxLayout11 = new QVBoxLayout();
    vboxLayout11->setObjectName(QString::fromUtf8("vboxLayout11"));
    fTrackerList = new QListWidget(tab_trackers);
    fTrackerList->setObjectName(QString::fromUtf8("fTrackerList"));
    QSizePolicy sizePolicy5(QSizePolicy::Ignored, QSizePolicy::Expanding);
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(fTrackerList->sizePolicy().hasHeightForWidth());
    fTrackerList->setSizePolicy(sizePolicy5);

    vboxLayout11->addWidget(fTrackerList);

    hboxLayout18 = new QHBoxLayout();
    hboxLayout18->setObjectName(QString::fromUtf8("hboxLayout18"));
    fBtnTrackerNew = new QToolButton(tab_trackers);
    fBtnTrackerNew->setObjectName(QString::fromUtf8("fBtnTrackerNew"));

    hboxLayout18->addWidget(fBtnTrackerNew);

    fBtnTrackerDelete = new QToolButton(tab_trackers);
    fBtnTrackerDelete->setObjectName(QString::fromUtf8("fBtnTrackerDelete"));
    fBtnTrackerDelete->setEnabled(false);

    hboxLayout18->addWidget(fBtnTrackerDelete);

    spacerItem34 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

    hboxLayout18->addItem(spacerItem34);


    vboxLayout11->addLayout(hboxLayout18);


    hboxLayout17->addLayout(vboxLayout11);

    fTrackerGroup = new QGroupBox(tab_trackers);
    fTrackerGroup->setObjectName(QString::fromUtf8("fTrackerGroup"));
    gridLayout9 = new QGridLayout(fTrackerGroup);
    gridLayout9->setObjectName(QString::fromUtf8("gridLayout9"));
    label_26 = new QLabel(fTrackerGroup);
    label_26->setObjectName(QString::fromUtf8("label_26"));
    label_26->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout9->addWidget(label_26, 0, 0, 1, 1);

    fTrackerName = new QLineEdit(fTrackerGroup);
    fTrackerName->setObjectName(QString::fromUtf8("fTrackerName"));

    gridLayout9->addWidget(fTrackerName, 0, 1, 1, 1);

    label_27 = new QLabel(fTrackerGroup);
    label_27->setObjectName(QString::fromUtf8("label_27"));
    label_27->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout9->addWidget(label_27, 1, 0, 1, 1);

    fTrackerAddress = new QLineEdit(fTrackerGroup);
    fTrackerAddress->setObjectName(QString::fromUtf8("fTrackerAddress"));

    gridLayout9->addWidget(fTrackerAddress, 1, 1, 1, 1);

    spacerItem35 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout9->addItem(spacerItem35, 2, 1, 1, 1);


    hboxLayout17->addWidget(fTrackerGroup);

    tabWidget->addTab(tab_trackers, QString());

    vboxLayout->addWidget(tabWidget);

    hboxLayout19 = new QHBoxLayout();
    hboxLayout19->setObjectName(QString::fromUtf8("hboxLayout19"));
    spacerItem36 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout19->addItem(spacerItem36);

    fBtnOk = new QPushButton(WidgetPrefs);
    fBtnOk->setObjectName(QString::fromUtf8("fBtnOk"));
    fBtnOk->setAutoDefault(true);
    fBtnOk->setDefault(true);

    hboxLayout19->addWidget(fBtnOk);


    vboxLayout->addLayout(hboxLayout19);

    label->setBuddy(fNickname);
    label_2->setBuddy(fStatus);
    label_4->setBuddy(fNickname);

    retranslateUi(WidgetPrefs);

    tabWidget->setCurrentIndex(2);
    tabWidget_2->setCurrentIndex(5);


    QMetaObject::connectSlotsByName(WidgetPrefs);
    } // setupUi

    void retranslateUi(QWidget *WidgetPrefs)
    {
    WidgetPrefs->setWindowTitle(QApplication::translate("WidgetPrefs", "Preferences", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WidgetPrefs", "Nickname:", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WidgetPrefs", "Status:", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("WidgetPrefs", "Icon:", 0, QApplication::UnicodeUTF8));
    fIcon->setText(QString());
    label_7->setText(QApplication::translate("WidgetPrefs", "Drag&Drop any Image\n"
"max size 32x32 pixels", 0, QApplication::UnicodeUTF8));
    btnGeneralDefaultIcon->setText(QApplication::translate("WidgetPrefs", "Default", 0, QApplication::UnicodeUTF8));
    btnGeneralSetIcon->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    label_28->setText(QApplication::translate("WidgetPrefs", "Application Language:\n"
"(requires restart)", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_general), QApplication::translate("WidgetPrefs", "General", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("WidgetPrefs", "Text:", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("WidgetPrefs", "URLs:", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("WidgetPrefs", "Chat Font:", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("WidgetPrefs", "User List Font:", 0, QApplication::UnicodeUTF8));
    label_17->setText(QApplication::translate("WidgetPrefs", "Icon Size:", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("WidgetPrefs", "Background:", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("WidgetPrefs", "Events:", 0, QApplication::UnicodeUTF8));
    fBtnIfUserlistCompactLarge->setText(QApplication::translate("WidgetPrefs", "Large", 0, QApplication::UnicodeUTF8));
    fBtnIfUserlistCompactSmall->setText(QApplication::translate("WidgetPrefs", "Small", 0, QApplication::UnicodeUTF8));
    fBtnIfUserlistAlternateRow->setText(QApplication::translate("WidgetPrefs", "Alternating Row Background", 0, QApplication::UnicodeUTF8));
    fBtnIfChatFont->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    fBtnIfUserlistFont->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(fTabUiChat), QApplication::translate("WidgetPrefs", "Chat", 0, QApplication::UnicodeUTF8));
    label_22->setText(QApplication::translate("WidgetPrefs", "Text:", 0, QApplication::UnicodeUTF8));
    label_24->setText(QApplication::translate("WidgetPrefs", "Titles:", 0, QApplication::UnicodeUTF8));
    label_25->setText(QApplication::translate("WidgetPrefs", "News Font:", 0, QApplication::UnicodeUTF8));
    fBtnIfNewsText->setText(QString());
    label_23->setText(QApplication::translate("WidgetPrefs", "Background:", 0, QApplication::UnicodeUTF8));
    fBtnIfNewsBack->setText(QString());
    fBtnIfNewsTitles->setText(QString());
    fBtnIfNewsFont->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(fTabUiNews), QApplication::translate("WidgetPrefs", "News", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(fTabUiMessages), QApplication::translate("WidgetPrefs", "Messages", 0, QApplication::UnicodeUTF8));
    label_21->setText(QApplication::translate("WidgetPrefs", "Files Font:", 0, QApplication::UnicodeUTF8));
    fBtnIfFilesAlternateRow->setText(QApplication::translate("WidgetPrefs", "Use Alternating Row Background", 0, QApplication::UnicodeUTF8));
    fBtnIfFilesFont->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(fTabUiFiles), QApplication::translate("WidgetPrefs", "Files", 0, QApplication::UnicodeUTF8));
    label_18->setText(QApplication::translate("WidgetPrefs", "Text:", 0, QApplication::UnicodeUTF8));
    label_20->setText(QApplication::translate("WidgetPrefs", "Preview Font:", 0, QApplication::UnicodeUTF8));
    toolButton_5->setText(QString());
    label_19->setText(QApplication::translate("WidgetPrefs", "Background:", 0, QApplication::UnicodeUTF8));
    toolButton_6->setText(QString());
    toolButton_14->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(fTabUiPreview), QApplication::translate("WidgetPrefs", "Preview", 0, QApplication::UnicodeUTF8));
    checkBox_2->setText(QApplication::translate("WidgetPrefs", "Use Alternating Row Background", 0, QApplication::UnicodeUTF8));
    tabWidget_2->setTabText(tabWidget_2->indexOf(fTabUiTrackers), QApplication::translate("WidgetPrefs", "Trackers", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_interface), QApplication::translate("WidgetPrefs", "Interface", 0, QApplication::UnicodeUTF8));
    btnBMAdd->setToolTip(QApplication::translate("WidgetPrefs", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Create a new bookmark.</p></body></html>", 0, QApplication::UnicodeUTF8));
    btnBMAdd->setText(QApplication::translate("WidgetPrefs", "New", 0, QApplication::UnicodeUTF8));
    btnBMRemove->setToolTip(QApplication::translate("WidgetPrefs", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Delete the selected bookmark.</p></body></html>", 0, QApplication::UnicodeUTF8));
    btnBMRemove->setText(QApplication::translate("WidgetPrefs", "Delete", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("WidgetPrefs", "Name:", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("WidgetPrefs", "Address:", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("WidgetPrefs", "Login:", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("WidgetPrefs", "Password:", 0, QApplication::UnicodeUTF8));
    fBMAutoConnect->setText(QApplication::translate("WidgetPrefs", "Auto-Connect at Startup", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_bookmarks), QApplication::translate("WidgetPrefs", "Bookmarks", 0, QApplication::UnicodeUTF8));
    label_29->setText(QApplication::translate("WidgetPrefs", "Style:", 0, QApplication::UnicodeUTF8));
    fChatStyleQwired->setText(QApplication::translate("WidgetPrefs", "Qwired", 0, QApplication::UnicodeUTF8));
    fChatStyleIRC->setText(QApplication::translate("WidgetPrefs", "IRC", 0, QApplication::UnicodeUTF8));
    label_31->setText(QApplication::translate("WidgetPrefs", "Settings:", 0, QApplication::UnicodeUTF8));
    fChatTimestampLine->setText(QApplication::translate("WidgetPrefs", "Timestamp Every Line", 0, QApplication::UnicodeUTF8));
    label_30->setText(QApplication::translate("WidgetPrefs", "Color:", 0, QApplication::UnicodeUTF8));
    fChatTimeColor->setText(QString());
    fChatEmoticons->setText(QApplication::translate("WidgetPrefs", "Show Emoticons", 0, QApplication::UnicodeUTF8));
    label_32->setText(QString());
    label_33->setText(QString());
    label_34->setText(QString());
    tabWidget->setTabText(tabWidget->indexOf(tab_chat), QApplication::translate("WidgetPrefs", "Chat", 0, QApplication::UnicodeUTF8));
    label_35->setText(QApplication::translate("WidgetPrefs", "Play Sound:", 0, QApplication::UnicodeUTF8));
    fEventsSound->setText(QApplication::translate("WidgetPrefs", "...", 0, QApplication::UnicodeUTF8));
    fEventsDisplayTrayMsg->setText(QApplication::translate("WidgetPrefs", "Display System Tray Message (requires Growl on Mac OS X)", 0, QApplication::UnicodeUTF8));
    fEventsPostChat->setText(QApplication::translate("WidgetPrefs", "Post Event in Chat", 0, QApplication::UnicodeUTF8));
    fEventsSysCmdEnabled->setText(QApplication::translate("WidgetPrefs", "Execute System Command:", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(Seite), QApplication::translate("WidgetPrefs", "Events", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("WidgetPrefs", "Download Folder:", 0, QApplication::UnicodeUTF8));
    btnFilesFolderSet->setText(QApplication::translate("WidgetPrefs", "Set...", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("WidgetPrefs", "Transfers:", 0, QApplication::UnicodeUTF8));
    fFilesQueue->setText(QApplication::translate("WidgetPrefs", "Queue Transfers", 0, QApplication::UnicodeUTF8));
    fFilesEncrypt->setText(QApplication::translate("WidgetPrefs", "Encrypt Transfers (slower on local networks)", 0, QApplication::UnicodeUTF8));
    fFilesOpenInNewWindow->setText(QApplication::translate("WidgetPrefs", "Always Open Folders In New Windows", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_files), QApplication::translate("WidgetPrefs", "Files", 0, QApplication::UnicodeUTF8));
    fBtnTrackerNew->setText(QApplication::translate("WidgetPrefs", "New", 0, QApplication::UnicodeUTF8));
    fBtnTrackerDelete->setText(QApplication::translate("WidgetPrefs", "Delete", 0, QApplication::UnicodeUTF8));
    fTrackerGroup->setTitle(QApplication::translate("WidgetPrefs", "Tracker Settings", 0, QApplication::UnicodeUTF8));
    label_26->setText(QApplication::translate("WidgetPrefs", "Tracker Name:", 0, QApplication::UnicodeUTF8));
    label_27->setText(QApplication::translate("WidgetPrefs", "Tracker Address:", 0, QApplication::UnicodeUTF8));
    tabWidget->setTabText(tabWidget->indexOf(tab_trackers), QApplication::translate("WidgetPrefs", "Trackers", 0, QApplication::UnicodeUTF8));
    fBtnOk->setText(QApplication::translate("WidgetPrefs", "Apply Changes", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetPrefs);
    } // retranslateUi

};

namespace Ui {
    class WidgetPrefs: public Ui_WidgetPrefs {};
} // namespace Ui

#endif // UI_WIDGETPREFS_H
