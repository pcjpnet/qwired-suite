#ifndef SERVERMONITORWINDOW_H
#define SERVERMONITORWINDOW_H

#include "ui_ServerMonitorWindow.h"

class ServerMonitorWindow : public QWidget, private Ui::ServerMonitorWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerMonitorWindow)

public:
    explicit ServerMonitorWindow(QWidget *parent = 0);

};

#endif // SERVERMONITORWINDOW_H
