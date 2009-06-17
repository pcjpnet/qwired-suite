#include "QwcFiletransferWidget.h"
#include "QwcFiletransferDelegate.h"

#include <QDesktopServices>
#include <QtDebug>
#include <QUrl>
#include <QFileInfo>
#include <QDir>

QwcFiletransferWidget::QwcFiletransferWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    transferList->setItemDelegate(new QwcFiletransferDelegate(this));


}


QwcFiletransferWidget::~QwcFiletransferWidget()
{
}


void QwcFiletransferWidget::on_btnResume_clicked()
{
    QModelIndexList selectedIndexes = transferList->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndexes);
    while (i.hasNext()) {
        QModelIndex item = i.next();
        if (!item.isValid()) { continue; }
        if (!item.data(Qt::UserRole).canConvert<QwcTransferInfo>()) { return; }
        QwcTransferInfo transfer = item.data(Qt::UserRole).value<QwcTransferInfo>();
        emit transferResumed(transfer);
    }

}


/*! Pause/Stop a transfer. */
void QwcFiletransferWidget::on_btnStop_clicked()
{
    QModelIndexList selectedIndexes = transferList->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndexes);
    while (i.hasNext()) {
        QModelIndex item = i.next();
        if (!item.isValid()) { continue; }
        if (!item.data(Qt::UserRole).canConvert<QwcTransferInfo>()) { return; }
        QwcTransferInfo transfer = item.data(Qt::UserRole).value<QwcTransferInfo>();
        emit transferStopped(transfer);
    }
}


/*! Reveal a file/directory on the user's desktop system.
*/
void QwcFiletransferWidget::on_btnReveal_clicked()
{
    QModelIndexList selectedIndexes = transferList->selectionModel()->selectedIndexes();
    QListIterator<QModelIndex> i(selectedIndexes);
    while (i.hasNext()) {
        QModelIndex item = i.next();
        if (!item.isValid()) { continue; }
        if (!item.data(Qt::UserRole).canConvert<QwcTransferInfo>()) { return; }
        QwcTransferInfo transfer = item.data(Qt::UserRole).value<QwcTransferInfo>();
        QFileInfo targetFile(transfer.file.localAbsolutePath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(targetFile.dir().absolutePath()));
    }
}


void QwcFiletransferWidget::handleTransferListSelectionChanged(const QItemSelection &previous, const QItemSelection &current)
{
    Q_UNUSED(current);
    Q_UNUSED(previous);
    btnResume->setEnabled(transferList->selectionModel()->hasSelection());
    btnStop->setEnabled(transferList->selectionModel()->hasSelection());
    btnReveal->setEnabled(transferList->selectionModel()->hasSelection());
}


void QwcFiletransferWidget::init()
{
    connect(transferList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleTransferListSelectionChanged(QItemSelection,QItemSelection)) );
}
