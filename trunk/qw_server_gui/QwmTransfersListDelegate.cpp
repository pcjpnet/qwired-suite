#include "QwmTransfersListDelegate.h"

#include <QPainter>

QwmTransfersListDelegate::QwmTransfersListDelegate(QObject *parent) : QItemDelegate(parent)
{

}


QSize QwmTransfersListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(400, 40);
}


void QwmTransfersListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->drawRect(option.rect);
}
