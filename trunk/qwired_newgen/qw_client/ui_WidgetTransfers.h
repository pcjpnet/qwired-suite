/********************************************************************************
** Form generated from reading ui file 'WidgetTransfers.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETTRANSFERS_H
#define UI_WIDGETTRANSFERS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListView>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetTransfers
{
public:
    QVBoxLayout *vboxLayout;
    QListView *fTransfers;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QToolButton *fBtnReveal;
    QToolButton *fBtnCancel;

    void setupUi(QWidget *WidgetTransfers)
    {
    if (WidgetTransfers->objectName().isEmpty())
        WidgetTransfers->setObjectName(QString::fromUtf8("WidgetTransfers"));
    WidgetTransfers->resize(400, 300);
    vboxLayout = new QVBoxLayout(WidgetTransfers);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    fTransfers = new QListView(WidgetTransfers);
    fTransfers->setObjectName(QString::fromUtf8("fTransfers"));

    vboxLayout->addWidget(fTransfers);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    fBtnReveal = new QToolButton(WidgetTransfers);
    fBtnReveal->setObjectName(QString::fromUtf8("fBtnReveal"));
    fBtnReveal->setEnabled(false);

    hboxLayout->addWidget(fBtnReveal);

    fBtnCancel = new QToolButton(WidgetTransfers);
    fBtnCancel->setObjectName(QString::fromUtf8("fBtnCancel"));
    fBtnCancel->setEnabled(false);

    hboxLayout->addWidget(fBtnCancel);


    vboxLayout->addLayout(hboxLayout);


    retranslateUi(WidgetTransfers);

    QMetaObject::connectSlotsByName(WidgetTransfers);
    } // setupUi

    void retranslateUi(QWidget *WidgetTransfers)
    {
    WidgetTransfers->setWindowTitle(QApplication::translate("WidgetTransfers", "Transfers", 0, QApplication::UnicodeUTF8));
    fBtnReveal->setText(QApplication::translate("WidgetTransfers", "Reveal", 0, QApplication::UnicodeUTF8));
    fBtnCancel->setText(QApplication::translate("WidgetTransfers", "Cancel Transfer", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetTransfers);
    } // retranslateUi

};

namespace Ui {
    class WidgetTransfers: public Ui_WidgetTransfers {};
} // namespace Ui

#endif // UI_WIDGETTRANSFERS_H
