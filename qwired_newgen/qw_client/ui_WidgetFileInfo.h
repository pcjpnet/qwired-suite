/********************************************************************************
** Form generated from reading ui file 'WidgetFileInfo.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETFILEINFO_H
#define UI_WIDGETFILEINFO_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetFileInfo
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *fIcon;
    QLineEdit *fName;
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *fKind;
    QLabel *label_2;
    QLabel *fSize;
    QLabel *label_3;
    QLabel *fPath;
    QLabel *label_4;
    QLabel *fCreated;
    QLabel *label_5;
    QLabel *fModified;
    QLabel *label_6;
    QLabel *fChecksum;
    QVBoxLayout *vboxLayout1;
    QLabel *label_7;
    QTextEdit *fComments;

    void setupUi(QWidget *WidgetFileInfo)
    {
    if (WidgetFileInfo->objectName().isEmpty())
        WidgetFileInfo->setObjectName(QString::fromUtf8("WidgetFileInfo"));
    WidgetFileInfo->resize(300, 271);
    WidgetFileInfo->setWindowIcon(QIcon(QString::fromUtf8(":/icons/btn_info.png")));
    vboxLayout = new QVBoxLayout(WidgetFileInfo);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fIcon = new QLabel(WidgetFileInfo);
    fIcon->setObjectName(QString::fromUtf8("fIcon"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(fIcon->sizePolicy().hasHeightForWidth());
    fIcon->setSizePolicy(sizePolicy);
    fIcon->setMinimumSize(QSize(16, 16));
    fIcon->setPixmap(QPixmap(QString::fromUtf8(":/icons/files/files-text.png")));

    hboxLayout->addWidget(fIcon);

    fName = new QLineEdit(WidgetFileInfo);
    fName->setObjectName(QString::fromUtf8("fName"));

    hboxLayout->addWidget(fName);


    vboxLayout->addLayout(hboxLayout);

    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setHorizontalSpacing(4);
    gridLayout->setVerticalSpacing(4);
    label = new QLabel(WidgetFileInfo);
    label->setObjectName(QString::fromUtf8("label"));
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    label->setFont(font);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    fKind = new QLabel(WidgetFileInfo);
    fKind->setObjectName(QString::fromUtf8("fKind"));
    QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fKind->sizePolicy().hasHeightForWidth());
    fKind->setSizePolicy(sizePolicy1);
    fKind->setWordWrap(true);

    gridLayout->addWidget(fKind, 0, 1, 1, 1);

    label_2 = new QLabel(WidgetFileInfo);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setFont(font);
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    fSize = new QLabel(WidgetFileInfo);
    fSize->setObjectName(QString::fromUtf8("fSize"));
    sizePolicy1.setHeightForWidth(fSize->sizePolicy().hasHeightForWidth());
    fSize->setSizePolicy(sizePolicy1);
    fSize->setWordWrap(true);

    gridLayout->addWidget(fSize, 1, 1, 1, 1);

    label_3 = new QLabel(WidgetFileInfo);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setFont(font);
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_3, 2, 0, 1, 1);

    fPath = new QLabel(WidgetFileInfo);
    fPath->setObjectName(QString::fromUtf8("fPath"));
    sizePolicy1.setHeightForWidth(fPath->sizePolicy().hasHeightForWidth());
    fPath->setSizePolicy(sizePolicy1);
    fPath->setWordWrap(true);

    gridLayout->addWidget(fPath, 2, 1, 1, 1);

    label_4 = new QLabel(WidgetFileInfo);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setFont(font);
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_4, 3, 0, 1, 1);

    fCreated = new QLabel(WidgetFileInfo);
    fCreated->setObjectName(QString::fromUtf8("fCreated"));
    sizePolicy1.setHeightForWidth(fCreated->sizePolicy().hasHeightForWidth());
    fCreated->setSizePolicy(sizePolicy1);
    fCreated->setWordWrap(true);

    gridLayout->addWidget(fCreated, 3, 1, 1, 1);

    label_5 = new QLabel(WidgetFileInfo);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setFont(font);
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_5, 4, 0, 1, 1);

    fModified = new QLabel(WidgetFileInfo);
    fModified->setObjectName(QString::fromUtf8("fModified"));
    sizePolicy1.setHeightForWidth(fModified->sizePolicy().hasHeightForWidth());
    fModified->setSizePolicy(sizePolicy1);
    fModified->setWordWrap(true);

    gridLayout->addWidget(fModified, 4, 1, 1, 1);

    label_6 = new QLabel(WidgetFileInfo);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setFont(font);
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_6, 5, 0, 1, 1);

    fChecksum = new QLabel(WidgetFileInfo);
    fChecksum->setObjectName(QString::fromUtf8("fChecksum"));
    sizePolicy1.setHeightForWidth(fChecksum->sizePolicy().hasHeightForWidth());
    fChecksum->setSizePolicy(sizePolicy1);
    fChecksum->setWordWrap(true);

    gridLayout->addWidget(fChecksum, 5, 1, 1, 1);


    vboxLayout->addLayout(gridLayout);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(2);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    label_7 = new QLabel(WidgetFileInfo);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setFont(font);

    vboxLayout1->addWidget(label_7);

    fComments = new QTextEdit(WidgetFileInfo);
    fComments->setObjectName(QString::fromUtf8("fComments"));

    vboxLayout1->addWidget(fComments);


    vboxLayout->addLayout(vboxLayout1);


    retranslateUi(WidgetFileInfo);

    QMetaObject::connectSlotsByName(WidgetFileInfo);
    } // setupUi

    void retranslateUi(QWidget *WidgetFileInfo)
    {
    WidgetFileInfo->setWindowTitle(QApplication::translate("WidgetFileInfo", "File Information", 0, QApplication::UnicodeUTF8));
    fIcon->setText(QString());
    label->setText(QApplication::translate("WidgetFileInfo", "Kind:", 0, QApplication::UnicodeUTF8));
    fKind->setText(QApplication::translate("WidgetFileInfo", "file", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WidgetFileInfo", "Size:", 0, QApplication::UnicodeUTF8));
    fSize->setText(QApplication::translate("WidgetFileInfo", "file", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("WidgetFileInfo", "Where:", 0, QApplication::UnicodeUTF8));
    fPath->setText(QApplication::translate("WidgetFileInfo", "file", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("WidgetFileInfo", "Created:", 0, QApplication::UnicodeUTF8));
    fCreated->setText(QApplication::translate("WidgetFileInfo", "file", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("WidgetFileInfo", "Modified:", 0, QApplication::UnicodeUTF8));
    fModified->setText(QApplication::translate("WidgetFileInfo", "file", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("WidgetFileInfo", "Checksum:", 0, QApplication::UnicodeUTF8));
    fChecksum->setText(QApplication::translate("WidgetFileInfo", "file", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("WidgetFileInfo", "Comments:", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetFileInfo);
    } // retranslateUi

};

namespace Ui {
    class WidgetFileInfo: public Ui_WidgetFileInfo {};
} // namespace Ui

#endif // UI_WIDGETFILEINFO_H
