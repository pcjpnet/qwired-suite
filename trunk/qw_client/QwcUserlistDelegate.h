#ifndef QWCUSERLISTDELEGATE_H
#define QWCUSERLISTDELEGATE_H

#include <QItemDelegate>
#include <QCache>


class QwcUserlistDelegate : public QItemDelegate
{

    Q_OBJECT

public:
    QwcUserlistDelegate(QObject *parent = 0);
    ~QwcUserlistDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QFont pListFont;
    bool pCompactMode;
    bool pAlternateRowBg;
    qreal backgroundOpacity;

private slots:
    void reloadPreferences();

};

#endif