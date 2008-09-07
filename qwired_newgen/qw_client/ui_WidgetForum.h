/********************************************************************************
** Form generated from reading ui file 'WidgetForum.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETFORUM_H
#define UI_WIDGETFORUM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTextBrowser>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetForum
{
public:
    QHBoxLayout *hboxLayout;
    QSplitter *fSplitterV;
    QWidget *widget;
    QVBoxLayout *vboxLayout;
    QLabel *fTopic;
    QSplitter *fSplitterH;
    QTextBrowser *fChatLog;
    QTextEdit *fChatInput;
    QWidget *layoutWidget;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout1;
    QToolButton *fBtnMsg;
    QToolButton *fBtnChat;
    QToolButton *fBtnInfo;
    QSpacerItem *spacerItem;
    QToolButton *fBtnBan;
    QToolButton *fBtnKick;
    QToolButton *fBtnInvite;
    QListView *fUsers;

    void setupUi(QWidget *WidgetForum)
    {
    if (WidgetForum->objectName().isEmpty())
        WidgetForum->setObjectName(QString::fromUtf8("WidgetForum"));
    WidgetForum->resize(601, 525);
    hboxLayout = new QHBoxLayout(WidgetForum);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fSplitterV = new QSplitter(WidgetForum);
    fSplitterV->setObjectName(QString::fromUtf8("fSplitterV"));
    fSplitterV->setOrientation(Qt::Horizontal);
    widget = new QWidget(fSplitterV);
    widget->setObjectName(QString::fromUtf8("widget"));
    vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    fTopic = new QLabel(widget);
    fTopic->setObjectName(QString::fromUtf8("fTopic"));
    QFont font;
    font.setPointSize(11);
    fTopic->setFont(font);

    vboxLayout->addWidget(fTopic);

    fSplitterH = new QSplitter(widget);
    fSplitterH->setObjectName(QString::fromUtf8("fSplitterH"));
    fSplitterH->setOrientation(Qt::Vertical);
    fChatLog = new QTextBrowser(fSplitterH);
    fChatLog->setObjectName(QString::fromUtf8("fChatLog"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(10);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(fChatLog->sizePolicy().hasHeightForWidth());
    fChatLog->setSizePolicy(sizePolicy);
    fChatLog->setOpenExternalLinks(true);
    fSplitterH->addWidget(fChatLog);
    fChatInput = new QTextEdit(fSplitterH);
    fChatInput->setObjectName(QString::fromUtf8("fChatInput"));
    fChatInput->setAcceptRichText(false);
    fSplitterH->addWidget(fChatInput);

    vboxLayout->addWidget(fSplitterH);

    fSplitterV->addWidget(widget);
    layoutWidget = new QWidget(fSplitterV);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    vboxLayout1 = new QVBoxLayout(layoutWidget);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    vboxLayout1->setContentsMargins(0, 0, 0, 0);
    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    fBtnMsg = new QToolButton(layoutWidget);
    fBtnMsg->setObjectName(QString::fromUtf8("fBtnMsg"));
    fBtnMsg->setEnabled(false);
    fBtnMsg->setIcon(QIcon(QString::fromUtf8(":/icons/icn_msg.png")));
    fBtnMsg->setIconSize(QSize(16, 16));
    fBtnMsg->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnMsg);

    fBtnChat = new QToolButton(layoutWidget);
    fBtnChat->setObjectName(QString::fromUtf8("fBtnChat"));
    fBtnChat->setEnabled(false);
    fBtnChat->setIcon(QIcon(QString::fromUtf8(":/icons/btn_chat.png")));
    fBtnChat->setIconSize(QSize(16, 16));
    fBtnChat->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnChat);

    fBtnInfo = new QToolButton(layoutWidget);
    fBtnInfo->setObjectName(QString::fromUtf8("fBtnInfo"));
    fBtnInfo->setEnabled(false);
    fBtnInfo->setIcon(QIcon(QString::fromUtf8(":/icons/btn_info.png")));
    fBtnInfo->setIconSize(QSize(16, 16));
    fBtnInfo->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnInfo);

    spacerItem = new QSpacerItem(16, 30, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    fBtnBan = new QToolButton(layoutWidget);
    fBtnBan->setObjectName(QString::fromUtf8("fBtnBan"));
    fBtnBan->setEnabled(false);
    fBtnBan->setIcon(QIcon(QString::fromUtf8(":/icons/btn_ban.png")));
    fBtnBan->setIconSize(QSize(16, 16));
    fBtnBan->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnBan);

    fBtnKick = new QToolButton(layoutWidget);
    fBtnKick->setObjectName(QString::fromUtf8("fBtnKick"));
    fBtnKick->setEnabled(false);
    fBtnKick->setIcon(QIcon(QString::fromUtf8(":/icons/btn_kick.png")));
    fBtnKick->setIconSize(QSize(16, 16));
    fBtnKick->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnKick);

    fBtnInvite = new QToolButton(layoutWidget);
    fBtnInvite->setObjectName(QString::fromUtf8("fBtnInvite"));
    fBtnInvite->setIcon(QIcon(QString::fromUtf8(":/icons/icn_invite.png")));
    fBtnInvite->setIconSize(QSize(16, 16));
    fBtnInvite->setPopupMode(QToolButton::InstantPopup);

    hboxLayout1->addWidget(fBtnInvite);


    vboxLayout1->addLayout(hboxLayout1);

    fUsers = new QListView(layoutWidget);
    fUsers->setObjectName(QString::fromUtf8("fUsers"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(1);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fUsers->sizePolicy().hasHeightForWidth());
    fUsers->setSizePolicy(sizePolicy1);
    fUsers->setMinimumSize(QSize(180, 0));

    vboxLayout1->addWidget(fUsers);

    fSplitterV->addWidget(layoutWidget);

    hboxLayout->addWidget(fSplitterV);


    retranslateUi(WidgetForum);

    QMetaObject::connectSlotsByName(WidgetForum);
    } // setupUi

    void retranslateUi(QWidget *WidgetForum)
    {
    WidgetForum->setWindowTitle(QApplication::translate("WidgetForum", "Qwired", 0, QApplication::UnicodeUTF8));
    fTopic->setText(QApplication::translate("WidgetForum", "Topic: (none)", 0, QApplication::UnicodeUTF8));
    fBtnMsg->setToolTip(QApplication::translate("WidgetForum", "Send a private message", 0, QApplication::UnicodeUTF8));
    fBtnChat->setToolTip(QApplication::translate("WidgetForum", "Start a private chat", 0, QApplication::UnicodeUTF8));
    fBtnInfo->setToolTip(QApplication::translate("WidgetForum", "Show information about user", 0, QApplication::UnicodeUTF8));
    fBtnBan->setToolTip(QApplication::translate("WidgetForum", "Ban from server", 0, QApplication::UnicodeUTF8));
    fBtnKick->setToolTip(QApplication::translate("WidgetForum", "Kick from server", 0, QApplication::UnicodeUTF8));
    fBtnKick->setText(QString());
    fBtnInvite->setToolTip(QApplication::translate("WidgetForum", "Invite to chat...", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetForum);
    } // retranslateUi

};

namespace Ui {
    class WidgetForum: public Ui_WidgetForum {};
} // namespace Ui

#endif // UI_WIDGETFORUM_H
