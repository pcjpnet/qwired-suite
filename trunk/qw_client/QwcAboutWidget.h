#ifndef QWCABOUTWIDGET_H
#define QWCABOUTWIDGET_H

#include "ui_QwcAboutWidget.h"

class QwcAboutWidget :
        public QWidget,
        protected Ui_QwcAboutWidget
{
    Q_OBJECT


public:
    QwcAboutWidget(QWidget *parent=0);

private slots:
    void on_fBtnDonate_clicked();
    void on_fBtnGoToWebsite_clicked();

};

#endif
