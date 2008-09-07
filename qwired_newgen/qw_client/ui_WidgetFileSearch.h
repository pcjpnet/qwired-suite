/********************************************************************************
** Form generated from reading ui file 'WidgetFileSearch.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETFILESEARCH_H
#define UI_WIDGETFILESEARCH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetFileSearch
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QLineEdit *fSearch;
    QListWidget *fResults;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem;
    QToolButton *fBtnReveal;
    QToolButton *fBtnDownload;

    void setupUi(QWidget *WidgetFileSearch)
    {
    if (WidgetFileSearch->objectName().isEmpty())
        WidgetFileSearch->setObjectName(QString::fromUtf8("WidgetFileSearch"));
    WidgetFileSearch->resize(400, 301);
    vboxLayout = new QVBoxLayout(WidgetFileSearch);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    label = new QLabel(WidgetFileSearch);
    label->setObjectName(QString::fromUtf8("label"));
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    label->setFont(font);

    hboxLayout->addWidget(label);

    fSearch = new QLineEdit(WidgetFileSearch);
    fSearch->setObjectName(QString::fromUtf8("fSearch"));

    hboxLayout->addWidget(fSearch);


    vboxLayout->addLayout(hboxLayout);

    fResults = new QListWidget(WidgetFileSearch);
    fResults->setObjectName(QString::fromUtf8("fResults"));

    vboxLayout->addWidget(fResults);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    spacerItem = new QSpacerItem(246, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    fBtnReveal = new QToolButton(WidgetFileSearch);
    fBtnReveal->setObjectName(QString::fromUtf8("fBtnReveal"));
    fBtnReveal->setEnabled(false);

    hboxLayout1->addWidget(fBtnReveal);

    fBtnDownload = new QToolButton(WidgetFileSearch);
    fBtnDownload->setObjectName(QString::fromUtf8("fBtnDownload"));
    fBtnDownload->setEnabled(false);

    hboxLayout1->addWidget(fBtnDownload);


    vboxLayout->addLayout(hboxLayout1);


    retranslateUi(WidgetFileSearch);

    QMetaObject::connectSlotsByName(WidgetFileSearch);
    } // setupUi

    void retranslateUi(QWidget *WidgetFileSearch)
    {
    WidgetFileSearch->setWindowTitle(QApplication::translate("WidgetFileSearch", "Form", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("WidgetFileSearch", "Search:", 0, QApplication::UnicodeUTF8));
    fBtnReveal->setText(QApplication::translate("WidgetFileSearch", "Reveal", 0, QApplication::UnicodeUTF8));
    fBtnDownload->setText(QApplication::translate("WidgetFileSearch", "Download", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetFileSearch);
    } // retranslateUi

};

namespace Ui {
    class WidgetFileSearch: public Ui_WidgetFileSearch {};
} // namespace Ui

#endif // UI_WIDGETFILESEARCH_H
