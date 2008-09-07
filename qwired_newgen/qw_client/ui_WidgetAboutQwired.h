/********************************************************************************
** Form generated from reading ui file 'WidgetAboutQwired.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETABOUTQWIRED_H
#define UI_WIDGETABOUTQWIRED_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetAboutQwired
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QLabel *label_2;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *WidgetAboutQwired)
    {
    if (WidgetAboutQwired->objectName().isEmpty())
        WidgetAboutQwired->setObjectName(QString::fromUtf8("WidgetAboutQwired"));
    WidgetAboutQwired->resize(330, 370);
    vboxLayout = new QVBoxLayout(WidgetAboutQwired);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    label = new QLabel(WidgetAboutQwired);
    label->setObjectName(QString::fromUtf8("label"));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);
    label->setPixmap(QPixmap(QString::fromUtf8(":/icons/qwired_logo_48.png")));
    label->setAlignment(Qt::AlignCenter);

    vboxLayout->addWidget(label);

    label_2 = new QLabel(WidgetAboutQwired);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setAlignment(Qt::AlignCenter);

    vboxLayout->addWidget(label_2);

    textBrowser = new QTextBrowser(WidgetAboutQwired);
    textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

    vboxLayout->addWidget(textBrowser);


    retranslateUi(WidgetAboutQwired);

    QMetaObject::connectSlotsByName(WidgetAboutQwired);
    } // setupUi

    void retranslateUi(QWidget *WidgetAboutQwired)
    {
    WidgetAboutQwired->setWindowTitle(QApplication::translate("WidgetAboutQwired", "Form", 0, QApplication::UnicodeUTF8));
    label->setText(QString());
    label_2->setText(QApplication::translate("WidgetAboutQwired", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:13pt; font-weight:600;\">Qwired</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Version %1</p></body></html>", 0, QApplication::UnicodeUTF8));
    textBrowser->setHtml(QApplication::translate("WidgetAboutQwired", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt; font-weight:600;\">Qwired is a free client for the Wired protocol. It is written in C++ using the Qt4 framework and released under the terms of the GPL (General Public License).</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"><span style=\" font-weight:600;\">Project Website</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-r"
        "ight:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">http://qwired-client.sourceforge.net/</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"><span style=\" font-weight:600;\">Programming</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt; font-weight:600;\"><span style=\" font-weight:400;\">Bastian Bense &lt;bastibense@gmail.com&gt;</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size"
        ":10pt;\"><span style=\" font-weight:600;\">Translations</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">French --- Sun-Beach</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">Portuguese --- NightDog</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">German --- Bastian Bense &lt;bastibense@gmail.com&gt;</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">Italian --- amStaff</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -q"
        "t-block-indent:0; text-indent:0px; font-size:10pt; font-weight:600;\">Special Thanks</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">Axel Andersson --- for creating Wired and releasing it's protocol specification to the public domain.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\">Urban Weigl --- for testing and mental support and hosting.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:10pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetAboutQwired);
    } // retranslateUi

};

namespace Ui {
    class WidgetAboutQwired: public Ui_WidgetAboutQwired {};
} // namespace Ui

#endif // UI_WIDGETABOUTQWIRED_H
