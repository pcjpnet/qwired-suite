#include "QwmMonitorWindow.h"

#include "QwTransferInfo.h"
#include "QwUser.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>

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


void QwmMonitorWindow::on_configurationTrackersList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    btnConfigurationTrackersRemove->setEnabled(current != NULL);
}


void QwmMonitorWindow::on_btnConfigurationTrackersAdd_clicked()
{
    QString newUrl = QInputDialog::getText(this, tr("Enter Tracker URL"),
                     tr("Enter the URL of a tracker. Tracker URLs take the form of \"tracker://some.domain.com\"."));
    if (newUrl.isEmpty()) { return; }
    QListWidgetItem *item = new QListWidgetItem(QIcon(":/icons/icn_tracker.png"),
                                                newUrl, configurationTrackersList);
    handleTrackersChanged();
}


void QwmMonitorWindow::on_btnConfigurationTrackersRemove_clicked()
{
    if (!configurationTrackersList->currentItem()) { return; }
    configurationTrackersList->takeItem(configurationTrackersList->currentIndex().row());
    handleTrackersChanged();
}


/*! Generate a new list of tracker URLs.
*/
void QwmMonitorWindow::handleTrackersChanged()
{
    QStringList trackerUrls;
    for (int i = 0; i < configurationTrackersList->count(); i++) {
        QListWidgetItem *item = configurationTrackersList->item(i);
        if (!item) { continue; }
        trackerUrls << item->text();
    }
    emit updatedTrackerList(trackerUrls);
}


void QwmMonitorWindow::on_btnTransfersDisconnectUser_clicked()
{
    QTreeWidgetItem *item = fUsersList->currentItem();
    if (!item) { return; }
    if (!item->data(0, Qt::UserRole).canConvert<QwUser>()) { return; }
    QwUser info = item->data(0, Qt::UserRole).value<QwUser>();
    emit requestedUserKick(info.pUserID);
}



