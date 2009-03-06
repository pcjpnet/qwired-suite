#ifndef SENDPRIVATEMESSAGEWIDGET_H
#define SENDPRIVATEMESSAGEWIDGET_H

#include <QWidget>
#include "ui_SendPrivateMessageWidget.h"
#include "QwcUserInfo.h"
//#include "wsessionmanager.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class SendPrivateMessageWidget : public QWidget, public Ui_SendPrivateMessageWidget
{
		Q_OBJECT
	public:
// 		void setTarget ( QwcSession *theSession, int theUserID );
// 		QPointer<QwcSession> pSession;
        SendPrivateMessageWidget ( QWidget *parent = 0 );
        ~SendPrivateMessageWidget();
		int pTargetID;
		void addText(QString theText, int theWho); // theWho: 0=me, 1=them

	signals:
		void newMessage(int theUser, QString theText);
		
	private slots:
		void on_fInput_returnPressed();
// 		void on_fBtnSend_clicked();
// 		void on_fBtnCancel_clicked();
		
		
	private:
        QwcUserInfo pUser;
// 		bool eventFilter(QObject *obj, QEvent *event);

};

#endif
