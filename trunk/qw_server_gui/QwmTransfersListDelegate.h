#ifndef QWMTRANSFERSLISTDELEGATE_H
#define QWMTRANSFERSLISTDELEGATE_H

#include <QObject>
#include <QItemDelegate>

class QwmTransfersListDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    QwmTransfersListDelegate(QObject *parent=0);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // QWMTRANSFERSLISTDELEGATE_H
