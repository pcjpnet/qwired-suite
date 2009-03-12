#ifndef QWCUSERLISTDELEGATE_H
#define QWCUSERLISTDELEGATE_H

#include <QItemDelegate>


class QwcUserlistDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    QwcUserlistDelegate(QObject *parent = 0);
    ~QwcUserlistDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QFont pListFont;
    bool pCompactMode;
    bool pAlternateRowBg;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private slots:
    void reloadPreferences();
};

#endif
