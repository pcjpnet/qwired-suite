#include "QwcEventFilter.h"

#include <QEvent>
#include <QMessageBox>

QwcEventFilter::QwcEventFilter(QObject *parent) : QObject(parent)
{

}

void QwcEventFilter::setSocket(QwcSocket *s)
{
    socket = s;
}


bool QwcEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Close
       && socket->pSocket->state() == QAbstractSocket::ConnectedState
       && socket->serverInfo.name != "") {
        if(QwcEventFilter::disconnectionPrompt(event)) {
            return true;
        }
    }

    // Normal course of action
    return QObject::eventFilter(obj, event);
}


bool QwcEventFilter::disconnectionPrompt(QEvent *event)
{
    if(QMessageBox::question(0, tr("Disconnect"), tr("Are you sure you want to continue? If you disconnect from \"%1\", any ongoing transfers will be cancelled.\n")
        .arg(socket->serverInfo.name),
        QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
        if(event) {
            event->ignore();
        }
        return true;
    } else {
        return false;
    }
}
