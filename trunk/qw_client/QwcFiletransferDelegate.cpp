
#include "QwcFiletransferDelegate.h"
#include "QwcTransfer.h"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QProgressBar>

QwcFiletransferDelegate::QwcFiletransferDelegate(QObject *parent) :
        QAbstractItemDelegate(parent)
{
}


QSize QwcFiletransferDelegate::sizeHint(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 52);
}


void QwcFiletransferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    if (!index.isValid()) { return; }

    painter->save();

    // Draw bottom line
    QPen pen(painter->pen());
    pen.setColor(Qt::gray);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

    if (option.state & QStyle::State_Selected) {
        // Selected items
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    }

    painter->translate(option.rect.topLeft());

    QwcTransfer *transfer = index.data(Qt::UserRole).value<QwcTransfer*>();
    if (!transfer) { return; }

    painter->setPen(Qt::black);

    // Status Text
    QString statusText;
    QPixmap statusIcon;

    // Default icon
    if (transfer->type() == Qwc::TransferTypeFileDownload
        || transfer->type() == Qwc::TransferTypeFolderDownload) {
        statusIcon = QPixmap(":/icons/16x16/go-down.png");
    } else {
        statusIcon = QPixmap(":/icons/16x16/go-up.png");
    }

    if (transfer->state() == Qwc::TransferStateRunning) {
        QString currentSpeedText("-");
        if (transfer->currentTransferSpeed() > -1) {
            currentSpeedText = QwFile::humanReadableSize(transfer->currentTransferSpeed());
        }
        statusText = tr("Active (%1 of %2 at %3/s)")
                     .arg(QwFile::humanReadableSize(transfer->completedTransferSize()))
                     .arg(QwFile::humanReadableSize(transfer->totalTransferSize()))
                     .arg(currentSpeedText);

    } else if (transfer->state() == Qwc::TransferStateComplete) {
        statusText = tr("Complete (%1)").arg(QwFile::humanReadableSize(transfer->totalTransferSize()));
        statusIcon = QPixmap(":/icons/emotes/face-smile.png");

    } else if (transfer->state() == Qwc::TransferStateError) {
        statusText = tr("Error");
        statusIcon = QPixmap(":/icons/emotes/face-sad.png");

    } else if (transfer->state() == Qwc::TransferStateQueuedOnClient) {
        statusText = tr("Queued Locally");

    } else if (transfer->state() == Qwc::TransferStatePaused) {
        statusText = tr("Paused");
        statusIcon = QPixmap(":/icons/16x16/media-playback-stop.png");

    } else if (transfer->state() == Qwc::TransferStateQueuedOnServer) {
        statusText = tr("Queued on Server (Position %1)").arg(transfer->serverQueuePosition());
    }

    QFont currentFont(painter->font());
    const int cellPadding = 4;
    painter->translate(cellPadding, cellPadding);

    // Icon
    if (!statusIcon.isNull()) {
        painter->drawPixmap(0, statusIcon.height() / 3, statusIcon);
        painter->translate(statusIcon.width() + cellPadding, 0);
    }

    // File Name
    currentFont.setPixelSize(12);
    currentFont.setBold(true);
    painter->setFont(currentFont);
    painter->drawText(QPoint(0, painter->fontMetrics().ascent()),
                      transfer->remotePath().section("/", -1, -1, QString::SectionSkipEmpty));
    painter->translate(0, painter->fontMetrics().height());


    // Status Bar
    currentFont.setPixelSize(currentFont.pixelSize() * 0.8);
    currentFont.setBold(false);
    painter->setFont(currentFont);
    painter->drawText(QPoint(0, painter->fontMetrics().ascent()), statusText);
    painter->translate(0, painter->fontMetrics().height());

    // Progress Bar
    QStyleOptionProgressBarV2 barOption;
    barOption.state = QStyle::State_Enabled | QStyle::State_Active | QStyle::State_Mini;
    barOption.direction = QApplication::layoutDirection();
    barOption.minimum = 0;
    barOption.maximum = 100;
    barOption.rect.setLeft(0);
    barOption.rect.setTop(2);
    barOption.rect.setHeight(12);
    barOption.rect.setWidth(option.rect.width() - painter->matrix().dx() - 9);

     if (transfer->state() == Qwc::TransferStateRunning && transfer->totalTransferSize() > 0) {
        barOption.progress = (qreal(transfer->completedTransferSize()) / qreal(transfer->totalTransferSize())) * 100;
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &barOption, painter);

    }


    painter->restore();

}

