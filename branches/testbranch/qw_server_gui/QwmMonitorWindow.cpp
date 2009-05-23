#include "QwmMonitorWindow.h"

#include "QwTransferInfo.h"
#include "QwUser.h"

QwmMonitorWindow::QwmMonitorWindow(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}


void QwmMonitorWindow::on_fTransfersList_currentRowChanged(int index)
{
    btnTransfersAbort->setEnabled(index > -1);
}


void QwmMonitorWindow::on_btnTransfersAbort_clicked()
{
    if (fTransfersList->currentRow() == -1) { return; }
    QListWidgetItem *item = fTransfersList->currentItem();
    if (!item) { return; }
    if (!item->data(Qt::UserRole).canConvert<QwTransferInfo>()) { return; }
    QwTransferInfo info = item->data(Qt::UserRole).value<QwTransferInfo>();
    emit requestedTransferAbort(info.hash);
}


void QwmMonitorWindow::on_fUsersList_itemSelectionChanged()
{
    btnTransfersDisconnectUser->setEnabled(fUsersList->currentItem());
}


void QwmMonitorWindow::on_btnTransfersDisconnectUser_clicked()
{
    QTreeWidgetItem *item = fUsersList->currentItem();
    if (!item) { return; }
    if (!item->data(0, Qt::UserRole).canConvert<QwUser>()) { return; }
    QwUser info = item->data(0, Qt::UserRole).value<QwUser>();
    emit requestedUserKick(info.pUserID);
}



