#ifndef QWCUSERLISTDELEGATE_H
#define QWCUSERLISTDELEGATE_H

#include <QtGui/QItemDelegate>

class QwcSocket;

class QwcUserlistDelegate :
        public QItemDelegate
{
    Q_OBJECT

public:
    QwcUserlistDelegate(QObject *parent = 0);
    ~QwcUserlistDelegate();

    void setSocket(QwcSocket *socket);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QFont m_listFont;
    bool m_compactMode;
    qreal m_opacity;
    QwcSocket *m_socket;

private slots:
    void reloadPreferences();

};

#endif
