/***************************************************************************
 * QWired - a free Wired client.
 * Copyright (C) 2008 Bastian Bense
 ***************************************************************************
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                   *
 ***************************************************************************/

 
#ifndef WIDGETFILEBROWSER_H
#define WIDGETFILEBROWSER_H

//#include <QWidget>
// #include <QtCore>
// #include <QtGui>
#include "ui_WidgetFileBrowser.h"
#include "modelfilelist.h"
#include "../wired/classwiredtransfer.h"

class ClassWiredSession;

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetFileBrowser
	: public QWidget, private Ui::WidgetFileBrowser
{
	
Q_OBJECT
		
public:
    WidgetFileBrowser(QWidget *parent = 0);
    ~WidgetFileBrowser();
   
    void initWithConnection(ClassWiredSession *theSession);
    void setPath(QString thePath);
    QPointer<ModelFileList> pModel;
	QPointer<QSortFilterProxyModel> pFilterProxy;
    QPointer<ClassWiredSession> pSession;

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private slots:
	void fileTransferDone(ClassWiredTransfer);
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
