#include "QwmTransfersListDelegate.h"

#include <QPainter>

QwmTransfersListDelegate::QwmTransfersListDelegate(QObject *parent) : QItemDelegate(parent)
{

}


QSize QwmTransfersListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(400, 40);
}


void QwmTransfersListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    painter->drawRect(option.rect);
}
