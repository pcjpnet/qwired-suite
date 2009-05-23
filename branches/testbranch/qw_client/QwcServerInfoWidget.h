#ifndef WIDGETSERVERINFORMATION_H
#define WIDGETSERVERINFORMATION_H

#include "QwcSocket.h"
#include "ui_QwcServerInfoWidget.h"
#include <QWidget>


class QwcServerInfoWidget : public QWidget, public Ui_QwcServerInfoWidget
{
    Q_OBJECT

public:
    QwcServerInfoWidget(QWidget *parent = 0);
    void loadInfo(QwcSocket *);

};

#endif
