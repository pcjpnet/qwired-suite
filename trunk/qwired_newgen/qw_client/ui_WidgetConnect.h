/********************************************************************************
** Form generated from reading ui file 'WidgetConnect.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETCONNECT_H
#define UI_WIDGETCONNECT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

class Ui_WidgetConnect
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QSpacerItem *spacerItem1;
    QGridLayout *gridLayout1;
    QToolButton *fBtnFavourites;
    QSpacerItem *spacerItem2;
    QLabel *label;
    QLineEdit *fAddress;
    QLabel *label_2;
    QLineEdit *fLogin;
    QLabel *label_3;
    QLineEdit *fPassword;
    QHBoxLayout *hboxLayout;
    QProgressBar *fProgress;
    QSpacerItem *spacerItem3;
    QPushButton *btnConnect;
    QSpacerItem *spacerItem4;
    QSpacerItem *spacerItem5;

    void setupUi(QWidget *WidgetConnect)
    {
    if (WidgetConnect->objectName().isEmpty())
        WidgetConnect->setObjectName(QString::fromUtf8("WidgetConnect"));
    WidgetConnect->resize(498, 260);
    gridLayout = new QGridLayout(WidgetConnect);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    spacerItem = new QSpacerItem(20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem, 0, 1, 1, 1);

    spacerItem1 = new QSpacerItem(16, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem1, 1, 0, 1, 1);

    gridLayout1 = new QGridLayout();
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    fBtnFavourites = new QToolButton(WidgetConnect);
    fBtnFavourites->setObjectName(QString::fromUtf8("fBtnFavourites"));
    fBtnFavourites->setIcon(QIcon(QString::fromUtf8(":/icons/icon_favourite.png")));
    fBtnFavourites->setIconSize(QSize(20, 20));
    fBtnFavourites->setPopupMode(QToolButton::InstantPopup);
    fBtnFavourites->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    gridLayout1->addWidget(fBtnFavourites, 0, 1, 1, 1);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout1->addItem(spacerItem2, 0, 2, 1, 1);

    label = new QLabel(WidgetConnect);
    label->setObjectName(QString::fromUtf8("label"));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label, 1, 0, 1, 1);

    fAddress = new QLineEdit(WidgetConnect);
    fAddress->setObjectName(QString::fromUtf8("fAddress"));
    fAddress->setMinimumSize(QSize(300, 0));

    gridLayout1->addWidget(fAddress, 1, 1, 1, 2);

    label_2 = new QLabel(WidgetConnect);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_2, 2, 0, 1, 1);

    fLogin = new QLineEdit(WidgetConnect);
    fLogin->setObjectName(QString::fromUtf8("fLogin"));
    fLogin->setMinimumSize(QSize(300, 0));

    gridLayout1->addWidget(fLogin, 2, 1, 1, 2);

    label_3 = new QLabel(WidgetConnect);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout1->addWidget(label_3, 3, 0, 1, 1);

    fPassword = new QLineEdit(WidgetConnect);
    fPassword->setObjectName(QString::fromUtf8("fPassword"));
    fPassword->setMinimumSize(QSize(300, 0));
    fPassword->setEchoMode(QLineEdit::Password);

    gridLayout1->addWidget(fPassword, 3, 1, 1, 2);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fProgress = new QProgressBar(WidgetConnect);
    fProgress->setObjectName(QString::fromUtf8("fProgress"));
    fProgress->setMaximumSize(QSize(100, 16777215));
    fProgress->setMaximum(0);
    fProgress->setValue(-1);
    fProgress->setAlignment(Qt::AlignHCenter);

    hboxLayout->addWidget(fProgress);

    spacerItem3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem3);

    btnConnect = new QPushButton(WidgetConnect);
    btnConnect->setObjectName(QString::fromUtf8("btnConnect"));
    btnConnect->setAutoDefault(true);
    btnConnect->setDefault(true);

    hboxLayout->addWidget(btnConnect);


    gridLayout1->addLayout(hboxLayout, 4, 1, 1, 2);


    gridLayout->addLayout(gridLayout1, 1, 1, 1, 2);

    spacerItem4 = new QSpacerItem(21, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout->addItem(spacerItem4, 1, 3, 1, 1);

    spacerItem5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout->addItem(spacerItem5, 2, 2, 1, 1);

    label->setBuddy(fAddress);
    label_2->setBuddy(fLogin);
    label_3->setBuddy(fPassword);

    retranslateUi(WidgetConnect);
    QObject::connect(fAddress, SIGNAL(returnPressed()), btnConnect, SLOT(click()));
    QObject::connect(fLogin, SIGNAL(returnPressed()), btnConnect, SLOT(click()));
    QObject::connect(fPassword, SIGNAL(returnPressed()), btnConnect, SLOT(click()));

    QMetaObject::connectSlotsByName(WidgetConnect);
    } // setupUi

    void retranslateUi(QWidget *WidgetConnect)
    {
    WidgetConnect->setWindowTitle(QApplication::translate("WidgetConnect", "Connect...", 0, QApplication::UnicodeUTF8));
    fBtnFavourites->setText(QApplication::translate("WidgetConnect", "Bookmarks", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WidgetConnect", "Address:", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WidgetConnect", "Login:", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("WidgetConnect", "Pasword:", 0, QApplication::UnicodeUTF8));
    btnConnect->setText(QApplication::translate("WidgetConnect", "Connect", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetConnect);
    } // retranslateUi

};

namespace Ui {
    class WidgetConnect: public Ui_WidgetConnect {};
} // namespace Ui

#endif // UI_WIDGETCONNECT_H
