/********************************************************************************
** Form generated from reading ui file 'WidgetSendPrivMsg.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETSENDPRIVMSG_H
#define UI_WIDGETSENDPRIVMSG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetSendPrivMsg
{
public:
    QHBoxLayout *hboxLayout;
    QSplitter *fSplitterV;
    QWidget *layoutWidget;
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout1;
    QToolButton *fBtnSave;
    QToolButton *fBtnDelete;
    QSpacerItem *spacerItem;
    QListWidget *fUsers;
    QSplitter *fSplitterH;
    QTextEdit *fMsg;
    QTextEdit *fInput;

    void setupUi(QWidget *WidgetSendPrivMsg)
    {
    if (WidgetSendPrivMsg->objectName().isEmpty())
        WidgetSendPrivMsg->setObjectName(QString::fromUtf8("WidgetSendPrivMsg"));
    WidgetSendPrivMsg->resize(498, 393);
    hboxLayout = new QHBoxLayout(WidgetSendPrivMsg);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fSplitterV = new QSplitter(WidgetSendPrivMsg);
    fSplitterV->setObjectName(QString::fromUtf8("fSplitterV"));
    fSplitterV->setOrientation(Qt::Horizontal);
    layoutWidget = new QWidget(fSplitterV);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    vboxLayout = new QVBoxLayout(layoutWidget);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    fBtnSave = new QToolButton(layoutWidget);
    fBtnSave->setObjectName(QString::fromUtf8("fBtnSave"));
    fBtnSave->setEnabled(false);
    fBtnSave->setIcon(QIcon(QString::fromUtf8(":/icons/icn_download.png")));
    fBtnSave->setIconSize(QSize(18, 18));
    fBtnSave->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnSave);

    fBtnDelete = new QToolButton(layoutWidget);
    fBtnDelete->setObjectName(QString::fromUtf8("fBtnDelete"));
    fBtnDelete->setEnabled(false);
    fBtnDelete->setIcon(QIcon(QString::fromUtf8(":/icons/icn_delete.png")));
    fBtnDelete->setIconSize(QSize(18, 18));
    fBtnDelete->setAutoRaise(true);

    hboxLayout1->addWidget(fBtnDelete);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);


    vboxLayout->addLayout(hboxLayout1);

    fUsers = new QListWidget(layoutWidget);
    fUsers->setObjectName(QString::fromUtf8("fUsers"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(1);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(fUsers->sizePolicy().hasHeightForWidth());
    fUsers->setSizePolicy(sizePolicy);
    fUsers->setAlternatingRowColors(true);
    fUsers->setIconSize(QSize(32, 32));
    fUsers->setSpacing(1);

    vboxLayout->addWidget(fUsers);

    fSplitterV->addWidget(layoutWidget);
    fSplitterH = new QSplitter(fSplitterV);
    fSplitterH->setObjectName(QString::fromUtf8("fSplitterH"));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fSplitterH->sizePolicy().hasHeightForWidth());
    fSplitterH->setSizePolicy(sizePolicy1);
    fSplitterH->setOrientation(Qt::Vertical);
    fMsg = new QTextEdit(fSplitterH);
    fMsg->setObjectName(QString::fromUtf8("fMsg"));
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy2.setHorizontalStretch(4);
    sizePolicy2.setVerticalStretch(4);
    sizePolicy2.setHeightForWidth(fMsg->sizePolicy().hasHeightForWidth());
    fMsg->setSizePolicy(sizePolicy2);
    fMsg->setTabChangesFocus(true);
    fMsg->setReadOnly(true);
    fSplitterH->addWidget(fMsg);
    fInput = new QTextEdit(fSplitterH);
    fInput->setObjectName(QString::fromUtf8("fInput"));
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(1);
    sizePolicy3.setHeightForWidth(fInput->sizePolicy().hasHeightForWidth());
    fInput->setSizePolicy(sizePolicy3);
    fInput->setTabChangesFocus(true);
    fInput->setAcceptRichText(false);
    fSplitterH->addWidget(fInput);
    fSplitterV->addWidget(fSplitterH);

    hboxLayout->addWidget(fSplitterV);

    QWidget::setTabOrder(fUsers, fMsg);

    retranslateUi(WidgetSendPrivMsg);

    QMetaObject::connectSlotsByName(WidgetSendPrivMsg);
    } // setupUi

    void retranslateUi(QWidget *WidgetSendPrivMsg)
    {
    WidgetSendPrivMsg->setWindowTitle(QApplication::translate("WidgetSendPrivMsg", "Private Messages", 0, QApplication::UnicodeUTF8));
    fBtnSave->setToolTip(QApplication::translate("WidgetSendPrivMsg", "Save Message History", 0, QApplication::UnicodeUTF8));
    fBtnSave->setText(QString());
    fBtnDelete->setToolTip(QApplication::translate("WidgetSendPrivMsg", "Delete Message History", 0, QApplication::UnicodeUTF8));
    fBtnDelete->setText(QString());
    Q_UNUSED(WidgetSendPrivMsg);
    } // retranslateUi

};

namespace Ui {
    class WidgetSendPrivMsg: public Ui_WidgetSendPrivMsg {};
} // namespace Ui

#endif // UI_WIDGETSENDPRIVMSG_H
