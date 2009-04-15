#include "QwcFiletransferWidget.h"
#include "QwcFiletransferDelegate.h"

#include <QDesktopServices>
#include <QtDebug>
#include <QUrl>

QwcFiletransferWidget::QwcFiletransferWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    fTransfers->setItemDelegate(new QwcFiletransferDelegate(this));
}


QwcFiletransferWidget::~QwcFiletransferWidget()
{
}


void QwcFiletransferWidget::on_fBtnCancel_clicked(bool )
{
    QModelIndex idx = fTransfers->currentIndex();
    if(!idx.isValid()) return;
    QwcTransferInfo tmpT = idx.data(Qt::UserRole).value<QwcTransferInfo>();
    qDebug() << "Cancelling transfer"<<tmpT.hash<<tmpT.file.path;
    emit transferCancelled(tmpT);
    fTransfers->reset();
    fBtnCancel->setEnabled(false);
    fBtnReveal->setEnabled(false);
}

void QwcFiletransferWidget::on_fBtnReveal_clicked(bool )
{
//    if(fTransfers->selectionModel()->hasSelection()) {
//        QModelIndex item = fTransfers->currentIndex();
//        QwcTransferInfo tmpT = item.data(Qt::UserRole).value<QwcTransferInfo>();
//        QDesktopServices::openUrl(QUrl(tr("file://%1").arg(tmpT.file.localAbsolutePath.left(tmpT.pLocalPath.lastIndexOf("/")),"")));
//        qDebug() << "Revealing '" << tr("file://%1").arg(tmpT.pLocalPath) << "'...";
//    }
}


void QwcFiletransferWidget::transferListSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    fBtnCancel->setEnabled(fTransfers->selectionModel()->hasSelection());
    fBtnReveal->setEnabled(fTransfers->selectionModel()->hasSelection());
}


void QwcFiletransferWidget::init()
{
    connect(fTransfers->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(transferListSelectionChanged(const QItemSelection &, const QItemSelection &)) );
}
