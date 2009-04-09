#ifndef DIALOGNEWSPOST_H
#define DIALOGNEWSPOST_H

#include <QDialog>
#include "ui_QwcNewsPostWidget.h"


class QwcNewsPostWidget : public QWidget, private Ui_QwcNewsPostWidget
{
	Q_OBJECT

	public:

        QwcNewsPostWidget(QWidget *parent = 0 )
		{
			Q_UNUSED(parent)
			setAttribute(Qt::WA_DeleteOnClose);
			setupUi(this);
                        connect(fBtnPost,SIGNAL(released()),this,SLOT(sendButtonClicked()));
                        connect(fBtnCancel,SIGNAL(released()),this,SLOT(cancelButtonClicked()));
		};

		
        ~QwcNewsPostWidget()
		{ };

		
		QString getPostText()
		{
			return fNews->toPlainText();
		};

        private slots:
                void sendButtonClicked()
                {
                    emit accepted();
                }

                void cancelButtonClicked()
                {
                    emit rejected();
                }

        signals:
                void accepted();
                void rejected();

};


#endif
