#ifndef WIDGETSERVERINFORMATION_H
#define WIDGETSERVERINFORMATION_H


#include "ui_QwcServerInfoWidget.h"
#include "QwcSocket.h"

#include <QtGui/QWidget>


class QwcServerInfoWidget :
        public QWidget,
        protected Ui_QwcServerInfoWidget
{
    Q_OBJECT

public:
    QwcServerInfoWidget(QWidget *parent = 0);
    void setInformationFromSocket(const QwcSocket *socket);

};

#endif
