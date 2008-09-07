/********************************************************************************
** Form generated from reading ui file 'WidgetConferenceList.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETCONFERENCELIST_H
#define UI_WIDGETCONFERENCELIST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

class Ui_WidgetConferenceList
{
public:
    QHBoxLayout *hboxLayout;
    QTableWidget *fList;

    void setupUi(QWidget *WidgetConferenceList)
    {
    if (WidgetConferenceList->objectName().isEmpty())
        WidgetConferenceList->setObjectName(QString::fromUtf8("WidgetConferenceList"));
    WidgetConferenceList->resize(626, 340);
    hboxLayout = new QHBoxLayout(WidgetConferenceList);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    fList = new QTableWidget(WidgetConferenceList);
    fList->setObjectName(QString::fromUtf8("fList"));
    fList->setAlternatingRowColors(true);

    hboxLayout->addWidget(fList);


    retranslateUi(WidgetConferenceList);

    QMetaObject::connectSlotsByName(WidgetConferenceList);
    } // setupUi

    void retranslateUi(QWidget *WidgetConferenceList)
    {
    WidgetConferenceList->setWindowTitle(QApplication::translate("WidgetConferenceList", "Conferences", 0, QApplication::UnicodeUTF8));
    if (fList->columnCount() < 3)
        fList->setColumnCount(3);

    QTableWidgetItem *__colItem = new QTableWidgetItem();
    __colItem->setText(QApplication::translate("WidgetConferenceList", "Users", 0, QApplication::UnicodeUTF8));
    fList->setHorizontalHeaderItem(0, __colItem);

    QTableWidgetItem *__colItem1 = new QTableWidgetItem();
    __colItem1->setText(QApplication::translate("WidgetConferenceList", "Topic", 0, QApplication::UnicodeUTF8));
    fList->setHorizontalHeaderItem(1, __colItem1);

    QTableWidgetItem *__colItem2 = new QTableWidgetItem();
    __colItem2->setText(QApplication::translate("WidgetConferenceList", "Description", 0, QApplication::UnicodeUTF8));
    fList->setHorizontalHeaderItem(2, __colItem2);
    Q_UNUSED(WidgetConferenceList);
    } // retranslateUi

};

namespace Ui {
    class WidgetConferenceList: public Ui_WidgetConferenceList {};
} // namespace Ui

#endif // UI_WIDGETCONFERENCELIST_H
