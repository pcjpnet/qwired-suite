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

    stackedWidget->setCurrentWidget(pageBrowser);
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


/*! Set the information about a file on the info display page.
*/
void QwcFileBrowserWidget::setFileInformation(QwcFileInfo file)
{
    infoName->setText(file.fileName());
    infoIcon->setPixmap(file.fileIcon().pixmap(16,16));
    infoSize->setText(QString("%1 (%2 bytes)").arg(QwcFileInfo::humanReadableSize(file.size)).arg(file.size));
    infoPath->setText(file.path);
    infoModified->setText(file.modified.toString());
    infoCreated->setText(file.created.toString());
    infoChecksum->setText(file.checksum);
    infoComment->setText(file.comment);

    switch (file.type) {
    case Qw::FileTypeDropBox:
        infoKind->setText(tr("Drop Box"));
        break;
    case Qw::FileTypeFolder:
        infoKind->setText(tr("Folder"));
        break;
    case Qw::FileTypeRegular:
        infoKind->setText(tr("File"));
        break;
    case Qw::FileTypeUploadsFolder:
        infoKind->setText(tr("Upload Folder"));
        break;
    default:
        infoKind->setText(tr("n/a"));
    }

    pageInfo->setEnabled(true);
}


/*! Set the user information for the current session. This will enable/disable features based on the
    set of privileges the user has.
*/
void QwcFileBrowserWidget::setUserInformation(QwcUserInfo info)
{
    userInfo = info;

    // Browser page
    btnDelete->setVisible(info.privDeleteFiles);
    btnNewFolder->setVisible(info.privCreateFolders);

    // Info page
    btnInfoApply->setEnabled(info.privAlterFiles);
    infoName->setEnabled(info.privAlterFiles);
    infoComment->setReadOnly(!info.privAlterFiles);
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
    this->setEnabled(true);
    btnBack->setEnabled(remotePath != "/");
    fList->resizeColumnToContents(0);

    // Set the root information as we don't get this from previous requests.
    if (remotePath == "/") {
        currentFolderInfo = QwcFileInfo();
        currentFolderInfo.path = "/";
        currentFolderInfo.type = Qw::FileTypeFolder;
    }

    // Enable/disable upload/download as needed
    btnUpload->setEnabled(
            (userInfo.privUploadAnywhere) ||
            (currentFolderInfo.type == Qw::FileTypeUploadsFolder && userInfo.privUpload) ||
            (currentFolderInfo.type == Qw::FileTypeDropBox && userInfo.privUpload) );


    // Disable some controls if we are in "search mode"
    if (!findFilter->text().isEmpty()) {
        pageBrowser->setEnabled(true);
    }

}


/*! Handle the end of the file search results list.
*/
void QwcFileBrowserWidget::handleSearchResultListDone()
{

    if (!waitingForListItems) { return; }
    qDebug() << "DONE";
    waitingForListItems = false;
    freeRemoteSpace = 0;
    fStats->setText(tr("%1 results for \"%2\"")
                    .arg(fList->topLevelItemCount())
                    .arg(findFilter->text()));
    this->setEnabled(true);
    fList->resizeColumnToContents(0);
    currentFolderInfo = QwcFileInfo();
    btnUpload->setEnabled(false);
    btnRefresh->setEnabled(false);
    labelCurrentPath->clear();
}


/*! Request a refresh of the browser contents from the server.
*/
void QwcFileBrowserWidget::on_btnRefresh_clicked()
{
    resetForListing();
    emit requestedRefresh(remotePath);
}


/*! The "Delete" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnDelete_clicked()
{
    QList<QTreeWidgetItem*> items = fList->selectedItems();

    // Confirm first
    QMessageBox::StandardButton button = QMessageBox::question(this,
        tr("Delete Files and Folders"),
        tr("Are you sure you want to delete the selected %n item(s)?\nThis can not be undone!", "", items.count()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (button == QMessageBox::No) { return; }

    // Now delete the selected items
    QListIterator<QTreeWidgetItem*> i(items);
    while (i.hasNext()) {
        QTreeWidgetItem *item = i.next();
        QwcFileInfo itemInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
        emit requestedDelete(itemInfo.path);
    }

    // Reset the file browser
    resetForListing();
    emit requestedRefresh(remotePath);
}


/*! The user has pressed return and the server should now search for the files.
*/
void QwcFileBrowserWidget::on_findFilter_returnPressed()
{
    resetForListing();
    if (!findFilter->text().isEmpty()) {
        this->setEnabled(false);
        emit requestedFileSearch(findFilter->text());
    } else {
        emit requestedRefresh("/");
    }
}


// Download button pressed
//void QwcFileBrowserWidget::on_fBtnDownload_clicked(bool)
//{
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
//}



//void QwcFileBrowserWidget::on_fBtnUpload_clicked(bool)
//{
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
//}



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
    if (items.count() == 0) { return; }
    QTreeWidgetItem *item = items.first();
    currentFileInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
    emit requestedInformation(currentFileInfo.path);
    stackedWidget->setCurrentWidget(pageInfo);
    pageInfo->setEnabled(false);
}


/*! The "New Folder" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnNewFolder_clicked()
{
    bool ok;
    QString folderName = QInputDialog::getText(this,
        tr("Create Folder"), tr("Enter a name for the new folder:"),
        QLineEdit::Normal, QString(), &ok);

    if (!ok) { return; }
    if (folderName.isEmpty()) {
        QMessageBox::warning(this, tr("No name defined"),
            tr("Can not create a folder without a name. Please enter a name for the new folder."));
        return;
    }

    folderName = folderName.replace("/","_");
    emit requestedNewFolder(remotePath + "/" + folderName);
    resetForListing();
    emit requestedRefresh(remotePath);
}


/*! Another item was selected from the list.
*/
void QwcFileBrowserWidget::on_fList_itemSelectionChanged()
{
    btnInfo->setEnabled(fList->selectedItems().count());
    btnDelete->setEnabled(fList->selectedItems().count());

    btnDownload->setEnabled(fList->selectedItems().count() && userInfo.privDownload);
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
        currentFolderInfo = fileInfo;
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


/*! The "Cancel" button of the info page has been clicked.
*/
void QwcFileBrowserWidget::on_btnInfoCancel_clicked()
{
    stackedWidget->setCurrentWidget(pageBrowser);
}


/*! The "Apply" button of the info page has been clicked.
*/
void QwcFileBrowserWidget::on_btnInfoApply_clicked()
{
    QwcFileInfo newInfo = currentFileInfo;
    newInfo.comment = infoComment->toPlainText();

    // Replace all slashes to prevent corrupted names
    newInfo.path = currentFileInfo.directoryPath() + infoName->text().replace("/", "_");

    emit requestedPathChange(currentFileInfo, newInfo);
    resetForListing();
    emit requestedRefresh(currentFolderInfo.path);
    stackedWidget->setCurrentWidget(pageBrowser);
}



