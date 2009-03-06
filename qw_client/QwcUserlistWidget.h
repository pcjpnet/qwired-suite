#ifndef QWCUSERLISTWIDGET_H
#define QWCUSERLISTWIDGET_H

#include <QListView>

class QwcUserlistWidget : public QListView
{

Q_OBJECT

public:
    QwcUserlistWidget(QWidget *parent = 0);
    ~QwcUserlistWidget();

    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

};

#endif
