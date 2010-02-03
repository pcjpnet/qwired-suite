
#include "QwcFiletransferDelegate.h"
#include "QwcTransfer.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QApplication>

QwcFiletransferDelegate::QwcFiletransferDelegate(QObject *parent) :
        QAbstractItemDelegate(parent)
{
}


QSize QwcFiletransferDelegate::sizeHint(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 60);
}


void QwcFiletransferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    if (!index.isValid()) { return; }


    if (option.state & QStyle::State_Selected) {
        // Selected items
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    }

    painter->translate(option.rect.topLeft());

    // Draw bottom line
    QPen pen(painter->pen());
    pen.setColor(Qt::gray);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

    QwcTransfer *transfer = index.data(Qt::UserRole).value<QwcTransfer*>();
    if (!transfer) { return; }

    painter->setPen(Qt::black);

    // Status Text
    QString statusText;
    if (transfer->state() == Qwc::TransferStateActive) {
        statusText = tr("Active (%1 of %2)")
                     .arg(QwFile::humanReadableSize(transfer->completedTransferSize()))
                     .arg(QwFile::humanReadableSize(transfer->totalTransferSize()));
    } else if (transfer->state() == Qwc::TransferStateComplete) {
        statusText = tr("Complete");
    } else if (transfer->state() == Qwc::TransferStateError) {
        statusText = tr("Error");
    } else if (transfer->state() == Qwc::TransferStateInactive) {
        statusText = tr("Inactive?");
    } else if (transfer->state() == Qwc::TransferStateIndexing) {
        statusText = tr("Indexing");
    } else if (transfer->state() == Qwc::TransferStatePaused) {
        statusText = tr("Paused");
    }

    QFont currentFont(painter->font());

    const int cellPadding = 4;
    painter->translate(cellPadding, cellPadding);

    // File Name
    currentFont.setPixelSize(12);
    currentFont.setBold(true);
    painter->setFont(currentFont);
    painter->drawText(QPoint(0, painter->fontMetrics().ascent()),
                      transfer->remotePath().section("/", -1, -1, QString::SectionSkipEmpty));
    painter->translate(0, painter->fontMetrics().height());

//    if (transfer->type() == Qwc::TransferTypeFolderUpload
//        || transfer->type() == Qwc::TransferTypeFolderDownload)
//    {
//        painter->drawText(0, painter->fontMetrics().ascent()+1, QString("%1 (%2)")
//                          .arg(transfer.folder.fileName())
//                          .arg(transfer.file.fileName()));
//    } else {
//        painter->drawText(0, painter->fontMetrics().ascent()+1, transfer->transferFiles().fileName());
//    }

    // Status Bar
    currentFont.setPixelSize(currentFont.pixelSize() * 0.8);
    currentFont.setBold(false);
    painter->setFont(currentFont);
    painter->drawText(QPoint(0, painter->fontMetrics().ascent()), statusText);
    painter->translate(0, painter->fontMetrics().height());


    // Progress Bar
    QStyleOptionProgressBarV2 barOption;
    barOption.state = QStyle::State_Enabled;
    barOption.direction = QApplication::layoutDirection();
    barOption.minimum = 0;
    barOption.maximum = 100;

    if (transfer->totalTransferSize() > 0) {
        barOption.progress = (qreal(transfer->completedTransferSize()) / qreal(transfer->totalTransferSize())) * 100;
    }

    barOption.rect.setLeft(0);
    barOption.rect.setTop(0);
    barOption.rect.setHeight(16);
    barOption.rect.setWidth(option.rect.width()-painter->matrix().dx()-9);
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &barOption, painter);


}

