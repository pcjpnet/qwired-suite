/********************************************************************************
** Form generated from reading ui file 'WidgetMotd.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETMOTD_H
#define UI_WIDGETMOTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetMotd
{
public:
    QVBoxLayout *vboxLayout;
    QTextBrowser *fMotd;

    void setupUi(QWidget *WidgetMotd)
    {
    if (WidgetMotd->objectName().isEmpty())
        WidgetMotd->setObjectName(QString::fromUtf8("WidgetMotd"));
    WidgetMotd->resize(498, 430);
    vboxLayout = new QVBoxLayout(WidgetMotd);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    fMotd = new QTextBrowser(WidgetMotd);
    fMotd->setObjectName(QString::fromUtf8("fMotd"));
    fMotd->setOpenExternalLinks(true);
    fMotd->setOpenLinks(false);

    vboxLayout->addWidget(fMotd);


    retranslateUi(WidgetMotd);

    QMetaObject::connectSlotsByName(WidgetMotd);
    } // setupUi

    void retranslateUi(QWidget *WidgetMotd)
    {
    WidgetMotd->setWindowTitle(QApplication::translate("WidgetMotd", "Message Of The Day", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetMotd);
    } // retranslateUi

};

namespace Ui {
    class WidgetMotd: public Ui_WidgetMotd {};
} // namespace Ui

#endif // UI_WIDGETMOTD_H
