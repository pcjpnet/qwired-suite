#include "QwcFiletransferWidget.h"
#include "QwcFiletransferDelegate.h"

#include <QDesktopServices>
#include <QtDebug>
#include <QUrl>

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


void QwcFiletransferWidget::on_btnReveal_clicked()
{

}


void QwcFiletransferWidget::handleTransferListSelectionChanged(const QItemSelection &previous, const QItemSelection &current)
{
    btnResume->setEnabled(transferList->selectionModel()->hasSelection());
    btnStop->setEnabled(transferList->selectionModel()->hasSelection());
    btnReveal->setEnabled(transferList->selectionModel()->hasSelection());
}


void QwcFiletransferWidget::init()
{
    connect(transferList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleTransferListSelectionChanged(QItemSelection,QItemSelection)) );
}
