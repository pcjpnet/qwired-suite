#include "QwcFileBrowserWidget.h"
#include "QwcSession.h"
#include "QwcSocket.h"

#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtGui/QFileIconProvider>
#include <QtCore/QSettings>
#include <math.h>

QwcFileBrowserWidget::QwcFileBrowserWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setAcceptDrops(true);
    setAttribute(Qt::WA_DeleteOnClose);

    m_socket = NULL;
    m_waitingForListItems = false;
    m_totalUsedSpace = 0;
    m_freeRemoteSpace = 0;
    labelCurrentPath->setText("/");

    m_model = new QStandardItemModel(this);
    m_model->setSortRole(Qt::UserRole + 1);

    fList->setModel(m_model);

    stackedWidget->setCurrentWidget(pageBrowser);
}


void QwcFileBrowserWidget::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }

    m_socket = socket;
    connect(m_socket, SIGNAL(onFilesListItem(QwcFileInfo)),
            this, SLOT(handleFilesListItem(QwcFileInfo)));
    connect(m_socket, SIGNAL(onFilesListDone(QString,qlonglong)),
            this, SLOT(handleFilesListDone(QString,qlonglong)));

    connect(m_socket, SIGNAL(fileSearchResultListItem(QwcFileInfo)),
            this, SLOT(handleFilesListItem(QwcFileInfo)));
    connect(m_socket, SIGNAL(fileSearchResultListDone()),
            this, SLOT(handleSearchResultListDone()));

    setRemotePath("/");
}

QwcSocket* QwcFileBrowserWidget::socket()
{ return m_socket; }

QString QwcFileBrowserWidget::remotePath() const
{ return m_remotePath; }


void QwcFileBrowserWidget::setRemotePath(const QString &path)
{
    m_remotePath = path;
    resetForListing();
    m_socket->getFileList(m_remotePath);
}


/*! Resets the browser to a state where the user can see that a new listing is loading. Also sets
    some internal variables that allow a proper behaviour.
*/
void QwcFileBrowserWidget::resetForListing()
{
    m_model->clear();
    m_model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Modified") << tr("Size"));
    labelCurrentPath->setText(m_remotePath);
    m_totalUsedSpace = 0;
    m_freeRemoteSpace = 0;
    m_waitingForListItems = true;
    this->setEnabled(false);
}


/*! Set the information about a file on the info display page.
*/
void QwcFileBrowserWidget::setFileInformation(QwcFileInfo file)
{
    infoName->setText(file.fileName());
    infoIcon->setPixmap(file.fileIcon().pixmap(16,16));
    infoSize->setText(tr("%1 (%2)").arg(QwcFileInfo::humanReadableSize(file.size)).arg(file.size));
    infoPath->setText(file.path);
    infoModified->setText(file.modified.toString());
    infoCreated->setText(file.created.toString());
    infoChecksum->setText(file.checksum);
    infoComment->setText(file.comment);

    switch (file.type) {
    case Qw::FileTypeDropBox: infoKind->setText(tr("Drop Box")); break;
    case Qw::FileTypeFolder: infoKind->setText(tr("Folder")); break;
    case Qw::FileTypeRegular: infoKind->setText(tr("File")); break;
    case Qw::FileTypeUploadsFolder: infoKind->setText(tr("Upload Folder")); break;
    default: infoKind->setText(tr("n/a")); break;
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
    btnDelete->setVisible(info.privileges().testFlag(Qws::PrivilegeDeleteFiles));
    btnNewFolder->setVisible(info.privileges().testFlag(Qws::PrivilegeCreateFolders));

    // Info page
    btnInfoApply->setEnabled(info.privileges().testFlag(Qws::PrivilegeAlterFiles));
    infoName->setEnabled(info.privileges().testFlag(Qws::PrivilegeAlterFiles));
    infoComment->setReadOnly(!info.privileges().testFlag(Qws::PrivilegeAlterFiles));
}



/*! Handle a new listing item from the server and add it to the file list.
*/
void QwcFileBrowserWidget::handleFilesListItem(QwcFileInfo item)
{
    if (!m_waitingForListItems) { return; }
    QList<QStandardItem*> columns;
    QStandardItem *newItem;

    newItem = new QStandardItem(); // File name
    newItem->setText(item.fileName());
    newItem->setIcon(item.fileIcon());
    newItem->setData(QVariant::fromValue(item), Qt::UserRole); // file information
    newItem->setData(item.fileName(), Qt::UserRole + 1); // sort role
    columns << newItem;

    newItem = new QStandardItem(); // Date
    newItem->setText(item.modified.toString());
    newItem->setData(item.modified, Qt::UserRole + 1); // sort role
    columns << newItem;

    newItem = new QStandardItem(); // Size
    if (item.type == Qw::FileTypeRegular) {
        newItem->setText(item.humanReadableSize(item.size));
    } else {
        newItem->setText(QString("%1 item(s)").arg(item.size));
    }
    newItem->setData(item.size, Qt::UserRole + 1); // sort role
    columns << newItem;

    m_model->appendRow(columns);


    if (item.type == Qw::FileTypeRegular) {
        m_totalUsedSpace += item.size;
    }
}


/*! Handle the end of a file listing.
*/
void QwcFileBrowserWidget::handleFilesListDone(QString path, qlonglong freeSpace)
{
    Q_UNUSED(path);
    if (!m_waitingForListItems) { return; }
    m_waitingForListItems = false;
    m_freeRemoteSpace = freeSpace;
    fStats->setText(tr("%1 items, %2 total, %3 available")
                    .arg(m_model->rowCount())
                    .arg(QwFile::humanReadableSize(m_totalUsedSpace))
                    .arg(QwFile::humanReadableSize(m_freeRemoteSpace)));
    this->setEnabled(true);
    btnBack->setEnabled(m_remotePath != "/");
    fList->resizeColumnToContents(0);

    // Set the root information as we don't get this from previous requests.
    if (m_remotePath == "/") {
        currentFolderInfo = QwcFileInfo();
        currentFolderInfo.path = "/";
        currentFolderInfo.type = Qw::FileTypeFolder;
    }

    // Enable/disable upload/download as needed
    btnUpload->setEnabled(
            (userInfo.privileges().testFlag(Qws::PrivilegeUploadAnywhere)) ||

            (currentFolderInfo.type == Qw::FileTypeUploadsFolder
             && userInfo.privileges().testFlag(Qws::PrivilegeUpload)) ||

            (currentFolderInfo.type == Qw::FileTypeDropBox
             && userInfo.privileges().testFlag(Qws::PrivilegeUpload)) );


    // Disable some controls if we are in "search mode"
    if (!findFilter->text().isEmpty()) {
        pageBrowser->setEnabled(true);
    }

}


/*! Handle the end of the file search results list.
*/
void QwcFileBrowserWidget::handleSearchResultListDone()
{

    if (!m_waitingForListItems) { return; }
    qDebug() << "DONE";
    m_waitingForListItems = false;
    m_freeRemoteSpace = 0;
    fStats->setText(tr("%1 results for \"%2\"")
                    .arg(m_model->rowCount())
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
    m_socket->getFileList(m_remotePath);
}


/*! The "Delete" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnDelete_clicked()
{
//    QList<QTreeWidgetItem*> items = fList->selectedItems();
//
//    // Confirm first
//    QMessageBox::StandardButton button = QMessageBox::question(this,
//        tr("Delete Files and Folders"),
//        tr("Are you sure you want to delete the selected %n item(s)?\nThis can not be undone!", "", items.count()),
//        QMessageBox::Yes | QMessageBox::No,
//        QMessageBox::No);
//    if (button == QMessageBox::No) { return; }
//
//    // Now delete the selected items
//    QListIterator<QTreeWidgetItem*> i(items);
//    while (i.hasNext()) {
//        QTreeWidgetItem *item = i.next();
//        QwcFileInfo itemInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
//        m_socket->deleteFile(itemInfo.path);
//    }
//
//    // Reset the file browser
//    resetForListing();
//    m_socket->getFileList(m_remotePath);
}


/*! The "Download" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnDownload_clicked()
{
//    QSettings settings;
//    QList<QTreeWidgetItem*> items = fList->selectedItems();
//
//    // Now delete the selected items
//    QListIterator<QTreeWidgetItem*> i(items);
//    while (i.hasNext()) {
//        QTreeWidgetItem *item = i.next();
//        QwcFileInfo itemInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
//
//        QDir downloadDirectory(settings.value("files/download_dir", QDir::homePath()).toString());
//
//        // Check if the target file already exists
//        if (QFile::exists(downloadDirectory.absoluteFilePath(itemInfo.fileName()))) {
//            if (QMessageBox::Cancel == QMessageBox::question(this,
//                tr("File exists"),
//                tr("The file or directory \"%1\" already exists. Do you want to overwrite it?").arg(itemInfo.fileName()),
//                QMessageBox::Cancel | QMessageBox::Save,
//                QMessageBox::Cancel))
//            {
//                return;
//            }
//        }
//
//        // Set the local path properly
//        if (itemInfo.type == Qw::FileTypeRegular) {
//            itemInfo.localAbsolutePath = downloadDirectory.absoluteFilePath(itemInfo.fileName() + ".WiredTransfer");
//        } else {
//            itemInfo.localAbsolutePath = downloadDirectory.absoluteFilePath(itemInfo.fileName());
//        }
//
//        m_socket->downloadFileOrFolder(itemInfo);
//    }
}


/*! The "Preview" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnPreview_clicked()
{
//    QSettings settings;
//    QList<QTreeWidgetItem*> items = fList->selectedItems();
//
//    // Now delete the selected items
//    QListIterator<QTreeWidgetItem*> i(items);
//    while (i.hasNext()) {
//        QTreeWidgetItem *item = i.next();
//        QwcFileInfo itemInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
//        QDir downloadDirectory = QDir::temp();
//        itemInfo.localAbsolutePath = downloadDirectory.absoluteFilePath(itemInfo.fileName() + ".WiredTransfer");
//        itemInfo.previewFileAfterTransfer = true;
//        m_socket->downloadFileOrFolder(itemInfo);
//    }
}


/*! The "Upload" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnUpload_clicked()
{
    QStringList targetFiles;

    if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
        // Select a directory when the alt-key is held
        QString selectedFolder = QFileDialog::getExistingDirectory(this, tr("Upload folder"),
                                                                   QDir::homePath());
        if (selectedFolder.isEmpty()) { return; }
        targetFiles << selectedFolder;
    } else {
        // By default we allow the user to select files
        targetFiles = QFileDialog::getOpenFileNames(this, tr("Upload Files"),
                                                    QDir::homePath(), tr("Any File (*.*)"));
        if (targetFiles.isEmpty()) { return; }
    }


    QStringListIterator i(targetFiles);
    while (i.hasNext()) {
        QString itemFile = i.next();
        QFileInfo itemInfo(itemFile);
        QwcFileInfo targetInfo;
        targetInfo.localAbsolutePath = itemFile;
        targetInfo.path = QDir::cleanPath(currentFolderInfo.path + "/" + itemInfo.fileName());
        if (itemInfo.isDir()) {
            targetInfo.type = Qw::FileTypeFolder;
        } else {
            targetInfo.type = Qw::FileTypeRegular;
            targetInfo.size = itemInfo.size();
            targetInfo.updateLocalChecksum();
        }
        m_socket->uploadFileOrFolder(targetInfo);
    }
}


/*! The user has pressed return and the server should now search for the files.
*/
void QwcFileBrowserWidget::on_findFilter_returnPressed()
{
    resetForListing();
    if (!findFilter->text().isEmpty()) {
        this->setEnabled(false);
        m_socket->searchFiles(findFilter->text());
    } else {
        m_socket->getFileList("/");
    }
}


void QwcFileBrowserWidget::on_fList_doubleClicked(const QModelIndex &index)
{
    QStandardItem *clickedItem = m_model->itemFromIndex(index);
    if (!clickedItem) { return; }
    QStandardItem *firstColumnItem = m_model->item(index.row(), 0);
    if (!firstColumnItem) { return; }

    QwcFileInfo fileInfo = firstColumnItem->data(Qt::UserRole).value<QwcFileInfo>();

    if (fileInfo.type == Qw::FileTypeDropBox
        || fileInfo.type == Qw::FileTypeFolder
        || fileInfo.type == Qw::FileTypeUploadsFolder)
    {
        m_remotePath = fileInfo.path;
        currentFolderInfo = fileInfo;
        setRemotePath(fileInfo.path);
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






/*! The "Back" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnBack_clicked()
{
    m_remotePath = m_remotePath.section("/", 0, -2);
    if (!m_remotePath.startsWith("/")) {
        m_remotePath = m_remotePath.prepend("/");
    }
    resetForListing();
    m_socket->getFileList(m_remotePath);
}


/*! The "Get Info" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnInfo_clicked()
{
//    QList<QTreeWidgetItem*> items = fList->selectedItems();
//    if (items.count() == 0) { return; }
//    QTreeWidgetItem *item = items.first();
//    currentFileInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
//    m_socket->getFileInformation(currentFileInfo.path);
//    stackedWidget->setCurrentWidget(pageInfo);
//    pageInfo->setEnabled(false);
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
    m_socket->createFolder(QString("%1/%2").arg(m_remotePath).arg(folderName));
    resetForListing();
    m_socket->getFileList(m_remotePath);
}


/*! Another item was selected from the list.
*/
//void QwcFileBrowserWidget::on_fList_itemSelectionChanged()
//{
//    btnInfo->setEnabled(fList->selectedItems().count());
//    btnDelete->setEnabled(fList->selectedItems().count());
//
//    btnDownload->setEnabled(fList->selectedItems().count()
//                            && userInfo.privileges().testFlag(Qws::PrivilegeDownload));
//
//    if (fList->selectedItems().count()) {
//        QStringList supportedPreviewSuffixes = QStringList() << "jpg" << "png" << "gif" << "jpeg";
//        QwcFileInfo fileInfo = fList->selectedItems().first()->data(0, Qt::UserRole).value<QwcFileInfo>();
//        btnPreview->setEnabled(fList->selectedItems().count()
//                    && userInfo.privileges().testFlag(Qws::PrivilegeDownload)
//                    && fileInfo.type == Qw::FileTypeRegular
//                    && supportedPreviewSuffixes.contains(fileInfo.fileName().section(".", -1, -1), Qt::CaseInsensitive));
//    } else {
//        btnPreview->setEnabled(false);
//    }
//}


/*! An item has been double-clicked. If it is a directory, we should descent into it.
*/
//void QwcFileBrowserWidget::on_fList_itemDoubleClicked(QTreeWidgetItem *item, int column)
//{
//    Q_UNUSED(column);
//    if (!item) { return; }
//    QwcFileInfo fileInfo = item->data(0, Qt::UserRole).value<QwcFileInfo>();
//    if (fileInfo.type == Qw::FileTypeDropBox
//        || fileInfo.type == Qw::FileTypeFolder
//        || fileInfo.type == Qw::FileTypeUploadsFolder)
//    {
//        m_remotePath = fileInfo.path;
//        currentFolderInfo = fileInfo;
//        resetForListing();
//        m_socket->getFileList(m_remotePath);
//    }
//
//}




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

    if (currentFileInfo.comment != newInfo.comment) {
        m_socket->setFileComment(newInfo.path, newInfo.comment);
    }

    if (currentFileInfo.path != newInfo.path) {
        m_socket->moveFile(currentFileInfo.path, newInfo.path);
    }

    resetForListing();
    m_socket->getFileList(currentFolderInfo.path);
    stackedWidget->setCurrentWidget(pageBrowser);
}



