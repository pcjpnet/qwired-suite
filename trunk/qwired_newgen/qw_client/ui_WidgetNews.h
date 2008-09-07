/********************************************************************************
** Form generated from reading ui file 'WidgetNews.ui'
**
** Created: Sat Jun 28 12:52:38 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_WIDGETNEWS_H
#define UI_WIDGETNEWS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QProgressBar>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTextEdit>
#include <QtGui/QToolButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_WidgetNews
{
public:
    QHBoxLayout *hboxLayout;
    QSplitter *splitter_2;
    QWidget *widget;
    QVBoxLayout *vboxLayout;
    QTreeWidget *listGroups;
    QHBoxLayout *hboxLayout1;
    QToolButton *fBtnRefresh;
    QProgressBar *fProc;
    QSpacerItem *spacerItem;
    QToolButton *fBtnDelete;
    QToolButton *fBtnPost;
    QSplitter *splitter;
    QTreeWidget *listArticles;
    QTextEdit *fNews;

    void setupUi(QWidget *WidgetNews)
    {
    if (WidgetNews->objectName().isEmpty())
        WidgetNews->setObjectName(QString::fromUtf8("WidgetNews"));
    WidgetNews->resize(684, 492);
    hboxLayout = new QHBoxLayout(WidgetNews);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    splitter_2 = new QSplitter(WidgetNews);
    splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
    splitter_2->setOrientation(Qt::Horizontal);
    widget = new QWidget(splitter_2);
    widget->setObjectName(QString::fromUtf8("widget"));
    vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    listGroups = new QTreeWidget(widget);
    listGroups->setObjectName(QString::fromUtf8("listGroups"));

    vboxLayout->addWidget(listGroups);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    fBtnRefresh = new QToolButton(widget);
    fBtnRefresh->setObjectName(QString::fromUtf8("fBtnRefresh"));

    hboxLayout1->addWidget(fBtnRefresh);

    fProc = new QProgressBar(widget);
    fProc->setObjectName(QString::fromUtf8("fProc"));
    fProc->setMaximum(0);
    fProc->setValue(-1);

    hboxLayout1->addWidget(fProc);

    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout1->addItem(spacerItem);

    fBtnDelete = new QToolButton(widget);
    fBtnDelete->setObjectName(QString::fromUtf8("fBtnDelete"));

    hboxLayout1->addWidget(fBtnDelete);

    fBtnPost = new QToolButton(widget);
    fBtnPost->setObjectName(QString::fromUtf8("fBtnPost"));

    hboxLayout1->addWidget(fBtnPost);


    vboxLayout->addLayout(hboxLayout1);

    splitter_2->addWidget(widget);
    splitter = new QSplitter(splitter_2);
    splitter->setObjectName(QString::fromUtf8("splitter"));
    splitter->setOrientation(Qt::Vertical);
    listArticles = new QTreeWidget(splitter);
    listArticles->setObjectName(QString::fromUtf8("listArticles"));
    splitter->addWidget(listArticles);
    fNews = new QTextEdit(splitter);
    fNews->setObjectName(QString::fromUtf8("fNews"));
    QFont font;
    font.setFamily(QString::fromUtf8("Monospace"));
    fNews->setFont(font);
    fNews->setReadOnly(true);
    splitter->addWidget(fNews);
    splitter_2->addWidget(splitter);

    hboxLayout->addWidget(splitter_2);


    retranslateUi(WidgetNews);

    QMetaObject::connectSlotsByName(WidgetNews);
    } // setupUi

    void retranslateUi(QWidget *WidgetNews)
    {
    WidgetNews->setWindowTitle(QApplication::translate("WidgetNews", "News", 0, QApplication::UnicodeUTF8));
    listGroups->headerItem()->setText(0, QApplication::translate("WidgetNews", "Newsgroup", 0, QApplication::UnicodeUTF8));
    listGroups->headerItem()->setText(1, QApplication::translate("WidgetNews", "Articles", 0, QApplication::UnicodeUTF8));
    fBtnRefresh->setText(QApplication::translate("WidgetNews", "Refresh", 0, QApplication::UnicodeUTF8));
    fBtnDelete->setText(QApplication::translate("WidgetNews", "Delete", 0, QApplication::UnicodeUTF8));
    fBtnPost->setText(QApplication::translate("WidgetNews", "Post...", 0, QApplication::UnicodeUTF8));
    listArticles->headerItem()->setText(0, QApplication::translate("WidgetNews", "Subject", 0, QApplication::UnicodeUTF8));
    listArticles->headerItem()->setText(1, QApplication::translate("WidgetNews", "Date", 0, QApplication::UnicodeUTF8));
    listArticles->headerItem()->setText(2, QApplication::translate("WidgetNews", "Author", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(WidgetNews);
    } // retranslateUi

};

namespace Ui {
    class WidgetNews: public Ui_WidgetNews {};
} // namespace Ui

#endif // UI_WIDGETNEWS_H
