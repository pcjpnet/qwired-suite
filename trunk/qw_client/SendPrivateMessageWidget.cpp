 
#include "SendPrivateMessageWidget.h"
#include <QMessageBox>
#include <QKeyEvent>

SendPrivateMessageWidget::SendPrivateMessageWidget(QWidget *parent) : QWidget ( parent )
{
// 	setAttribute(Qt::WA_DeleteOnClose);
	setupUi ( this );
	//fBtnSend->setShortcut(QKeySequence(Qt::Key_Enter));
	pTargetID = 0;
	fMsg->installEventFilter(this);
}


SendPrivateMessageWidget::~SendPrivateMessageWidget()
{
}

void SendPrivateMessageWidget::on_fInput_returnPressed() {
	emit newMessage(pTargetID, fInput->text());
	addText(fInput->text(),0);
	fInput->clear();
}



// void WidgetSendPrivMsg::setTarget ( QwcSession *theSession, int theUserID )
// {
// 	pSession = theSession;
// 	if ( pSession and pSession->wiredSocket() ) {
// 		pUser = pSession->wiredSocket()->getUserByID ( theUserID );
// 	}
// 	//fTitle->setText ( tr("Private Message To: %1").arg (pUser.pNick) );
// }

// void WidgetSendPrivMsg::on_fBtnSend_clicked()
// {
// 	if( fBtnSend->text()==tr("Dismiss") ) {
// 		this->close();
// 	} else {
// 		if( pSession and pSession->wiredSocket() ) {
// 			pSession->wiredSocket()->sendPrivateMessage(pUser.pUserID, fMsg->toPlainText());
// 			this->close();
// 		}
// 	}
// }

// void WidgetSendPrivMsg::on_fBtnCancel_clicked()
// {
// 	if( fBtnCancel->text()==tr("Reply") ) {
// 		fMsg->setReadOnly(false);
// 		fMsg->clear();
// 		fBtnSend->setText(tr("Send"));
// 		fBtnCancel->setText(tr("Cancel"));
// 		fBtnCancel->setShortcut(QKeySequence(""));
// 	} else {
// 		this->close();
// 	}
// 	
// }

// bool WidgetSendPrivMsg::eventFilter(QObject * obj, QEvent *event)
// {
// 	if (obj == fMsg) {
// 		if (event->type() == QEvent::KeyPress) {
// 			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
// 			if(keyEvent->key() == Qt::Key_Enter) {
// 				fBtnSend->click();
// 				return true;
// 			}
// 		}
// 	}
// 	return false;
// }

void SendPrivateMessageWidget::addText(QString theText, int theWho=0) {
	QColor tmpColor;
	if(theWho==0)
		 tmpColor = Qt::darkGray;
	else tmpColor = Qt::black;

// 	QString tmpHTML = QString("<div style='background-color: %1;'>%2</div><br>")
// 			.arg(tmpColor)
// 			.arg(theText);

	QTextCursor tc = fMsg->textCursor();
	tc.movePosition(QTextCursor::End);
	fMsg->setTextCursor(tc);

	fMsg->setTextColor(tmpColor);
	fMsg->append(theText);
// 	tc.insertHtml(tmpHTML);
}


