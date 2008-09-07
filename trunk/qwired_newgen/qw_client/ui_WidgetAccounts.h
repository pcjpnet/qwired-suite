/********************************************************************************
** Form generated from reading ui file 'WidgetAccounts.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETACCOUNTS_H
#define UI_WIDGETACCOUNTS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetAccounts
{
public:
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout;
    QListWidget *fList;
    QHBoxLayout *hboxLayout1;
    QToolButton *fBtnNew;
    QSpacerItem *spacerItem;
    QVBoxLayout *vboxLayout1;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *fType;
    QLabel *label_2;
    QLineEdit *fName;
    QLabel *label_3;
    QLineEdit *fPassword;
    QLabel *label_4;
    QComboBox *fGroup;
    QGridLayout *gridLayout1;
    QGroupBox *fGroupBasic;
    QHBoxLayout *hboxLayout2;
    QGridLayout *gridLayout2;
    QCheckBox *fBasicGetUserInfo;
    QCheckBox *fBasicPostNews;
    QCheckBox *fBasicBroadcast;
    QCheckBox *fBasicClearNews;
    QCheckBox *fBasicSetTopic;
    QSpacerItem *spacerItem1;
    QGroupBox *fGroupFiles;
    QHBoxLayout *hboxLayout3;
    QGridLayout *gridLayout3;
    QCheckBox *fFilesDownload;
    QCheckBox *fFilesCreateFolders;
    QCheckBox *fFilesMoveChange;
    QCheckBox *fFilesUpload;
    QCheckBox *fFilesUploadAnywhere;
    QCheckBox *fFilesDelete;
    QCheckBox *fFilesViewDropBoxes;
    QSpacerItem *spacerItem2;
    QGroupBox *fGroupAdmin;
    QHBoxLayout *hboxLayout4;
    QGridLayout *gridLayout4;
    QCheckBox *fUsersCreate;
    QCheckBox *fUsersKick;
    QCheckBox *fUsersEdit;
    QCheckBox *fUsersBan;
    QCheckBox *fUsersDelete;
    QCheckBox *fUsersNoKick;
    QCheckBox *fUsersElevate;
    QSpacerItem *spacerItem3;
    QGroupBox *fGroupLimits;
    QGridLayout *gridLayout5;
    QLabel *label_5;
    QLineEdit *fLimitDown;
    QLabel *label_7;
    QLabel *label_6;
    QLineEdit *fLimitUp;
    QLabel *label_8;
    QSpacerItem *spacerItem4;
    QHBoxLayout *hboxLayout5;
    QSpacerItem *spacerItem5;
    QToolButton *fBtnDelete;
    QToolButton *fBtnApply;

    void setupUi(QWidget *WidgetAccounts)
    {
    if (WidgetAccounts->objectName().isEmpty())
        WidgetAccounts->setObjectName(QString::fromUtf8("WidgetAccounts"));
    WidgetAccounts->resize(664, 502);
    hboxLayout = new QHBoxLayout(WidgetAccounts);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    fList = new QListWidget(WidgetAccounts);
    fList->setObjectName(QString::fromUtf8("fList"));

    vboxLayout->addWidget(fList);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    fBtnNew = new QToolButton(WidgetAccounts);
    fBtnNew->setObjectName(QString::fromUtf8("fBtnNew"));

    hboxLayout1->addWidget(fBtnNew);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);


    vboxLayout->addLayout(hboxLayout1);


    hboxLayout->addLayout(vboxLayout);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    label = new QLabel(WidgetAccounts);
    label->setObjectName(QString::fromUtf8("label"));
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    label->setFont(font);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label, 0, 0, 1, 1);

    fType = new QComboBox(WidgetAccounts);
    fType->setObjectName(QString::fromUtf8("fType"));
    fType->setEnabled(false);

    gridLayout->addWidget(fType, 0, 1, 1, 1);

    label_2 = new QLabel(WidgetAccounts);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setFont(font);
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_2, 1, 0, 1, 1);

    fName = new QLineEdit(WidgetAccounts);
    fName->setObjectName(QString::fromUtf8("fName"));
    fName->setEnabled(false);

    gridLayout->addWidget(fName, 1, 1, 1, 1);

    label_3 = new QLabel(WidgetAccounts);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setFont(font);
    label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_3, 2, 0, 1, 1);

    fPassword = new QLineEdit(WidgetAccounts);
    fPassword->setObjectName(QString::fromUtf8("fPassword"));
    fPassword->setEnabled(false);
    fPassword->setEchoMode(QLineEdit::Password);

    gridLayout->addWidget(fPassword, 2, 1, 1, 1);

    label_4 = new QLabel(WidgetAccounts);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setFont(font);
    label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout->addWidget(label_4, 3, 0, 1, 1);

    fGroup = new QComboBox(WidgetAccounts);
    fGroup->setObjectName(QString::fromUtf8("fGroup"));
    fGroup->setEnabled(false);

    gridLayout->addWidget(fGroup, 3, 1, 1, 1);


    vboxLayout1->addLayout(gridLayout);

    gridLayout1 = new QGridLayout();
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    fGroupBasic = new QGroupBox(WidgetAccounts);
    fGroupBasic->setObjectName(QString::fromUtf8("fGroupBasic"));
    fGroupBasic->setEnabled(false);
    fGroupBasic->setFont(font);
    hboxLayout2 = new QHBoxLayout(fGroupBasic);
    hboxLayout2->setSpacing(2);
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    gridLayout2->setHorizontalSpacing(0);
    gridLayout2->setVerticalSpacing(0);
    fBasicGetUserInfo = new QCheckBox(fGroupBasic);
    fBasicGetUserInfo->setObjectName(QString::fromUtf8("fBasicGetUserInfo"));
    QFont font1;
    font1.setPointSize(10);
    font1.setBold(false);
    font1.setWeight(50);
    fBasicGetUserInfo->setFont(font1);

    gridLayout2->addWidget(fBasicGetUserInfo, 0, 0, 1, 1);

    fBasicPostNews = new QCheckBox(fGroupBasic);
    fBasicPostNews->setObjectName(QString::fromUtf8("fBasicPostNews"));
    fBasicPostNews->setFont(font1);

    gridLayout2->addWidget(fBasicPostNews, 0, 1, 1, 1);

    fBasicBroadcast = new QCheckBox(fGroupBasic);
    fBasicBroadcast->setObjectName(QString::fromUtf8("fBasicBroadcast"));
    fBasicBroadcast->setFont(font1);

    gridLayout2->addWidget(fBasicBroadcast, 1, 0, 1, 1);

    fBasicClearNews = new QCheckBox(fGroupBasic);
    fBasicClearNews->setObjectName(QString::fromUtf8("fBasicClearNews"));
    fBasicClearNews->setFont(font1);

    gridLayout2->addWidget(fBasicClearNews, 1, 1, 1, 1);

    fBasicSetTopic = new QCheckBox(fGroupBasic);
    fBasicSetTopic->setObjectName(QString::fromUtf8("fBasicSetTopic"));
    fBasicSetTopic->setFont(font1);

    gridLayout2->addWidget(fBasicSetTopic, 2, 0, 1, 1);

    spacerItem1 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem1, 3, 0, 1, 1);


    hboxLayout2->addLayout(gridLayout2);


    gridLayout1->addWidget(fGroupBasic, 0, 0, 1, 1);

    fGroupFiles = new QGroupBox(WidgetAccounts);
    fGroupFiles->setObjectName(QString::fromUtf8("fGroupFiles"));
    fGroupFiles->setEnabled(false);
    fGroupFiles->setFont(font);
    hboxLayout3 = new QHBoxLayout(fGroupFiles);
    hboxLayout3->setSpacing(2);
    hboxLayout3->setObjectName(QString::fromUtf8("hboxLayout3"));
    gridLayout3 = new QGridLayout();
    gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
    gridLayout3->setHorizontalSpacing(0);
    gridLayout3->setVerticalSpacing(0);
    fFilesDownload = new QCheckBox(fGroupFiles);
    fFilesDownload->setObjectName(QString::fromUtf8("fFilesDownload"));
    fFilesDownload->setFont(font1);

    gridLayout3->addWidget(fFilesDownload, 0, 0, 2, 1);

    fFilesCreateFolders = new QCheckBox(fGroupFiles);
    fFilesCreateFolders->setObjectName(QString::fromUtf8("fFilesCreateFolders"));
    fFilesCreateFolders->setFont(font1);

    gridLayout3->addWidget(fFilesCreateFolders, 0, 1, 1, 1);

    fFilesMoveChange = new QCheckBox(fGroupFiles);
    fFilesMoveChange->setObjectName(QString::fromUtf8("fFilesMoveChange"));
    fFilesMoveChange->setFont(font1);

    gridLayout3->addWidget(fFilesMoveChange, 1, 1, 3, 1);

    fFilesUpload = new QCheckBox(fGroupFiles);
    fFilesUpload->setObjectName(QString::fromUtf8("fFilesUpload"));
    fFilesUpload->setFont(font1);

    gridLayout3->addWidget(fFilesUpload, 2, 0, 1, 1);

    fFilesUploadAnywhere = new QCheckBox(fGroupFiles);
    fFilesUploadAnywhere->setObjectName(QString::fromUtf8("fFilesUploadAnywhere"));
    fFilesUploadAnywhere->setFont(font1);

    gridLayout3->addWidget(fFilesUploadAnywhere, 3, 0, 2, 1);

    fFilesDelete = new QCheckBox(fGroupFiles);
    fFilesDelete->setObjectName(QString::fromUtf8("fFilesDelete"));
    fFilesDelete->setFont(font1);

    gridLayout3->addWidget(fFilesDelete, 4, 1, 1, 1);

    fFilesViewDropBoxes = new QCheckBox(fGroupFiles);
    fFilesViewDropBoxes->setObjectName(QString::fromUtf8("fFilesViewDropBoxes"));
    fFilesViewDropBoxes->setFont(font1);

    gridLayout3->addWidget(fFilesViewDropBoxes, 5, 0, 1, 1);

    spacerItem2 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout3->addItem(spacerItem2, 6, 0, 1, 1);


    hboxLayout3->addLayout(gridLayout3);


    gridLayout1->addWidget(fGroupFiles, 0, 1, 1, 1);

    fGroupAdmin = new QGroupBox(WidgetAccounts);
    fGroupAdmin->setObjectName(QString::fromUtf8("fGroupAdmin"));
    fGroupAdmin->setEnabled(false);
    fGroupAdmin->setFont(font);
    hboxLayout4 = new QHBoxLayout(fGroupAdmin);
    hboxLayout4->setSpacing(2);
    hboxLayout4->setObjectName(QString::fromUtf8("hboxLayout4"));
    gridLayout4 = new QGridLayout();
    gridLayout4->setObjectName(QString::fromUtf8("gridLayout4"));
    gridLayout4->setHorizontalSpacing(0);
    gridLayout4->setVerticalSpacing(0);
    fUsersCreate = new QCheckBox(fGroupAdmin);
    fUsersCreate->setObjectName(QString::fromUtf8("fUsersCreate"));
    fUsersCreate->setFont(font1);

    gridLayout4->addWidget(fUsersCreate, 0, 0, 1, 1);

    fUsersKick = new QCheckBox(fGroupAdmin);
    fUsersKick->setObjectName(QString::fromUtf8("fUsersKick"));
    fUsersKick->setFont(font1);

    gridLayout4->addWidget(fUsersKick, 0, 1, 1, 1);

    fUsersEdit = new QCheckBox(fGroupAdmin);
    fUsersEdit->setObjectName(QString::fromUtf8("fUsersEdit"));
    fUsersEdit->setFont(font1);

    gridLayout4->addWidget(fUsersEdit, 1, 0, 1, 1);

    fUsersBan = new QCheckBox(fGroupAdmin);
    fUsersBan->setObjectName(QString::fromUtf8("fUsersBan"));
    fUsersBan->setFont(font1);

    gridLayout4->addWidget(fUsersBan, 1, 1, 1, 1);

    fUsersDelete = new QCheckBox(fGroupAdmin);
    fUsersDelete->setObjectName(QString::fromUtf8("fUsersDelete"));
    fUsersDelete->setFont(font1);

    gridLayout4->addWidget(fUsersDelete, 2, 0, 1, 1);

    fUsersNoKick = new QCheckBox(fGroupAdmin);
    fUsersNoKick->setObjectName(QString::fromUtf8("fUsersNoKick"));
    fUsersNoKick->setFont(font1);

    gridLayout4->addWidget(fUsersNoKick, 2, 1, 1, 1);

    fUsersElevate = new QCheckBox(fGroupAdmin);
    fUsersElevate->setObjectName(QString::fromUtf8("fUsersElevate"));
    fUsersElevate->setFont(font1);

    gridLayout4->addWidget(fUsersElevate, 3, 0, 1, 1);

    spacerItem3 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout4->addItem(spacerItem3, 4, 0, 1, 1);


    hboxLayout4->addLayout(gridLayout4);


    gridLayout1->addWidget(fGroupAdmin, 1, 0, 1, 1);

    fGroupLimits = new QGroupBox(WidgetAccounts);
    fGroupLimits->setObjectName(QString::fromUtf8("fGroupLimits"));
    fGroupLimits->setEnabled(false);
    fGroupLimits->setFont(font);
    gridLayout5 = new QGridLayout(fGroupLimits);
    gridLayout5->setObjectName(QString::fromUtf8("gridLayout5"));
    label_5 = new QLabel(fGroupLimits);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
    label_5->setSizePolicy(sizePolicy);
    QFont font2;
    font2.setBold(false);
    font2.setWeight(50);
    label_5->setFont(font2);
    label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout5->addWidget(label_5, 0, 0, 1, 1);

    fLimitDown = new QLineEdit(fGroupLimits);
    fLimitDown->setObjectName(QString::fromUtf8("fLimitDown"));
    fLimitDown->setMinimumSize(QSize(60, 0));
    fLimitDown->setMaximumSize(QSize(70, 16777215));

    gridLayout5->addWidget(fLimitDown, 0, 1, 1, 1);

    label_7 = new QLabel(fGroupLimits);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    sizePolicy.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
    label_7->setSizePolicy(sizePolicy);
    label_7->setFont(font2);

    gridLayout5->addWidget(label_7, 0, 2, 1, 1);

    label_6 = new QLabel(fGroupLimits);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    sizePolicy.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
    label_6->setSizePolicy(sizePolicy);
    label_6->setFont(font2);
    label_6->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    gridLayout5->addWidget(label_6, 1, 0, 1, 1);

    fLimitUp = new QLineEdit(fGroupLimits);
    fLimitUp->setObjectName(QString::fromUtf8("fLimitUp"));
    fLimitUp->setMinimumSize(QSize(60, 0));
    fLimitUp->setMaximumSize(QSize(70, 16777215));

    gridLayout5->addWidget(fLimitUp, 1, 1, 1, 1);

    label_8 = new QLabel(fGroupLimits);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    sizePolicy.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
    label_8->setSizePolicy(sizePolicy);
    label_8->setFont(font2);

    gridLayout5->addWidget(label_8, 1, 2, 1, 1);

    spacerItem4 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout5->addItem(spacerItem4, 2, 0, 1, 1);


    gridLayout1->addWidget(fGroupLimits, 1, 1, 1, 1);


    vboxLayout1->addLayout(gridLayout1);

    hboxLayout5 = new QHBoxLayout();
    hboxLayout5->setObjectName(QString::fromUtf8("hboxLayout5"));
    spacerItem5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout5->addItem(spacerItem5);

    fBtnDelete = new QToolButton(WidgetAccounts);
    fBtnDelete->setObjectName(QString::fromUtf8("fBtnDelete"));

    hboxLayout5->addWidget(fBtnDelete);

    fBtnApply = new QToolButton(WidgetAccounts);
    fBtnApply->setObjectName(QString::fromUtf8("fBtnApply"));

    hboxLayout5->addWidget(fBtnApply);


    vboxLayout1->addLayout(hboxLayout5);


    hboxLayout->addLayout(vboxLayout1);


    retranslateUi(WidgetAccounts);

    QMetaObject::connectSlotsByName(WidgetAccounts);
    } // setupUi

    void retranslateUi(QWidget *WidgetAccounts)
    {
    WidgetAccounts->setWindowTitle(QApplication::translate("WidgetAccounts", "Accounts", 0, QApplication::UnicodeUTF8));
    fBtnNew->setText(QApplication::translate("WidgetAccounts", "New...", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WidgetAccounts", "Type:", 0, QApplication::UnicodeUTF8));
    fType->clear();
    fType->addItem(QIcon(QString::fromUtf8(":/icons/icon_account.png")), QApplication::translate("WidgetAccounts", "Account", 0, QApplication::UnicodeUTF8));
    fType->addItem(QIcon(QString::fromUtf8(":/icons/icon_accountgroup.png")), QApplication::translate("WidgetAccounts", "Group", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("WidgetAccounts", "Name:", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("WidgetAccounts", "Password:", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("WidgetAccounts", "Group:", 0, QApplication::UnicodeUTF8));
    fGroupBasic->setTitle(QApplication::translate("WidgetAccounts", "Basic", 0, QApplication::UnicodeUTF8));
    fBasicGetUserInfo->setText(QApplication::translate("WidgetAccounts", "Get User Info", 0, QApplication::UnicodeUTF8));
    fBasicPostNews->setText(QApplication::translate("WidgetAccounts", "Post News", 0, QApplication::UnicodeUTF8));
    fBasicBroadcast->setText(QApplication::translate("WidgetAccounts", "Broadcast", 0, QApplication::UnicodeUTF8));
    fBasicClearNews->setText(QApplication::translate("WidgetAccounts", "Clear News", 0, QApplication::UnicodeUTF8));
    fBasicSetTopic->setText(QApplication::translate("WidgetAccounts", "Set Topic", 0, QApplication::UnicodeUTF8));
    fGroupFiles->setTitle(QApplication::translate("WidgetAccounts", "Files && Folders", 0, QApplication::UnicodeUTF8));
    fFilesDownload->setText(QApplication::translate("WidgetAccounts", "Download", 0, QApplication::UnicodeUTF8));
    fFilesCreateFolders->setText(QApplication::translate("WidgetAccounts", "Create Folders", 0, QApplication::UnicodeUTF8));
    fFilesMoveChange->setText(QApplication::translate("WidgetAccounts", "Move && Change", 0, QApplication::UnicodeUTF8));
    fFilesUpload->setText(QApplication::translate("WidgetAccounts", "Upload", 0, QApplication::UnicodeUTF8));
    fFilesUploadAnywhere->setText(QApplication::translate("WidgetAccounts", "Upload Anywhere", 0, QApplication::UnicodeUTF8));
    fFilesDelete->setText(QApplication::translate("WidgetAccounts", "Delete", 0, QApplication::UnicodeUTF8));
    fFilesViewDropBoxes->setText(QApplication::translate("WidgetAccounts", "View Drop Boxes", 0, QApplication::UnicodeUTF8));
    fGroupAdmin->setTitle(QApplication::translate("WidgetAccounts", "Users && Groups", 0, QApplication::UnicodeUTF8));
    fUsersCreate->setText(QApplication::translate("WidgetAccounts", "Create Accounts", 0, QApplication::UnicodeUTF8));
    fUsersKick->setText(QApplication::translate("WidgetAccounts", "Kick Users", 0, QApplication::UnicodeUTF8));
    fUsersEdit->setText(QApplication::translate("WidgetAccounts", "Edit Accounts", 0, QApplication::UnicodeUTF8));
    fUsersBan->setText(QApplication::translate("WidgetAccounts", "Ban Users", 0, QApplication::UnicodeUTF8));
    fUsersDelete->setText(QApplication::translate("WidgetAccounts", "Delete Accounts", 0, QApplication::UnicodeUTF8));
    fUsersNoKick->setText(QApplication::translate("WidgetAccounts", "Cannot Be Kicked", 0, QApplication::UnicodeUTF8));
    fUsersElevate->setText(QApplication::translate("WidgetAccounts", "Elevate Privileges", 0, QApplication::UnicodeUTF8));
    fGroupLimits->setTitle(QApplication::translate("WidgetAccounts", "Limits", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("WidgetAccounts", "Download:", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("WidgetAccounts", "KB/s", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("WidgetAccounts", "Upoad:", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("WidgetAccounts", "KB/s", 0, QApplication::UnicodeUTF8));
    fBtnDelete->setText(QApplication::translate("WidgetAccounts", "Delete", 0, QApplication::UnicodeUTF8));
    fBtnApply->setText(QApplication::translate("WidgetAccounts", "Apply Changes", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetAccounts);
    } // retranslateUi

};

namespace Ui {
    class WidgetAccounts: public Ui_WidgetAccounts {};
} // namespace Ui

#endif // UI_WIDGETACCOUNTS_H
