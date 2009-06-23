#include "QwmMonitorWindow.h"

#include "QwTransferInfo.h"
#include "QwUser.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

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


void QwmMonitorWindow::on_btnConfigurationSetBanner_clicked()
{
    QString selectedPath = QFileDialog::getOpenFileName(this, tr("Open Banner Image"),
                                                        QDir::homePath(), tr("PNG-Image (*.png)"));
    if (selectedPath.isEmpty()) { return; }
    QFile targetFile(selectedPath);

    if (!targetFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Unable to load banner image"),
            tr("Could not open the image file: %1").arg(targetFile.errorString()));
        return;
    }
    QByteArray imageData = targetFile.readAll();

    // Scale and display the banner
    QImage bannerImage =  QImage::fromData(imageData).scaled(QSize(200, 35), Qt::KeepAspectRatio);
    fConfigurationBanner->setPixmap(QPixmap::fromImage(bannerImage));

    // Emit a signal for the controller
    emit selectedNewBanner(bannerImage);
}


void QwmMonitorWindow::on_btnTransfersDisconnectUser_clicked()
{
    QTreeWidgetItem *item = fUsersList->currentItem();
    if (!item) { return; }
    if (!item->data(0, Qt::UserRole).canConvert<QwUser>()) { return; }
    QwUser info = item->data(0, Qt::UserRole).value<QwUser>();
    emit requestedUserKick(info.pUserID);
}



