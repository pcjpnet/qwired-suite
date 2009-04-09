#ifndef QWCFILETRANSFERWIDGET_H
#define QWCFILETRANSFERWIDGET_H



#include "ui_QwcFiletransferWidget.h"
#include "QwcFiletransferInfo.h"
#include <QWidget>


class QwcFiletransferWidget : public QWidget, public Ui_QwcFiletransferWidget
{
	
Q_OBJECT
		
public:
    QwcFiletransferWidget(QWidget *parent = 0);
    ~QwcFiletransferWidget();
	void init();

signals:
    void transferCancelled(QwcFiletransferInfo);
	
private slots:
	void on_fBtnCancel_clicked(bool);
	void on_fBtnReveal_clicked(bool);
	void transferListSelectionChanged(const QItemSelection &, const QItemSelection &);

};

#endif
