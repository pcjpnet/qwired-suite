#include "QwmMonitorWindow.h"

QwmMonitorWindow::QwmMonitorWindow(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}


void QwmMonitorWindow::on_fTransfersList_currentRowChanged(int index)
{
    btnTransfersAbort->setEnabled(index > -1);
}
