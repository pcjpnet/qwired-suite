#ifndef WIDGETUSERINFO_H
#define WIDGETUSERINFO_H

#include <QWidget>
#include "ui_QwcUserInfoWidget.h"
#include "QwcUserInfo.h"

/**
        @author Bastian Bense <bastibense@gmail.com>
*/

class QwcUserInfoWidget :
        public QWidget,
        protected Ui_QwcUserInfoWidget
{
    Q_OBJECT

public:
    QwcUserInfoWidget(QWidget *parent = 0);
    ~QwcUserInfoWidget();
    void setUser(QwcUserInfo theUser);
    void loadTransfers(const QwcUserInfo &theUser);
    int pUserID;

};

#endif
