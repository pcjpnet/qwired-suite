/********************************************************************************
** Form generated from reading ui file 'WidgetUserInfo.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETUSERINFO_H
#define UI_WIDGETUSERINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetUserInfo
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *fIcon;
    QLabel *fNick;
    QFrame *line;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *fStatus;
    QLabel *label_2;
    QLabel *fLogin;
    QLabel *label_3;
    QLabel *fID;
    QLabel *label_4;
    QLabel *fAddress;
    QLabel *label_5;
    QLabel *fHost;
    QLabel *label_6;
    QLabel *fVersion;
    QLabel *label_7;
    QLabel *fCipher;
    QLabel *label_8;
    QLabel *fLoginTime;
    QLabel *label_9;
    QLabel *fIdleTime;
    QGroupBox *fGroupDownloads;
    QGroupBox *fGroupDownloads_2;
    QGroupBox *fGroupUploads;
    QSpacerItem *spacerItem;

    void setupUi(QWidget *WidgetUserInfo)
    {
    if (WidgetUserInfo->objectName().isEmpty())
        WidgetUserInfo->setObjectName(QString::fromUtf8("WidgetUserInfo"));
    WidgetUserInfo->resize(334, 286);
    vboxLayout = new QVBoxLayout(WidgetUserInfo);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fIcon = new QLabel(WidgetUserInfo);
    fIcon->setObjectName(QString::fromUtf8("fIcon"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(fIcon->sizePolicy().hasHeightForWidth());
    fIcon->setSizePolicy(sizePolicy);
    fIcon->setFrameShape(QFrame::StyledPanel);
    fIcon->setFrameShadow(QFrame::Raised);
    fIcon->setPixmap(QPixmap());
    fIcon->setScaledContents(false);
    fIcon->setMargin(1);

    hboxLayout->addWidget(fIcon);

    fNick = new QLabel(WidgetUserInfo);
    fNick->setObjectName(QString::fromUtf8("fNick"));
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    fNick->setFont(font);
    fNick->setIndent(10);

    hboxLayout->addWidget(fNick);


    vboxLayout->addLayout(hboxLayout);

    line = new QFrame(WidgetUserInfo);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    vboxLayout->addWidget(line);

    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setVerticalSpacing(0);
    gridLayout->setContentsMargins(-1, -1, -1, 6);
    label = new QLabel(WidgetUserInfo);
    label->setObjectName(QString::fromUtf8("label"));
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy1);
    QFont font1;
    font1.setBold(true);
    font1.setWeight(75);
    label->setFont(font1);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    fStatus = new QLabel(WidgetUserInfo);
    fStatus->setObjectName(QString::fromUtf8("fStatus"));
    QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(fStatus->sizePolicy().hasHeightForWidth());
    fStatus->setSizePolicy(sizePolicy2);
    fStatus->setWordWrap(true);

    gridLayout->addWidget(fStatus, 0, 1, 1, 1);

    label_2 = new QLabel(WidgetUserInfo);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    sizePolicy1.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
    label_2->setSizePolicy(sizePolicy1);
    label_2->setFont(font1);
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    fLogin = new QLabel(WidgetUserInfo);
    fLogin->setObjectName(QString::fromUtf8("fLogin"));
    sizePolicy2.setHeightForWidth(fLogin->sizePolicy().hasHeightForWidth());
    fLogin->setSizePolicy(sizePolicy2);
    fLogin->setWordWrap(true);

    gridLayout->addWidget(fLogin, 1, 1, 1, 1);

    label_3 = new QLabel(WidgetUserInfo);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
    label_3->setSizePolicy(sizePolicy1);
    label_3->setFont(font1);
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_3, 2, 0, 1, 1);

    fID = new QLabel(WidgetUserInfo);
    fID->setObjectName(QString::fromUtf8("fID"));
    sizePolicy2.setHeightForWidth(fID->sizePolicy().hasHeightForWidth());
    fID->setSizePolicy(sizePolicy2);
    fID->setWordWrap(true);

    gridLayout->addWidget(fID, 2, 1, 1, 1);

    label_4 = new QLabel(WidgetUserInfo);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    sizePolicy1.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
    label_4->setSizePolicy(sizePolicy1);
    label_4->setFont(font1);
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_4, 3, 0, 1, 1);

    fAddress = new QLabel(WidgetUserInfo);
    fAddress->setObjectName(QString::fromUtf8("fAddress"));
    sizePolicy2.setHeightForWidth(fAddress->sizePolicy().hasHeightForWidth());
    fAddress->setSizePolicy(sizePolicy2);
    fAddress->setWordWrap(true);

    gridLayout->addWidget(fAddress, 3, 1, 1, 1);

    label_5 = new QLabel(WidgetUserInfo);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    sizePolicy1.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
    label_5->setSizePolicy(sizePolicy1);
    label_5->setFont(font1);
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_5, 4, 0, 1, 1);

    fHost = new QLabel(WidgetUserInfo);
    fHost->setObjectName(QString::fromUtf8("fHost"));
    sizePolicy2.setHeightForWidth(fHost->sizePolicy().hasHeightForWidth());
    fHost->setSizePolicy(sizePolicy2);
    fHost->setWordWrap(true);

    gridLayout->addWidget(fHost, 4, 1, 1, 1);

    label_6 = new QLabel(WidgetUserInfo);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    sizePolicy1.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
    label_6->setSizePolicy(sizePolicy1);
    label_6->setFont(font1);
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_6, 5, 0, 1, 1);

    fVersion = new QLabel(WidgetUserInfo);
    fVersion->setObjectName(QString::fromUtf8("fVersion"));
    QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(fVersion->sizePolicy().hasHeightForWidth());
    fVersion->setSizePolicy(sizePolicy3);
    fVersion->setWordWrap(true);

    gridLayout->addWidget(fVersion, 5, 1, 1, 1);

    label_7 = new QLabel(WidgetUserInfo);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    sizePolicy1.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
    label_7->setSizePolicy(sizePolicy1);
    label_7->setFont(font1);
    label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_7, 6, 0, 1, 1);

    fCipher = new QLabel(WidgetUserInfo);
    fCipher->setObjectName(QString::fromUtf8("fCipher"));
    sizePolicy2.setHeightForWidth(fCipher->sizePolicy().hasHeightForWidth());
    fCipher->setSizePolicy(sizePolicy2);
    fCipher->setWordWrap(true);

    gridLayout->addWidget(fCipher, 6, 1, 1, 1);

    label_8 = new QLabel(WidgetUserInfo);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    sizePolicy1.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
    label_8->setSizePolicy(sizePolicy1);
    label_8->setFont(font1);
    label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_8, 7, 0, 1, 1);

    fLoginTime = new QLabel(WidgetUserInfo);
    fLoginTime->setObjectName(QString::fromUtf8("fLoginTime"));
    sizePolicy2.setHeightForWidth(fLoginTime->sizePolicy().hasHeightForWidth());
    fLoginTime->setSizePolicy(sizePolicy2);
    fLoginTime->setWordWrap(true);

    gridLayout->addWidget(fLoginTime, 7, 1, 1, 1);

    label_9 = new QLabel(WidgetUserInfo);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    sizePolicy1.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
    label_9->setSizePolicy(sizePolicy1);
    label_9->setFont(font1);
    label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_9, 8, 0, 1, 1);

    fIdleTime = new QLabel(WidgetUserInfo);
    fIdleTime->setObjectName(QString::fromUtf8("fIdleTime"));
    sizePolicy2.setHeightForWidth(fIdleTime->sizePolicy().hasHeightForWidth());
    fIdleTime->setSizePolicy(sizePolicy2);
    fIdleTime->setWordWrap(true);

    gridLayout->addWidget(fIdleTime, 8, 1, 1, 1);


    vboxLayout->addLayout(gridLayout);

    fGroupDownloads = new QGroupBox(WidgetUserInfo);
    fGroupDownloads->setObjectName(QString::fromUtf8("fGroupDownloads"));
    fGroupDownloads->setFont(font1);
    fGroupDownloads_2 = new QGroupBox(fGroupDownloads);
    fGroupDownloads_2->setObjectName(QString::fromUtf8("fGroupDownloads_2"));
    fGroupDownloads_2->setGeometry(QRect(19, 269, 302, 18));
    fGroupDownloads_2->setFont(font1);

    vboxLayout->addWidget(fGroupDownloads);

    fGroupUploads = new QGroupBox(WidgetUserInfo);
    fGroupUploads->setObjectName(QString::fromUtf8("fGroupUploads"));
    fGroupUploads->setFont(font1);

    vboxLayout->addWidget(fGroupUploads);

    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);


    retranslateUi(WidgetUserInfo);

    QMetaObject::connectSlotsByName(WidgetUserInfo);
    } // setupUi

    void retranslateUi(QWidget *WidgetUserInfo)
    {
    WidgetUserInfo->setWindowTitle(QApplication::translate("WidgetUserInfo", "User Information", 0, QApplication::UnicodeUTF8));
    fIcon->setText(QString());
    fNick->setText(QApplication::translate("WidgetUserInfo", "user name", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WidgetUserInfo", "Status:", 0, QApplication::UnicodeUTF8));
    fStatus->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WidgetUserInfo", "Login:", 0, QApplication::UnicodeUTF8));
    fLogin->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("WidgetUserInfo", "ID:", 0, QApplication::UnicodeUTF8));
    fID->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("WidgetUserInfo", "Address:", 0, QApplication::UnicodeUTF8));
    fAddress->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("WidgetUserInfo", "Host:", 0, QApplication::UnicodeUTF8));
    fHost->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("WidgetUserInfo", "Version:", 0, QApplication::UnicodeUTF8));
    fVersion->setText(QApplication::translate("WidgetUserInfo", "this is a really really long text that shows how the window is wrapped", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("WidgetUserInfo", "Cipher:", 0, QApplication::UnicodeUTF8));
    fCipher->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("WidgetUserInfo", "Login Time:", 0, QApplication::UnicodeUTF8));
    fLoginTime->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("WidgetUserInfo", "Idle Time:", 0, QApplication::UnicodeUTF8));
    fIdleTime->setText(QApplication::translate("WidgetUserInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    fGroupDownloads->setTitle(QApplication::translate("WidgetUserInfo", "Downloads", 0, QApplication::UnicodeUTF8));
    fGroupDownloads_2->setTitle(QApplication::translate("WidgetUserInfo", "Downloads", 0, QApplication::UnicodeUTF8));
    fGroupUploads->setTitle(QApplication::translate("WidgetUserInfo", "Uploads", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetUserInfo);
    } // retranslateUi

};

namespace Ui {
    class WidgetUserInfo: public Ui_WidgetUserInfo {};
} // namespace Ui

#endif // UI_WIDGETUSERINFO_H
