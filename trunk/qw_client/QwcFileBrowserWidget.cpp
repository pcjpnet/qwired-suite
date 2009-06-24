#include "QwcFileBrowserWidget.h"
#include "QwcSession.h"

#include <math.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>


QwcFileBrowserWidget::QwcFileBrowserWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    setAcceptDrops(true);
    setAttribute(Qt::WA_DeleteOnClose);

    waitingForListItems = false;
    totalUsedSpace = 0;
    freeRemoteSpace = 0;
    labelCurrentPath->setText("/");
}


/*! Resets the browser to a state where the user can see that a new listing is loading. Also sets
    some internal variables that allow a proper behaviour.
*/
void QwcFileBrowserWidget::resetForListing()
{
    fList->clear();
    labelCurrentPath->setText(remotePath);
    totalUsedSpace = 0;
    freeRemoteSpace = 0;
    waitingForListItems = true;
    this->setEnabled(false);
}


//void QwcFileBrowserWidget::initWithConnection(QwcSession *theSession)
//{
//    pModel = new QwcFilelistModel(this);
//    pFilterProxy = new QSortFilterProxyModel(this);
//    pFilterProxy->setSourceModel(pModel);
//    pFilterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
//    pFilterProxy->setSortRole(Qt::UserRole);
//
//    pSession = theSession;
//    connect( pSession->wiredSocket(), SIGNAL(onFilesListItem(QwcFileInfo)), pModel, SLOT(onServerFileListItem(QwcFileInfo)) );
//    connect( pSession->wiredSocket(), SIGNAL(onFilesListDone(QString,qlonglong)), pModel, SLOT(onServerFileListDone(QString,qlonglong)) );
//    connect( pSession->wiredSocket(), SIGNAL(onFilesListDone(QString,qlonglong)), this, SLOT(doUpdateBrowserStats(QString,qlonglong)) );
//    connect( pSession->wiredSocket(), SIGNAL(fileTransferDone(QwcTransferInfo)), this, SLOT(fileTransferDone(QwcTransferInfo)) );
//
//    fList->setModel(pFilterProxy);
//    fList->setAlternatingRowColors(true);
//
//    fBtnNewFolder->setEnabled( pSession->wiredSocket()->sessionUser.privCreateFolders );
//}


void QwcFileBrowserWidget::on_fFilter_textEdited(QString theFilter)
{
//    pFilterProxy->setFilterWildcard(theFilter);
}



/*! The user clicked on a file and the download of the said file should be started.
*/
void QwcFileBrowserWidget::downloadFile(QString path)
{
//    QSettings settings;
//    QString fileName = path.section("/", -1, -1);
//    QDir localTargetFolder(settings.value("files/download_dir", QDir::homePath()).toString());
//    //QFile localTargetFile(localTargetFolder.absoluteFilePath(fileName));
//    //pSession->downloadFile(path, localTargetFile.fileName());
//    pSession->wiredSocket()->getFile(path, localTargetFolder.absoluteFilePath(fileName));
}

/*! Handle a new listing item from the server and add it to the file list.
*/
void QwcFileBrowserWidget::handleFilesListItem(QwcFileInfo item)
{
    if (!waitingForListItems) { return; }
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(fList);
    treeItem->setText(0, item.fileName());
    treeItem->setIcon(0, item.fileIcon());
    treeItem->setData(0, Qt::UserRole, QVariant::fromValue(item));

    if (item.type == Qw::FileTypeRegular) {
        treeItem->setText(1, item.humanReadableSize(item.size));
    } else {
        treeItem->setText(1, QString("%1 item(s)").arg(item.size));
    }

    treeItem->setText(2, item.modified.toLocalTime().toString());

    if (item.type == Qw::FileTypeRegular) {
        totalUsedSpace += item.size;
    }
}


/*! Handle the end of a file listing.
*/
void QwcFileBrowserWidget::handleFilesListDone(QString path, qlonglong freeSpace)
{
    if (!waitingForListItems) { return; }
    waitingForListItems = false;
    freeRemoteSpace = freeSpace;
    fStats->setText(tr("%1 items, %2 total, %3 available")
                    .arg(fList->topLevelItemCount())
                    .arg(QwFile::humanReadableSize(totalUsedSpace))
                    .arg(QwFile::humanReadableSize(freeRemoteSpace)));
    waitingForListItems = false;
    this->setEnabled(true);
    btnBack->setEnabled(remotePath != "/");
    fList->resizeColumnToContents(0);
}


/*! Request a refresh of the browser contents from the server.
*/
void QwcFileBrowserWidget::on_btnRefresh_clicked()
{
    resetForListing();
    emit requestedRefresh(remotePath);
}


// Download button pressed
void QwcFileBrowserWidget::on_fBtnDownload_clicked(bool)
{
//    if(!pSession->wiredSocket()->sessionUser.privDownload) return;
//    QListIterator<QModelIndex> i(fList->selectionModel()->selectedRows(0));
//    while(i.hasNext()) {
//        QModelIndex index = i.next();
//        if(!index.isValid()) continue;
//        QwcFileInfo tmpFile = index.data(Qt::UserRole+1).value<QwcFileInfo>();
//        if (tmpFile.type == Qw::FileTypeFolder|| tmpFile.type == Qw::FileTypeDropBox|| tmpFile.type == Qw::FileTypeUploadsFolder) {
//            // Folder download
//            QSettings settings;
//            QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
//            pSession->downloadFolder(tmpFile.path, tmpDownloadFolder.path());
//        } else { // Regular file
//            downloadFile(tmpFile.path);
//        }
//    }
//    pSession->doActionTransfers();
}



void QwcFileBrowserWidget::on_fBtnUpload_clicked(bool)
{
//    QStringList files = QFileDialog::getOpenFileNames(this, tr("Upload File"), QDir::homePath());
//    QStringListIterator i(files);
//    while (i.hasNext()) {
//        QString item = i.next();
//        QFileInfo itemInfo(item);
//        if (!itemInfo.exists() || !itemInfo.isReadable()) { continue; }
//        QString remotePath = pModel->pCurrentPath + "/" + itemInfo.fileName();
//        pSession->wiredSocket()->putFile(itemInfo.absoluteFilePath(), remotePath);
//        //pSession->uploadFile(itemInfo.absoluteFilePath(), remotePath);
//    }
//    // Display the transfer pane
//    pSession->doActionTransfers();
}


void QwcFileBrowserWidget::on_fBtnDelete_clicked(bool)
{
//    QModelIndexList list = fList->selectionModel()->selectedRows(0);
//
//    QMessageBox::StandardButton button = QMessageBox::question(this,
//                                                               tr("Delete File"), tr("Are you sure you want to delete the selected %n item(s)?\nThis can not be undone!", "", list.count()),
//                                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
//    if (button != QMessageBox::Yes) return;
//
//    QListIterator<QModelIndex> i(list);
//    while(i.hasNext()) {
//        QModelIndex index = i.next();
//        if(!index.isValid()) continue;
//        QwcFileInfo tmpFile = index.data(Qt::UserRole+1).value<QwcFileInfo>();
//        pSession->wiredSocket()->deleteFile(tmpFile.path);
//    }
//
//    // Request an updated list
//    pModel->clearList();
//    pModel->pWaitingForList = true;
//    pSession->wiredSocket()->getFileList(pModel->pCurrentPath);
}


void QwcFileBrowserWidget::on_fBtnNewFolder_clicked(bool)
{
//    QString tmpFolderName = QInputDialog::getText(this,
//                                                  tr("Create Folder"), tr("Enter a name for the new folder:"));
//    if(!tmpFolderName.isEmpty()) {
//        tmpFolderName = tmpFolderName.replace("/","_");
//        pSession->wiredSocket()->createFolder( pModel->pCurrentPath+"/"+tmpFolderName );
//        pModel->clearList();
//        pModel->pWaitingForList = true;
//        pSession->wiredSocket()->getFileList( pModel->pCurrentPath );
//    }
}


//
// Drag And Drop
//

/// Check if they got something we can handle.
//void QwcFileBrowserWidget::dragEnterEvent(QDragEnterEvent *event)
//{
//    if(event->mimeData()->hasUrls())
//        event->acceptProposedAction();
//}




/*! The "Back" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnBack_clicked()
{
    remotePath = remotePath.section("/", 0, -2);
    if (!remotePath.startsWith("/")) {
        remotePath = remotePath.prepend("/");
    }
    resetForListing();
    emit requestedRefresh(remotePath);
}


/*! The "Get Info" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnInfo_clicked()
{
    QList<QTreeWidgetItem*> items = fList->selectedItems();
    QListIterator<QTreeWidgetItem*> i(items);
    while (i.hasNext()) {
        QTreeWidgetItem *item = i.next();
        QwcFileInfo itemInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
        emit requestedInformation(itemInfo.path);
    }
}


/*! Another item was selected from the list.
*/
void QwcFileBrowserWidget::on_fList_itemSelectionChanged()
{
    btnInfo->setEnabled(fList->selectedItems().count());
}


/*! An item has been double-clicked. If it is a directory, we should descent into it.
*/
void QwcFileBrowserWidget::on_fList_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!item) { return; }
    QwcFileInfo fileInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
    if (fileInfo.type == Qw::FileTypeDropBox
        || fileInfo.type == Qw::FileTypeFolder
        || fileInfo.type == Qw::FileTypeUploadsFolder)
    {
        remotePath = fileInfo.path;
        resetForListing();
        emit requestedRefresh(remotePath);
    }

}


/// File transfer completed. Refresh the view.
void QwcFileBrowserWidget::fileTransferDone(QwcTransferInfo transfer)
{
//    if(transfer.type != Qw::TransferTypeUpload) return;
//    pModel->clearList();
//    pModel->pWaitingForList = true;
//    pSession->wiredSocket()->getFileList( pModel->pCurrentPath );
}






