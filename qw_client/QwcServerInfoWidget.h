#ifndef WIDGETSERVERINFORMATION_H
#define WIDGETSERVERINFORMATION_H


#include "ui_QwcServerInfoWidget.h"
#include "QwcSocket.h"

#include <QWidget>


class QwcServerInfoWidget : public QWidget, public Ui_QwcServerInfoWidget
{
    Q_OBJECT

public:
    QwcServerInfoWidget(QWidget *parent = 0);
    void setInformationFromSocket(const QwcSocket *socket);

};

#endif
