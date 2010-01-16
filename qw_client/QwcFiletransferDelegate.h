#ifndef QWCFILETRANSFERDELEGATE_H
#define QWCFILETRANSFERDELEGATE_H

#include <QAbstractItemDelegate>

class QwcFiletransferDelegate :
        public QAbstractItemDelegate
{
    Q_OBJECT

public:
    QwcFiletransferDelegate(QObject *parent = 0);
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif
