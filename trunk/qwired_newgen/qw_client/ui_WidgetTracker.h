/********************************************************************************
** Form generated from reading ui file 'WidgetTracker.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETTRACKER_H
#define UI_WIDGETTRACKER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetTracker
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QComboBox *fServers;
    QToolButton *fBtnRefresh;
    QSpacerItem *spacerItem;
    QLabel *label;
    QLineEdit *fFilter;
    QTreeView *fList;
    QLabel *fProgress;

    void setupUi(QWidget *WidgetTracker)
    {
    if (WidgetTracker->objectName().isEmpty())
        WidgetTracker->setObjectName(QString::fromUtf8("WidgetTracker"));
    WidgetTracker->resize(600, 406);
    vboxLayout = new QVBoxLayout(WidgetTracker);
    vboxLayout->setSpacing(4);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(14, 14, 14, 14);
    hboxLayout = new QHBoxLayout();
    hboxLayout->setSpacing(6);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fServers = new QComboBox(WidgetTracker);
    fServers->setObjectName(QString::fromUtf8("fServers"));
    QFont font;
    font.setPointSize(11);
    fServers->setFont(font);

    hboxLayout->addWidget(fServers);

    fBtnRefresh = new QToolButton(WidgetTracker);
    fBtnRefresh->setObjectName(QString::fromUtf8("fBtnRefresh"));

    hboxLayout->addWidget(fBtnRefresh);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    label = new QLabel(WidgetTracker);
    label->setObjectName(QString::fromUtf8("label"));
    label->setFont(font);
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout->addWidget(label);

    fFilter = new QLineEdit(WidgetTracker);
    fFilter->setObjectName(QString::fromUtf8("fFilter"));
    fFilter->setFont(font);

    hboxLayout->addWidget(fFilter);


    vboxLayout->addLayout(hboxLayout);

    fList = new QTreeView(WidgetTracker);
    fList->setObjectName(QString::fromUtf8("fList"));
    fList->setFont(font);
    fList->setFocusPolicy(Qt::NoFocus);
    fList->setRootIsDecorated(false);
    fList->setItemsExpandable(false);
    fList->setSortingEnabled(true);

    vboxLayout->addWidget(fList);

    fProgress = new QLabel(WidgetTracker);
    fProgress->setObjectName(QString::fromUtf8("fProgress"));
    fProgress->setFont(font);
    fProgress->setAlignment(Qt::AlignCenter);

    vboxLayout->addWidget(fProgress);


    retranslateUi(WidgetTracker);

    QMetaObject::connectSlotsByName(WidgetTracker);
    } // setupUi

    void retranslateUi(QWidget *WidgetTracker)
    {
    WidgetTracker->setWindowTitle(QApplication::translate("WidgetTracker", "Trackers", 0, QApplication::UnicodeUTF8));
    fBtnRefresh->setText(QApplication::translate("WidgetTracker", "Refresh", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WidgetTracker", "Filter:", 0, QApplication::UnicodeUTF8));
    fProgress->setText(QApplication::translate("WidgetTracker", "%1 servers registered with tracker", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetTracker);
    } // retranslateUi

};

namespace Ui {
    class WidgetTracker: public Ui_WidgetTracker {};
} // namespace Ui

#endif // UI_WIDGETTRACKER_H
