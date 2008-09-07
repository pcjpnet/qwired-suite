/********************************************************************************
** Form generated from reading ui file 'WidgetServerInfo.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETSERVERINFO_H
#define UI_WIDGETSERVERINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetServerInfo
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QLabel *fServerName;
    QFrame *line;
    QLabel *fBanner;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *fDescription;
    QLabel *label_2;
    QLabel *fUptime;
    QLabel *label_3;
    QLabel *fURL;
    QLabel *label_4;
    QLabel *fFiles;
    QLabel *label_5;
    QLabel *fSize;
    QLabel *label_6;
    QLabel *fVersion;
    QLabel *label_7;
    QLabel *fProtocol;
    QLabel *label_8;
    QLabel *fSslProtocol;

    void setupUi(QWidget *WidgetServerInfo)
    {
    if (WidgetServerInfo->objectName().isEmpty())
        WidgetServerInfo->setObjectName(QString::fromUtf8("WidgetServerInfo"));
    WidgetServerInfo->resize(300, 280);
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(WidgetServerInfo->sizePolicy().hasHeightForWidth());
    WidgetServerInfo->setSizePolicy(sizePolicy);
    hboxLayout = new QHBoxLayout(WidgetServerInfo);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    fServerName = new QLabel(WidgetServerInfo);
    fServerName->setObjectName(QString::fromUtf8("fServerName"));
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    fServerName->setFont(font);

    vboxLayout->addWidget(fServerName);

    line = new QFrame(WidgetServerInfo);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    vboxLayout->addWidget(line);

    fBanner = new QLabel(WidgetServerInfo);
    fBanner->setObjectName(QString::fromUtf8("fBanner"));
    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fBanner->sizePolicy().hasHeightForWidth());
    fBanner->setSizePolicy(sizePolicy1);
    fBanner->setMinimumSize(QSize(200, 32));
    fBanner->setFrameShape(QFrame::StyledPanel);
    fBanner->setFrameShadow(QFrame::Raised);
    fBanner->setPixmap(QPixmap());
    fBanner->setScaledContents(false);
    fBanner->setMargin(1);

    vboxLayout->addWidget(fBanner);

    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setVerticalSpacing(0);
    gridLayout->setContentsMargins(-1, -1, -1, 6);
    label = new QLabel(WidgetServerInfo);
    label->setObjectName(QString::fromUtf8("label"));
    QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy2);
    QFont font1;
    font1.setBold(true);
    font1.setWeight(75);
    label->setFont(font1);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    fDescription = new QLabel(WidgetServerInfo);
    fDescription->setObjectName(QString::fromUtf8("fDescription"));
    QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(fDescription->sizePolicy().hasHeightForWidth());
    fDescription->setSizePolicy(sizePolicy3);
    fDescription->setWordWrap(true);

    gridLayout->addWidget(fDescription, 0, 1, 1, 1);

    label_2 = new QLabel(WidgetServerInfo);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
    label_2->setSizePolicy(sizePolicy2);
    label_2->setFont(font1);
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    fUptime = new QLabel(WidgetServerInfo);
    fUptime->setObjectName(QString::fromUtf8("fUptime"));
    sizePolicy3.setHeightForWidth(fUptime->sizePolicy().hasHeightForWidth());
    fUptime->setSizePolicy(sizePolicy3);
    fUptime->setWordWrap(true);

    gridLayout->addWidget(fUptime, 1, 1, 1, 1);

    label_3 = new QLabel(WidgetServerInfo);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    sizePolicy2.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
    label_3->setSizePolicy(sizePolicy2);
    label_3->setFont(font1);
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_3, 2, 0, 1, 1);

    fURL = new QLabel(WidgetServerInfo);
    fURL->setObjectName(QString::fromUtf8("fURL"));
    sizePolicy3.setHeightForWidth(fURL->sizePolicy().hasHeightForWidth());
    fURL->setSizePolicy(sizePolicy3);
    fURL->setWordWrap(true);

    gridLayout->addWidget(fURL, 2, 1, 1, 1);

    label_4 = new QLabel(WidgetServerInfo);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    sizePolicy2.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
    label_4->setSizePolicy(sizePolicy2);
    label_4->setFont(font1);
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_4, 3, 0, 1, 1);

    fFiles = new QLabel(WidgetServerInfo);
    fFiles->setObjectName(QString::fromUtf8("fFiles"));
    sizePolicy3.setHeightForWidth(fFiles->sizePolicy().hasHeightForWidth());
    fFiles->setSizePolicy(sizePolicy3);
    fFiles->setWordWrap(true);

    gridLayout->addWidget(fFiles, 3, 1, 1, 1);

    label_5 = new QLabel(WidgetServerInfo);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
    label_5->setSizePolicy(sizePolicy2);
    label_5->setFont(font1);
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_5, 4, 0, 1, 1);

    fSize = new QLabel(WidgetServerInfo);
    fSize->setObjectName(QString::fromUtf8("fSize"));
    sizePolicy3.setHeightForWidth(fSize->sizePolicy().hasHeightForWidth());
    fSize->setSizePolicy(sizePolicy3);
    fSize->setWordWrap(true);

    gridLayout->addWidget(fSize, 4, 1, 1, 1);

    label_6 = new QLabel(WidgetServerInfo);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    sizePolicy2.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
    label_6->setSizePolicy(sizePolicy2);
    label_6->setFont(font1);
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_6, 5, 0, 1, 1);

    fVersion = new QLabel(WidgetServerInfo);
    fVersion->setObjectName(QString::fromUtf8("fVersion"));
    QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(fVersion->sizePolicy().hasHeightForWidth());
    fVersion->setSizePolicy(sizePolicy4);
    fVersion->setWordWrap(true);

    gridLayout->addWidget(fVersion, 5, 1, 1, 1);

    label_7 = new QLabel(WidgetServerInfo);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    sizePolicy2.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
    label_7->setSizePolicy(sizePolicy2);
    label_7->setFont(font1);
    label_7->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_7, 6, 0, 1, 1);

    fProtocol = new QLabel(WidgetServerInfo);
    fProtocol->setObjectName(QString::fromUtf8("fProtocol"));
    sizePolicy3.setHeightForWidth(fProtocol->sizePolicy().hasHeightForWidth());
    fProtocol->setSizePolicy(sizePolicy3);
    fProtocol->setWordWrap(true);

    gridLayout->addWidget(fProtocol, 6, 1, 1, 1);

    label_8 = new QLabel(WidgetServerInfo);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
    label_8->setSizePolicy(sizePolicy2);
    label_8->setFont(font1);
    label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_8, 7, 0, 1, 1);

    fSslProtocol = new QLabel(WidgetServerInfo);
    fSslProtocol->setObjectName(QString::fromUtf8("fSslProtocol"));
    sizePolicy3.setHeightForWidth(fSslProtocol->sizePolicy().hasHeightForWidth());
    fSslProtocol->setSizePolicy(sizePolicy3);
    fSslProtocol->setWordWrap(true);

    gridLayout->addWidget(fSslProtocol, 7, 1, 1, 1);


    vboxLayout->addLayout(gridLayout);


    hboxLayout->addLayout(vboxLayout);


    retranslateUi(WidgetServerInfo);

    QMetaObject::connectSlotsByName(WidgetServerInfo);
    } // setupUi

    void retranslateUi(QWidget *WidgetServerInfo)
    {
    WidgetServerInfo->setWindowTitle(QApplication::translate("WidgetServerInfo", "Server Information", 0, QApplication::UnicodeUTF8));
    fServerName->setText(QApplication::translate("WidgetServerInfo", "Server Name", 0, QApplication::UnicodeUTF8));
    fBanner->setText(QString());
    label->setText(QApplication::translate("WidgetServerInfo", "Description:", 0, QApplication::UnicodeUTF8));
    fDescription->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WidgetServerInfo", "Uptime:", 0, QApplication::UnicodeUTF8));
    fUptime->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("WidgetServerInfo", "URL:", 0, QApplication::UnicodeUTF8));
    fURL->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("WidgetServerInfo", "Files:", 0, QApplication::UnicodeUTF8));
    fFiles->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("WidgetServerInfo", "Size:", 0, QApplication::UnicodeUTF8));
    fSize->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("WidgetServerInfo", "Version:", 0, QApplication::UnicodeUTF8));
    fVersion->setText(QApplication::translate("WidgetServerInfo", "Version", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("WidgetServerInfo", "Protocol:", 0, QApplication::UnicodeUTF8));
    fProtocol->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("WidgetServerInfo", "SSL Protocol:", 0, QApplication::UnicodeUTF8));
    fSslProtocol->setText(QApplication::translate("WidgetServerInfo", "TextLabel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetServerInfo);
    } // retranslateUi

};

namespace Ui {
    class WidgetServerInfo: public Ui_WidgetServerInfo {};
} // namespace Ui

#endif // UI_WIDGETSERVERINFO_H
