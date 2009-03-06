#ifndef QWCEVENTFILTER_H
#define QWCEVENTFILTER_H

#include <QObject>
#include "QwcSocket.h"

class QwcEventFilter : public QObject
{
    Q_OBJECT

public:
   QwcEventFilter(QObject *parent=0);
   QwcSocket *pWiredSocket;
   void setSocket(QwcSocket *s);
   bool disconnectionPrompt(QEvent *event=0);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

};

#endif
