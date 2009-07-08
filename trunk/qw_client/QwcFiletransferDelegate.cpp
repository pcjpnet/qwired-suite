#include "QwcFiletransferDelegate.h"
#include "QwcTransferInfo.h"
#include "QwcFileInfo.h"

#include <QFont>
#include <QPainter>
#include <QDebug>
#include <QApplication>

QwcFiletransferDelegate::QwcFiletransferDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{
}

QSize QwcFiletransferDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 50);
}

void QwcFiletransferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) { return; }
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
    }

    if (!index.data(Qt::UserRole).canConvert<QwcTransferInfo>()) { return; }
    QwcTransferInfo transfer = index.data(Qt::UserRole).value<QwcTransferInfo>();

    painter->save();
    painter->translate(option.rect.topLeft());

    // Status Text
    QString statusText;
    QPixmap icon;

    if (transfer.state == Qw::TransferInfoStateNone) {
        statusText = tr("Locally queued");
        icon = QPixmap(":/icons/32x32/start-here.png");

    } else if (transfer.state == Qw::TransferInfoStateWaiting) {
        statusText = tr("Waiting for server");
        icon = QPixmap(":/icons/32x32/network-server.png");

    } else if (transfer.state == Qw::TransferInfoStatePaused) {
        statusText = tr("Stopped");
        icon = QPixmap(":/icons/32x32/media-playback-pause.png");

    } else if (transfer.state == Qw::TransferInfoStateQueued) {
        statusText = tr("Remotely queued (position %1)").arg(transfer.queuePosition);
        icon = QPixmap(":icons/32x32/start-here.png");

    } else if (transfer.state == Qw::TransferInfoStateActive) {
        statusText = tr("Active - %1 of %2 (%3%) - %4/s")
                     .arg(QwFile::humanReadableSize(transfer.bytesTransferred))
                     .arg(QwFile::humanReadableSize(transfer.file.size))
                     .arg(double(double(transfer.bytesTransferred)/double(transfer.file.size)*100), 0, 'f', 2)
                     .arg(QwFile::humanReadableSize(transfer.currentTransferSpeed));

        // Icon
        if (transfer.type == Qw::TransferTypeDownload || transfer.type == Qw::TransferTypeFolderDownload) {
            icon = QPixmap(":icons/32x32/go-down.png");
        } else if (transfer.type == Qw::TransferTypeUpload || transfer.type == Qw::TransferTypeFolderUpload) {
            icon = QPixmap(":icons/32x32/go-up.png");
        }
    }

    // Add a small folder icon when we are having a folder transfer
    if (transfer.type == Qw::TransferTypeFolderDownload || transfer.type == Qw::TransferTypeFolderUpload) {
        QPainter painter(&icon);
        painter.drawPixmap(32-16, 32-16, QPixmap(":icons/files/files-folder.png"));
        painter.end();
    }
    painter->drawPixmap(9, 9, icon);

    QFont currentFont(painter->font());

    painter->translate(50, 0);

    // File Name
    currentFont.setPixelSize(12);
    currentFont.setBold(true);
    painter->setFont(currentFont);
    painter->drawText(0, painter->fontMetrics().ascent()+1, transfer.file.fileName());

    // Status Bar
    currentFont.setPixelSize(9);
    currentFont.setBold(false);
    painter->setFont(currentFont);
    painter->translate(0, 18);
    painter->drawText(0, painter->fontMetrics().ascent()-1, statusText);


    // Progress Bar
    if (transfer.state == Qw::TransferInfoStateActive) {
        QStyleOptionProgressBarV2 barOption;
        barOption.state = QStyle::State_Enabled;
        barOption.direction = QApplication::layoutDirection();
        barOption.minimum = 0;
        barOption.maximum = 100;
        barOption.progress = int(double(transfer.bytesTransferred)/double(transfer.file.size)*100);
        barOption.rect.setLeft(0);
        barOption.rect.setTop(0);
        barOption.rect.setHeight(16);
        barOption.rect.setWidth(option.rect.width()-painter->matrix().dx()-9);
        painter->translate(0, 12);
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &barOption, painter);

    }
    painter->restore();

    // Draw bottom line
    painter->save();
    QPen pen(painter->pen());
    pen.setColor(Qt::gray);
    pen.setWidth(1);
    painter->setPen(pen);
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());
    painter->restore();

}

