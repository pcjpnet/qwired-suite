/********************************************************************************
** Form generated from reading ui file 'WidgetFileBrowser.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETFILEBROWSER_H
#define UI_WIDGETFILEBROWSER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetFileBrowser
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QToolButton *fBtnBack;
    QSpacerItem *spacerItem;
    QToolButton *fBtnDownload;
    QToolButton *fBtnUpload;
    QToolButton *fBtnInfo;
    QToolButton *fBtnNewFolder;
    QToolButton *fBtnRefresh;
    QSpacerItem *spacerItem1;
    QToolButton *fBtnDelete;
    QSpacerItem *spacerItem2;
    QLineEdit *fFilter;
    QTreeView *fList;
    QLabel *fStats;

    void setupUi(QWidget *WidgetFileBrowser)
    {
    if (WidgetFileBrowser->objectName().isEmpty())
        WidgetFileBrowser->setObjectName(QString::fromUtf8("WidgetFileBrowser"));
    WidgetFileBrowser->resize(500, 399);
    WidgetFileBrowser->setWindowIcon(QIcon(QString::fromUtf8(":/icons/files.png")));
    vboxLayout = new QVBoxLayout(WidgetFileBrowser);
    vboxLayout->setSpacing(4);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(1);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fBtnBack = new QToolButton(WidgetFileBrowser);
    fBtnBack->setObjectName(QString::fromUtf8("fBtnBack"));
    fBtnBack->setEnabled(false);
    fBtnBack->setIcon(QIcon(QString::fromUtf8(":/icons/icn_back.png")));
    fBtnBack->setIconSize(QSize(18, 18));
    fBtnBack->setAutoRaise(true);

    hboxLayout->addWidget(fBtnBack);

    spacerItem = new QSpacerItem(12, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    fBtnDownload = new QToolButton(WidgetFileBrowser);
    fBtnDownload->setObjectName(QString::fromUtf8("fBtnDownload"));
    fBtnDownload->setEnabled(false);
    fBtnDownload->setIcon(QIcon(QString::fromUtf8(":/icons/icn_download.png")));
    fBtnDownload->setIconSize(QSize(18, 18));
    fBtnDownload->setAutoRaise(true);

    hboxLayout->addWidget(fBtnDownload);

    fBtnUpload = new QToolButton(WidgetFileBrowser);
    fBtnUpload->setObjectName(QString::fromUtf8("fBtnUpload"));
    fBtnUpload->setEnabled(false);
    fBtnUpload->setIcon(QIcon(QString::fromUtf8(":/icons/icn_upload.png")));
    fBtnUpload->setIconSize(QSize(18, 18));
    fBtnUpload->setAutoRaise(true);

    hboxLayout->addWidget(fBtnUpload);

    fBtnInfo = new QToolButton(WidgetFileBrowser);
    fBtnInfo->setObjectName(QString::fromUtf8("fBtnInfo"));
    fBtnInfo->setEnabled(false);
    fBtnInfo->setIcon(QIcon(QString::fromUtf8(":/icons/btn_info.png")));
    fBtnInfo->setIconSize(QSize(18, 18));
    fBtnInfo->setAutoRaise(true);

    hboxLayout->addWidget(fBtnInfo);

    fBtnNewFolder = new QToolButton(WidgetFileBrowser);
    fBtnNewFolder->setObjectName(QString::fromUtf8("fBtnNewFolder"));
    fBtnNewFolder->setEnabled(false);
    fBtnNewFolder->setIcon(QIcon(QString::fromUtf8(":/icons/icn_foldernew.png")));
    fBtnNewFolder->setIconSize(QSize(18, 18));
    fBtnNewFolder->setAutoRaise(true);

    hboxLayout->addWidget(fBtnNewFolder);

    fBtnRefresh = new QToolButton(WidgetFileBrowser);
    fBtnRefresh->setObjectName(QString::fromUtf8("fBtnRefresh"));
    fBtnRefresh->setEnabled(false);
    fBtnRefresh->setIcon(QIcon(QString::fromUtf8(":/icons/icn_refresh.png")));
    fBtnRefresh->setIconSize(QSize(18, 18));
    fBtnRefresh->setAutoRaise(true);

    hboxLayout->addWidget(fBtnRefresh);

    spacerItem1 = new QSpacerItem(12, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem1);

    fBtnDelete = new QToolButton(WidgetFileBrowser);
    fBtnDelete->setObjectName(QString::fromUtf8("fBtnDelete"));
    fBtnDelete->setEnabled(false);
    fBtnDelete->setIcon(QIcon(QString::fromUtf8(":/icons/icn_delete.png")));
    fBtnDelete->setIconSize(QSize(18, 18));
    fBtnDelete->setAutoRaise(true);

    hboxLayout->addWidget(fBtnDelete);

    spacerItem2 = new QSpacerItem(6, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem2);

    fFilter = new QLineEdit(WidgetFileBrowser);
    fFilter->setObjectName(QString::fromUtf8("fFilter"));

    hboxLayout->addWidget(fFilter);


    vboxLayout->addLayout(hboxLayout);

    fList = new QTreeView(WidgetFileBrowser);
    fList->setObjectName(QString::fromUtf8("fList"));
    fList->setRootIsDecorated(false);
    fList->setUniformRowHeights(true);
    fList->setItemsExpandable(false);
    fList->setSortingEnabled(true);

    vboxLayout->addWidget(fList);

    fStats = new QLabel(WidgetFileBrowser);
    fStats->setObjectName(QString::fromUtf8("fStats"));
    fStats->setAlignment(Qt::AlignCenter);

    vboxLayout->addWidget(fStats);


    retranslateUi(WidgetFileBrowser);

    QMetaObject::connectSlotsByName(WidgetFileBrowser);
    } // setupUi

    void retranslateUi(QWidget *WidgetFileBrowser)
    {
    WidgetFileBrowser->setWindowTitle(QApplication::translate("WidgetFileBrowser", "Files", 0, QApplication::UnicodeUTF8));
    fBtnBack->setToolTip(QApplication::translate("WidgetFileBrowser", "Back", 0, QApplication::UnicodeUTF8));
    fBtnBack->setText(QString());
    fBtnBack->setShortcut(QApplication::translate("WidgetFileBrowser", "Ctrl+Up", 0, QApplication::UnicodeUTF8));
    fBtnDownload->setToolTip(QApplication::translate("WidgetFileBrowser", "Download File...", 0, QApplication::UnicodeUTF8));
    fBtnDownload->setText(QString());
    fBtnUpload->setToolTip(QApplication::translate("WidgetFileBrowser", "Upload File...", 0, QApplication::UnicodeUTF8));
    fBtnUpload->setText(QString());
    fBtnInfo->setToolTip(QApplication::translate("WidgetFileBrowser", "Get Info", 0, QApplication::UnicodeUTF8));
    fBtnInfo->setText(QString());
    fBtnNewFolder->setToolTip(QApplication::translate("WidgetFileBrowser", "New Folder...", 0, QApplication::UnicodeUTF8));
    fBtnRefresh->setToolTip(QApplication::translate("WidgetFileBrowser", "Refresh", 0, QApplication::UnicodeUTF8));
    fBtnRefresh->setText(QString());
    fBtnRefresh->setShortcut(QApplication::translate("WidgetFileBrowser", "F5", 0, QApplication::UnicodeUTF8));
    fBtnDelete->setToolTip(QApplication::translate("WidgetFileBrowser", "Delete...", 0, QApplication::UnicodeUTF8));
    fBtnDelete->setText(QString());
    fStats->setText(QApplication::translate("WidgetFileBrowser", "0 items, 0 bytes total, 0 GB available", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetFileBrowser);
    } // retranslateUi

};

namespace Ui {
    class WidgetFileBrowser: public Ui_WidgetFileBrowser {};
} // namespace Ui

#endif // UI_WIDGETFILEBROWSER_H
