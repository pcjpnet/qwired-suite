#ifndef QWCFILETRANSFERDELEGATE_H
#define QWCFILETRANSFERDELEGATE_H

#include <QAbstractItemDelegate>
#include <QPainter>
#include <QApplication>

class QwcFiletransferDelegate : public QAbstractItemDelegate
{

Q_OBJECT

public:
    QwcFiletransferDelegate(QObject *parent = 0);
    ~QwcFiletransferDelegate();

    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

#endif
