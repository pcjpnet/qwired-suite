#ifndef SERVERMONITORWINDOW_H
#define SERVERMONITORWINDOW_H

#include "ui_ServerMonitorWindow.h"

class ServerMonitorWindow : public QWidget, public Ui_ServerMonitorWindow
{
    Q_OBJECT

public:
    ServerMonitorWindow(QWidget *parent = 0);

private slots:
    //void on_myButton_clicked();

};

#endif // SERVERMONITORWINDOW_H
