#include "ClassWiredEventFilter.h"
#include <QEvent>
#include <QMessageBox>

void ClassWiredEventFilter::setSocket(WiredSocket *s)
{
    pWiredSocket = s;
}

bool ClassWiredEventFilter::eventFilter(QObject *obj, QEvent *event)
 {
     if(event->type() == QEvent::Close && pWiredSocket->pSocket->state() == QAbstractSocket::ConnectedState && pWiredSocket->pServerName != "") {
         if(ClassWiredEventFilter::disconnectionPrompt(event)) {
             return true;
         }
     }

     // Normal course of action
     return QObject::eventFilter(obj, event);
 }

bool ClassWiredEventFilter::disconnectionPrompt(QEvent *event) {
    if(QMessageBox::question(0, tr("Disconnect"),tr("Are you sure you want to continue? If you disconnect from \"%1\", any ongoing transfers will be cancelled.\n").arg(pWiredSocket->pServerName),QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
        if(event) {
            event->ignore();
        }
        return true;
    } else {
        return false;
    }
}
