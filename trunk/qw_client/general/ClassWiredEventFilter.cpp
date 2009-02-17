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

        // Issue a confirmation dialog
        QMessageBox msgBox;
        msgBox.setWindowTitle(pWiredSocket->pServerName);
        msgBox.setText(tr("Are you sure you want to disconnect?"));
        msgBox.setInformativeText(QString(tr("If you disconnect from \"%1\", any ongoing transfers will be cancelled.\n")).arg(pWiredSocket->pServerName));
        QPushButton *disconnectButton = msgBox.addButton(tr("Disconnect"), QMessageBox::ActionRole);
        msgBox.addButton(tr("Abort"), QMessageBox::ActionRole);
        msgBox.setDefaultButton(disconnectButton);
        msgBox.exec();

        // If user presses other than "Disconnect", we do nothing
        if(msgBox.clickedButton() != disconnectButton) {
            event->ignore();
            return true;
        }
     }

     // Normal course of action
     return QObject::eventFilter(obj, event);
 }
