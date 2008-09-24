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

 
#include "widgetfilebrowser.h"
#include "general/classwiredsession.h"

#include <math.h>

WidgetFileBrowser::WidgetFileBrowser(QWidget *parent)
	: QWidget(parent)
{
	setupUi(this);
	setAcceptDrops(true);
	setAttribute(Qt::WA_DeleteOnClose);
}


WidgetFileBrowser::~WidgetFileBrowser()
{ }


void WidgetFileBrowser::initWithConnection(ClassWiredSession *theSession)
{
	pModel = new ModelFileList(this);
	pFilterProxy = new QSortFilterProxyModel(this);
	pFilterProxy->setSourceModel(pModel);
	pFilterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	pFilterProxy->setSortRole(Qt::UserRole);
	
	pSession = theSession;
	connect( pSession->wiredSocket(), SIGNAL(onFilesListItem(ClassWiredFile)),
			 pModel, SLOT(onServerFileListItem(ClassWiredFile)) );
	connect( pSession->wiredSocket(), SIGNAL(onFilesListDone(QString,qlonglong)),
			 pModel, SLOT(onServerFileListDone(QString,qlonglong)) );
	connect( pSession->wiredSocket(), SIGNAL(onFilesListDone(QString,qlonglong)),
			 this, SLOT(doUpdateBrowserStats(QString,qlonglong)) );
	connect( pSession->wiredSocket(), SIGNAL(fileTransferDone(ClassWiredTransfer)),
			 this, SLOT(fileTransferDone(ClassWiredTransfer)) );

	fList->setModel(pFilterProxy);
	fList->setAlternatingRowColors(true);
	
	fBtnNewFolder->setEnabled( pSession->wiredSocket()->sessionUser.privCreateFolders );
}


void WidgetFileBrowser::on_fFilter_textEdited(QString theFilter)
{
	pFilterProxy->setFilterWildcard(theFilter);
}


// Item has been double-clicked; if it is a folder, browse it
void WidgetFileBrowser::on_fList_doubleClicked(const QModelIndex &index)
{
	if(pModel!=0 and pSession!=0) {
		ClassWiredFile tmpFile = index.data(Qt::UserRole+1).value<ClassWiredFile>();
		
		if( tmpFile.type == 0 ) { // Files
			if(!pSession->wiredSocket()->sessionUser.privDownload) return;
			downloadFile(tmpFile.path);
					
		} else if(tmpFile.type>0) { // Directories
			pFilterProxy->setFilterWildcard("");
			fFilter->setText("");
			pModel->clearList();
			pModel->pWaitingForList = true;
			pModel->pCurrentPath = tmpFile.path; // Load path from data
			pSession->wiredSocket()->getFileList(tmpFile.path);
			fBtnBack->setEnabled( tmpFile.path!="/" );
		}
	}
}


void WidgetFileBrowser::setPath(QString thePath)
{
	if(pModel)
		pModel->pCurrentPath = thePath;
}


// Navigate back
void WidgetFileBrowser::on_fBtnBack_clicked(bool)
{
	QString tmpPath = pModel->pCurrentPath.section("/",0,-2);
	if(tmpPath.isEmpty()) tmpPath="/";
	pModel->clearList();
	pModel->pWaitingForList = true;
	pModel->pCurrentPath = tmpPath;
	pFilterProxy->setFilterWildcard("");
	fFilter->setText("");
	pSession->wiredSocket()->getFileList(tmpPath);
	fBtnBack->setEnabled(tmpPath!="/");
}


void WidgetFileBrowser::on_fBtnInfo_clicked(bool)
{
	QModelIndex tmpIdx = fList->currentIndex();
	if(tmpIdx.isValid()) {
		ClassWiredFile tmpFile = tmpIdx.data(Qt::UserRole+1).value<ClassWiredFile>();
		pSession->wiredSocket()->statFile(tmpFile.path);
		fBtnInfo->setEnabled(false);
	}
}


// Update the stats in the browser
void WidgetFileBrowser::doUpdateBrowserStats(QString thePath, qlonglong theFree)
{
	Q_UNUSED(theFree)
	if( thePath == pModel->pCurrentPath ) {
		QString tmpStr;
		tmpStr = tr("%1 items, %2 total, %3 available")
				.arg(pModel->rowCount())
				.arg( ClassWiredFile::humanReadableSize(pModel->pTotalSize) )
				.arg( ClassWiredFile::humanReadableSize(pModel->pFreeSize) );
		
		if(thePath!="/")
				setWindowTitle(tr("Files - %1").arg(pModel->pCurrentPath.section("/",-1)));
		else	setWindowTitle(tr("Files - %1").arg(pModel->pCurrentPath));
		
		fStats->setText(tmpStr);
		fList->resizeColumnToContents(0);
		fList->resizeColumnToContents(1);
		fList->resizeColumnToContents(2);
	}
}


void WidgetFileBrowser::downloadFile(QString theRemotePath)
{
	QSettings settings;
	QString tmpName = theRemotePath.section("/",-1,-1);
	QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
	QFile tmpDir( tmpDownloadFolder.absoluteFilePath(tmpName) );
	if(tmpDir.exists()) {
		QMessageBox messageBox(this);
		messageBox.setWindowTitle(tr("File Exists"));
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setText(tr("The file you are trying to download already exists in your download directory. Overwrite it?") );
		QAbstractButton *rejectButton = messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
		Q_UNUSED(rejectButton)
		QAbstractButton *deleteButton = messageBox.addButton(tr("Overwrite"), QMessageBox::AcceptRole);
		messageBox.exec();
		if (messageBox.clickedButton()!=deleteButton)
			return;
	}
	pSession->wiredSocket()->getFile(theRemotePath, tmpDir.fileName());
	pSession->doActionTransfers();

}


// Download button pressed
void WidgetFileBrowser::on_fBtnDownload_clicked(bool)
{
	QModelIndex tmpIdx = fList->currentIndex();
	if(tmpIdx.isValid()) {
		ClassWiredFile tmpFile = tmpIdx.data(Qt::UserRole+1).value<ClassWiredFile>();
		if(!pSession->wiredSocket()->sessionUser.privDownload)
			return;
		downloadFile(tmpFile.path);
	}
}


// Another file has been selected
void WidgetFileBrowser::on_fList_clicked(const QModelIndex&)
{
	QModelIndex tmpIdx = fList->currentIndex();
	if(tmpIdx.isValid()) {
		ClassWiredFile tmpFile = tmpIdx.data(Qt::UserRole+1).value<ClassWiredFile>();
		fBtnDownload->setEnabled(tmpFile.type==0 && pSession->wiredSocket()->sessionUser.privDownload); // Only files can be downloaded now
		fBtnInfo->setEnabled(true);
		fBtnDelete->setEnabled( pSession->wiredSocket()->sessionUser.privDeleteFiles );
		fBtnUpload->setEnabled( pSession->wiredSocket()->sessionUser.privUpload || pSession->wiredSocket()->sessionUser.privUploadAnywhere );
		fBtnNewFolder->setEnabled( pSession->wiredSocket()->sessionUser.privCreateFolders );
		return;
	}
	
	fBtnNewFolder->setEnabled(false);
	fBtnDelete->setEnabled(false);
	fBtnDownload->setEnabled(false);
	fBtnInfo->setEnabled(false);
	fBtnUpload->setEnabled(false);
}


void WidgetFileBrowser::on_fBtnUpload_clicked(bool)
{
	QString fileName = QFileDialog::getOpenFileName(this,
			tr("Upload File"), QDir::homePath());
	if(!fileName.isEmpty()) {
		QString tmpFileName = fileName.section("/",-1,-1);
		QString tmpRemote = pModel->pCurrentPath+"/"+tmpFileName;
		pSession->wiredSocket()->putFile(fileName, tmpRemote);
		pSession->doActionTransfers();
	}
}


void WidgetFileBrowser::on_fBtnDelete_clicked(bool)
{
	QModelIndex tmpIdx = fList->currentIndex();
	if(tmpIdx.isValid()) {
		ClassWiredFile tmpFile = tmpIdx.data(Qt::UserRole+1).value<ClassWiredFile>();
		QMessageBox::StandardButton button = QMessageBox::question(this,
				tr("Delete File"),
				tr("Are you sure you want to delete the item '%1'?\nThis can not be undone!").arg(tmpFile.fileName()),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (button != QMessageBox::Yes)
			return;
		pModel->clearList();
		pModel->pWaitingForList = true;
		pSession->wiredSocket()->deleteFile(tmpFile.path);
		pSession->wiredSocket()->getFileList(pModel->pCurrentPath);
		
		this->raise();
	}
}


void WidgetFileBrowser::on_fBtnNewFolder_clicked(bool)
{
	QString tmpFolderName = QInputDialog::getText(this,
			tr("Create Folder"), tr("Enter a name for the new folder:"));
	if(!tmpFolderName.isEmpty()) {
		tmpFolderName = tmpFolderName.replace("/","_");
		pSession->wiredSocket()->createFolder( pModel->pCurrentPath+"/"+tmpFolderName );
		pModel->clearList();
		pModel->pWaitingForList = true;
		pSession->wiredSocket()->getFileList( pModel->pCurrentPath );
	}
}


//
// Drag And Drop
//

/// Check if they got something we can handle.
void WidgetFileBrowser::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasUrls())
		event->acceptProposedAction();
}


/// A file has been dropped, lets upload.
void WidgetFileBrowser::dropEvent(QDropEvent *event)
{
	QList<QUrl> tmpUrls = event->mimeData()->urls();
	QListIterator<QUrl> i(tmpUrls);
	while(i.hasNext()) {
		QUrl tmpUrl = i.next();
		QFile tmpFile(tmpUrl.toLocalFile());
		if(tmpFile.exists()) {
			QString tmpFileName = tmpFile.fileName().section("/",-1,-1);
			QString tmpRemote = pModel->pCurrentPath+"/"+tmpFileName;
			pSession->wiredSocket()->putFile(tmpFile.fileName(), tmpRemote);
			pSession->doActionTransfers();
		}
	}
	event->acceptProposedAction();
}


/// File transfer completed. Refresh the view.
void WidgetFileBrowser::fileTransferDone(ClassWiredTransfer ) {
	pModel->clearList();
	pModel->pWaitingForList = true;
	pSession->wiredSocket()->getFileList( pModel->pCurrentPath );
}






