#ifndef QWMMONITORWINDOW_H
#define QWMMONITORWINDOW_H

#include "ui_QwmMonitorWindow.h"

class QwmMonitorWindow : public QWidget, public Ui_QwmMonitorWindow
{
    Q_OBJECT

public:
    QwmMonitorWindow(QWidget *parent = 0);

private slots:
    //void on_myButton_clicked();
    void on_fTransfersList_currentRowChanged(int index);

};

#endif // QwmMonitorWindow_H
