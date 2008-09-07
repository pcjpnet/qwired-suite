/********************************************************************************
** Form generated from reading ui file 'DialogNewsPost.ui'
**
** Created: Sat Jun 28 11:11:50 2008
**      by: Qt User Interface Compiler version 4.3.4
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DIALOGNEWSPOST_H
#define UI_DIALOGNEWSPOST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

class Ui_DialogNewsPost
{
public:
    QVBoxLayout *vboxLayout;
    QTextEdit *fNews;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *fBtnCancel;
    QPushButton *fBtnPost;

    void setupUi(QDialog *DialogNewsPost)
    {
    if (DialogNewsPost->objectName().isEmpty())
        DialogNewsPost->setObjectName(QString::fromUtf8("DialogNewsPost"));
    DialogNewsPost->resize(400, 300);
    vboxLayout = new QVBoxLayout(DialogNewsPost);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    fNews = new QTextEdit(DialogNewsPost);
    fNews->setObjectName(QString::fromUtf8("fNews"));
    QFont font;
    font.setPointSize(11);
    fNews->setFont(font);

    vboxLayout->addWidget(fNews);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout->addItem(spacerItem);

    fBtnCancel = new QPushButton(DialogNewsPost);
    fBtnCancel->setObjectName(QString::fromUtf8("fBtnCancel"));

    hboxLayout->addWidget(fBtnCancel);

    fBtnPost = new QPushButton(DialogNewsPost);
    fBtnPost->setObjectName(QString::fromUtf8("fBtnPost"));
    fBtnPost->setDefault(true);

    hboxLayout->addWidget(fBtnPost);


    vboxLayout->addLayout(hboxLayout);


    retranslateUi(DialogNewsPost);
    QObject::connect(fBtnPost, SIGNAL(clicked()), DialogNewsPost, SLOT(accept()));
    QObject::connect(fBtnCancel, SIGNAL(clicked()), DialogNewsPost, SLOT(reject()));

    QMetaObject::connectSlotsByName(DialogNewsPost);
    } // setupUi

    void retranslateUi(QDialog *DialogNewsPost)
    {
    DialogNewsPost->setWindowTitle(QApplication::translate("DialogNewsPost", "Post News", 0, QApplication::UnicodeUTF8));
    fBtnCancel->setText(QApplication::translate("DialogNewsPost", "Cancel", 0, QApplication::UnicodeUTF8));
    fBtnPost->setText(QApplication::translate("DialogNewsPost", "Post", 0, QApplication::UnicodeUTF8));
    fBtnPost->setShortcut(QApplication::translate("DialogNewsPost", "Enter", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(DialogNewsPost);
    } // retranslateUi

};

namespace Ui {
    class DialogNewsPost: public Ui_DialogNewsPost {};
} // namespace Ui

#endif // UI_DIALOGNEWSPOST_H
