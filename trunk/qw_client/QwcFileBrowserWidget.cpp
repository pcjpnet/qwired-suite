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
#include "QwcFiletransferDelegate.h"

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

    m_fileListModel = new QStandardItemModel(this);
    m_fileListModel->setSortRole(Qt::UserRole + 1);
    fList->setModel(m_fileListModel);
    connect(fList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleListSelectionChanged(QItemSelection,QItemSelection)));

    m_fileTransferModel = new QStandardItemModel(this);
    transferList->setModel(m_fileTransferModel);
    transferList->setItemDelegate(new QwcFiletransferDelegate);

    connect(transferList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleTransferListSelectionChanged(QItemSelection,QItemSelection)));

    labelCurrentPath->setAttribute(Qt::WA_MacSmallSize);
    fStats->setAttribute(Qt::WA_MacSmallSize);
    fList->setAttribute(Qt::WA_MacSmallSize);

    stackedWidget->setCurrentWidget(pageBrowser);
}


void QwcFileBrowserWidget::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }

    m_socket = socket;
    connect(m_socket, SIGNAL(onFilesListItem(QwcFileInfo)),
            this, SLOT(handleFilesListItem(QwcFileInfo)));
    connect(m_socket, SIGNAL(onFilesListDone(QString,qint64)),
            this, SLOT(handleFilesListDone(QString,qint64)));

    connect(m_socket, SIGNAL(fileSearchResultListItem(QwcFileInfo)),
            this, SLOT(handleFilesListItem(QwcFileInfo)));
    connect(m_socket, SIGNAL(fileSearchResultListDone()),
            this, SLOT(handleSearchResultListDone()));

    connect(m_socket, SIGNAL(transferCreated(QwcTransfer*)),
            this, SLOT(handleSocketTransferCreated(QwcTransfer*)));
    connect(m_socket, SIGNAL(transferRemoved(QwcTransfer*)),
            this, SLOT(handleSocketTransferRemoved(QwcTransfer*)));
    connect(m_socket, SIGNAL(transferChanged(QwcTransfer*)),
            this, SLOT(handleSocketTransferChanged(QwcTransfer*)));

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
    m_fileListModel->clear();
    m_fileListModel->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Modified") << tr("Size"));
    labelCurrentPath->setText(m_remotePath);
    m_totalUsedSpace = 0;
    m_freeRemoteSpace = 0;
    m_waitingForListItems = true;
}


/*! Set the information about a file on the info display page.
*/
void QwcFileBrowserWidget::setFileInformation(const QwFile &file)
{
    infoName->setText(file.fileName());
//    infoIcon->setPixmap(file.fileIcon().pixmap(16,16));
    infoSize->setText(tr("%1 (%2)").arg(QwcFileInfo::humanReadableSize(file.size())).arg(file.size()));
    infoPath->setText(file.remotePath());
    infoModified->setText(file.modified.toString(Qt::SystemLocaleShortDate));
    infoCreated->setText(file.created.toString(Qt::SystemLocaleShortDate));
    infoChecksum->setText(file.checksum());
    infoComment->setText(file.comment);

    switch (file.type) {
    case Qw::FileTypeDropBox: infoKind->setText(tr("Drop Box")); break;
    case Qw::FileTypeFolder: infoKind->setText(tr("Folder")); break;
    case Qw::FileTypeRegular: infoKind->setText(tr("File")); break;
    case Qw::FileTypeUploadsFolder: infoKind->setText(tr("Upload Folder")); break;
    default: infoKind->setText(tr("n/a")); break;
    }

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

    QFontMetrics fontMetrics(newItem->font());
    newItem->setSizeHint(QSize(fontMetrics.width(item.fileName()) + 40, 18));
    columns << newItem;

    newItem = new QStandardItem(); // Date
    newItem->setText(item.modified.toString(Qt::SystemLocaleShortDate));
    newItem->setData(item.modified, Qt::UserRole + 1); // sort role
    columns << newItem;

    newItem = new QStandardItem(); // Size
    if (item.type == Qw::FileTypeRegular) {
        newItem->setText(item.humanReadableSize(item.size()));
    } else {
        newItem->setText(QString("%1 item(s)").arg(item.size()));
    }
    newItem->setData(item.size(), Qt::UserRole + 1); // sort role
    columns << newItem;

    m_fileListModel->appendRow(columns);


    if (item.type == Qw::FileTypeRegular) {
        m_totalUsedSpace += item.size();
    }
}


/*! Handle the end of a file listing.
*/
void QwcFileBrowserWidget::handleFilesListDone(const QString &path, qint64 freeSpace)
{
    if (m_remotePath != path) { return; }
    if (!m_waitingForListItems) { return; }

    m_waitingForListItems = false;
    m_freeRemoteSpace = freeSpace;
    fStats->setText(tr("%1 items, %2 total, %3 available")
                    .arg(m_fileListModel->rowCount())
                    .arg(QwFile::humanReadableSize(m_totalUsedSpace))
                    .arg(QwFile::humanReadableSize(m_freeRemoteSpace)));

    btnBack->setEnabled(m_remotePath != "/");
    fList->resizeColumnToContents(0);

    // Set the root information as we don't get this from previous requests.
    if (m_remotePath == "/") {
        currentFolderInfo = QwcFileInfo();
        currentFolderInfo.setRemotePath("/");
        currentFolderInfo.type = Qw::FileTypeFolder;
    }

    // Enable/disable upload/download as needed
    btnUpload->setEnabled(
            (userInfo.privileges().testFlag(Qws::PrivilegeUploadAnywhere))
            || (currentFolderInfo.type == Qw::FileTypeUploadsFolder
                && userInfo.privileges().testFlag(Qws::PrivilegeUpload))
            || (currentFolderInfo.type == Qw::FileTypeDropBox
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
    m_waitingForListItems = false;
    m_freeRemoteSpace = 0;
    fStats->setText(tr("%1 results for \"%2\"")
                    .arg(m_fileListModel->rowCount())
                    .arg(findFilter->text()));
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
    if (!m_socket->sessionUser().privileges().testFlag(Qws::PrivilegeDownload)) { return; }
    QSettings settings;

    // Confirm first
    QMessageBox::StandardButton button = QMessageBox::question(this,
        tr("Delete Files and Folders"),
        tr("Are you sure you want to delete the selected %n item(s)?\nThis can not be undone!", "",
           fList->selectionModel()->selectedRows(0).count()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);
    if (button == QMessageBox::No) { return; }

    foreach (QModelIndex index, fList->selectionModel()->selectedRows(0)) {
        QStandardItem *clickedItem = m_fileListModel->itemFromIndex(index);
        if (!clickedItem) { continue; }
        QStandardItem *firstColumnItem = m_fileListModel->item(index.row(), 0);
        if (!firstColumnItem) { continue; }
        QwcFileInfo fileInfo = firstColumnItem->data(Qt::UserRole).value<QwcFileInfo>();
        m_socket->deleteFile(fileInfo.remotePath());
    }

    // Reset the file browser
    resetForListing();
    m_socket->getFileList(m_remotePath);
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
//    QStringList targetFiles;
//
//    if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
//        // Select a directory when the alt-key is held
//        QString selectedFolder = QFileDialog::getExistingDirectory(this, tr("Upload folder"),
//                                                                   QDir::homePath());
//        if (selectedFolder.isEmpty()) { return; }
//        targetFiles << selectedFolder;
//    } else {
//        // By default we allow the user to select files
//        targetFiles = QFileDialog::getOpenFileNames(this, tr("Upload Files"),
//                                                    QDir::homePath(), tr("Any File (*.*)"));
//        if (targetFiles.isEmpty()) { return; }
//    }
//
//
//    QStringListIterator i(targetFiles);
//    while (i.hasNext()) {
//        QString itemFile = i.next();
//        QFileInfo itemInfo(itemFile);
//        QwcFileInfo targetInfo;
//        targetInfo.localAbsolutePath = itemFile;
//        targetInfo.setRemotePath(QDir::cleanPath(currentFolderInfo.remotePath() + "/" + itemInfo.fileName()));
//        if (itemInfo.isDir()) {
//            targetInfo.type = Qw::FileTypeFolder;
//        } else {
//            targetInfo.type = Qw::FileTypeRegular;
//            targetInfo.setSize(itemInfo.size());
//            targetInfo.updateLocalChecksum();
//        }
//        m_socket->uploadFileOrFolder(targetInfo);
//    }
}


/*! The user has pressed return and the server should now search for the files.
*/
void QwcFileBrowserWidget::on_findFilter_returnPressed()
{
    resetForListing();
    if (!findFilter->text().isEmpty()) {
        m_socket->searchFiles(findFilter->text());
    } else {
        m_socket->getFileList("/");
    }
}


void QwcFileBrowserWidget::on_fList_doubleClicked(const QModelIndex &index)
{
    QStandardItem *clickedItem = m_fileListModel->itemFromIndex(index);
    if (!clickedItem) { return; }
    QStandardItem *firstColumnItem = m_fileListModel->item(index.row(), 0);
    if (!firstColumnItem) { return; }

    QwcFileInfo fileInfo = firstColumnItem->data(Qt::UserRole).value<QwcFileInfo>();

    if (fileInfo.type == Qw::FileTypeDropBox
        || fileInfo.type == Qw::FileTypeFolder
        || fileInfo.type == Qw::FileTypeUploadsFolder)
    {
        m_remotePath = fileInfo.remotePath();
        currentFolderInfo = fileInfo;
        setRemotePath(fileInfo.remotePath().section("/", 0, -1, QString::SectionSkipEmpty));
    }
}


void QwcFileBrowserWidget::handleListSelectionChanged(const QItemSelection &, const QItemSelection &)
{
    const bool &hasSelection = fList->selectionModel()->hasSelection();
    btnDownload->setEnabled(hasSelection);
    btnDelete->setEnabled(hasSelection && m_socket->sessionUser().privileges().testFlag(Qws::PrivilegeDeleteFiles));
    btnInfo->setEnabled(hasSelection);
}


void QwcFileBrowserWidget::handleTransferListSelectionChanged(const QItemSelection &selected,
                                                              const QItemSelection &deselected)
{
    if (transferList->selectionModel()->hasSelection()) {        
        pauseTransfer->setEnabled(true);
        resumeTransfer->setEnabled(true);
        cancelTransfer->setEnabled(true);
    } else {
        pauseTransfer->setEnabled(false);
        resumeTransfer->setEnabled(false);
        cancelTransfer->setEnabled(false);
    }
}

void QwcFileBrowserWidget::on_pauseTransfer_clicked()
{
    foreach (QModelIndex index, transferList->selectionModel()->selectedRows(0)) {
        if (!index.isValid()) { continue; }
        QwcTransfer *transfer = index.data(Qt::UserRole).value<QwcTransfer*>();
        if (!transfer) { continue; }
        transfer->stop();
    }
}

void QwcFileBrowserWidget::on_resumeTransfer_clicked()
{
    foreach (QModelIndex index, transferList->selectionModel()->selectedRows(0)) {
        if (!index.isValid()) { continue; }
        QwcTransfer *transfer = index.data(Qt::UserRole).value<QwcTransfer*>();
        if (!transfer) { continue; }
        transfer->start();
    }
}

void QwcFileBrowserWidget::on_cancelTransfer_clicked()
{
    foreach (QModelIndex index, transferList->selectionModel()->selectedRows(0)) {
        if (!index.isValid()) { continue; }
        QwcTransfer *transfer = index.data(Qt::UserRole).value<QwcTransfer*>();
        if (!transfer) { continue; }
        m_socket->removeTransfer(transfer);
    }
}

/*! A transfer task was created.
*/
void QwcFileBrowserWidget::handleSocketTransferCreated(QwcTransfer *transfer)
{
    QStandardItem *item = new QStandardItem;
    item->setData(qVariantFromValue(transfer), Qt::UserRole);
    m_fileTransferModel->appendRow(item);
}

/*! A transfer task was removed.
*/
void QwcFileBrowserWidget::handleSocketTransferRemoved(QwcTransfer *transfer)
{
    for (int i = 0; i < m_fileTransferModel->rowCount(); i++) {
        if (m_fileTransferModel->item(i,0)->data(Qt::UserRole).value<QwcTransfer*>() == transfer) {
            m_fileTransferModel->takeRow(i);
            return;
        }
    }
}


/*! A transfer task was created.
*/
void QwcFileBrowserWidget::handleSocketTransferChanged(QwcTransfer *transfer)
{
    for (int i = 0; i < m_fileTransferModel->rowCount(); i++) {
        if (m_fileTransferModel->item(i,0)->data(Qt::UserRole).value<QwcTransfer*>() == transfer) {
            transferList->update(m_fileTransferModel->item(i,0)->index());
            return;
        }

    }
}


void QwcFileBrowserWidget::on_btnDownload_clicked()
{
    if (!m_socket->sessionUser().privileges().testFlag(Qws::PrivilegeDownload)) { return; }
    QSettings settings;

    foreach (QModelIndex index, fList->selectionModel()->selectedRows(0)) {
        QStandardItem *clickedItem = m_fileListModel->itemFromIndex(index);
        if (!clickedItem) { continue; }
        QStandardItem *firstColumnItem = m_fileListModel->item(index.row(), 0);
        if (!firstColumnItem) { continue; }
        QwcFileInfo fileInfo = firstColumnItem->data(Qt::UserRole).value<QwcFileInfo>();

        QDir downloadFolder(settings.value("general/downloadLocation",
                                           QDir::home().absoluteFilePath("Downloads")).toString());
        m_socket->downloadPath(fileInfo.remotePath(),
                               downloadFolder.absoluteFilePath(fileInfo.fileName()));
    }
}








/*! The "Back" button has been clicked.
*/
void QwcFileBrowserWidget::on_btnBack_clicked()
{
    m_remotePath = m_remotePath.section("/", 0, -2, QString::SectionSkipEmpty);
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
    QStandardItem *clickedItem = m_fileListModel->itemFromIndex(fList->selectionModel()->selectedRows(0).first());
    if (!clickedItem) { return; }
    currentFileInfo = clickedItem->data(Qt::UserRole).value<QwcFileInfo>();

    m_socket->getFileInformation(currentFileInfo.remotePath());
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
    m_socket->createFolder(QString("%1/%2").arg(m_remotePath).arg(folderName));
    resetForListing();
    m_socket->getFileList(m_remotePath);
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
    newInfo.setRemotePath(currentFileInfo.directoryPath() + infoName->text().replace("/", "_"));

    if (currentFileInfo.comment != newInfo.comment) {
        m_socket->setFileComment(newInfo.remotePath(), newInfo.comment);
    }

    if (currentFileInfo.remotePath() != newInfo.remotePath()) {
        m_socket->moveFile(currentFileInfo.remotePath(), newInfo.remotePath());
    }

    resetForListing();
    m_socket->getFileList(currentFolderInfo.remotePath());
    stackedWidget->setCurrentWidget(pageBrowser);
}



