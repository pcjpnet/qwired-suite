#ifndef QWCFILEBROWSERWIDGET_H
#define QWCFILEBROWSERWIDGET_H

#include "ui_QwcFileBrowserWidget.h"

#include "QwcFilelistModel.h"
#include "QwcFiletransferInfo.h"

class QwcSession;

class QwcFileBrowserWidget : public QWidget, private Ui_QwcFileBrowserWidget
{
	
Q_OBJECT
		
public:
    QwcFileBrowserWidget(QWidget *parent = 0);
   
    void initWithConnection(QwcSession *theSession);
    void setPath(QString thePath);
    QPointer<QwcFilelistModel> pModel;
	QPointer<QSortFilterProxyModel> pFilterProxy;
    QPointer<QwcSession> pSession;

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private slots:
    void fileTransferDone(QwcFiletransferInfo);
	void downloadFile(QString);
	
public slots:
	void doUpdateBrowserStats(QString thePath, qlonglong theFree);
    void on_fList_doubleClicked(const QModelIndex &index);
    void on_fList_clicked(const QModelIndex &index);
	void on_fBtnBack_clicked(bool checked);
	void on_fBtnDownload_clicked(bool checked);
	void on_fBtnUpload_clicked(bool checked);
	void on_fBtnDelete_clicked(bool checked);
	void on_fBtnNewFolder_clicked(bool checked);
	void on_fBtnInfo_clicked(bool checked);
	void on_fFilter_textEdited(QString);
	
    

};

#endif
