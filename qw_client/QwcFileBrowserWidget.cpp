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
}

void QwcFileBrowserWidget::initWithConnection(QwcSession *theSession)
{
    pModel = new QwcFilelistModel(this);
    pFilterProxy = new QSortFilterProxyModel(this);
    pFilterProxy->setSourceModel(pModel);
    pFilterProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    pFilterProxy->setSortRole(Qt::UserRole);

    pSession = theSession;
    connect( pSession->wiredSocket(), SIGNAL(onFilesListItem(QwcFileInfo)), pModel, SLOT(onServerFileListItem(QwcFileInfo)) );
    connect( pSession->wiredSocket(), SIGNAL(onFilesListDone(QString,qlonglong)), pModel, SLOT(onServerFileListDone(QString,qlonglong)) );
    connect( pSession->wiredSocket(), SIGNAL(onFilesListDone(QString,qlonglong)), this, SLOT(doUpdateBrowserStats(QString,qlonglong)) );
    connect( pSession->wiredSocket(), SIGNAL(fileTransferDone(QwcTransferInfo)), this, SLOT(fileTransferDone(QwcTransferInfo)) );

    fList->setModel(pFilterProxy);
    fList->setAlternatingRowColors(true);

    fBtnNewFolder->setEnabled( pSession->wiredSocket()->sessionUser.privCreateFolders );
}


void QwcFileBrowserWidget::on_fFilter_textEdited(QString theFilter)
{
    pFilterProxy->setFilterWildcard(theFilter);
}


// Item has been double-clicked; if it is a folder, browse it
void QwcFileBrowserWidget::on_fList_doubleClicked(const QModelIndex &index)
{
    if(pModel!=0 and pSession!=0) {
        QwcFileInfo tmpFile = index.data(Qt::UserRole+1).value<QwcFileInfo>();

        if (tmpFile.type == Qw::FileTypeRegular) {
            if(!pSession->wiredSocket()->sessionUser.privDownload) return;
            downloadFile(tmpFile.path);
            pSession->doActionTransfers();

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


void QwcFileBrowserWidget::setPath(QString thePath)
{
    if(pModel)
        pModel->pCurrentPath = thePath;
}


// Navigate back
void QwcFileBrowserWidget::on_fBtnBack_clicked(bool)
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


void QwcFileBrowserWidget::on_fBtnInfo_clicked(bool)
{
    QModelIndex tmpIdx = fList->currentIndex();
    if(tmpIdx.isValid()) {
        QwcFileInfo tmpFile = tmpIdx.data(Qt::UserRole+1).value<QwcFileInfo>();
        pSession->wiredSocket()->statFile(tmpFile.path);
        fBtnInfo->setEnabled(false);
    }
}


// Update the stats in the browser
void QwcFileBrowserWidget::doUpdateBrowserStats(QString thePath, qlonglong theFree)
{
    Q_UNUSED(theFree)
            if( thePath == pModel->pCurrentPath ) {
        QString tmpStr;
        tmpStr = tr("%1 items, %2 total, %3 available")
                 .arg(pModel->rowCount())
                 .arg( QwcFileInfo::humanReadableSize(pModel->pTotalSize) )
                 .arg( QwcFileInfo::humanReadableSize(pModel->pFreeSize) );

        if(thePath!="/")
            setWindowTitle(tr("Files - %1").arg(pModel->pCurrentPath.section("/",-1)));
        else	setWindowTitle(tr("Files - %1").arg(pModel->pCurrentPath));

        fStats->setText(tmpStr);
        fList->resizeColumnToContents(0);
        fList->resizeColumnToContents(1);
        fList->resizeColumnToContents(2);
    }
}


/*! The user clicked on a file and the download of the said file should be started
*/
void QwcFileBrowserWidget::downloadFile(QString path)
{
    QSettings settings;
    QString fileName = path.section("/", -1, -1);
    QDir localTargetFolder(settings.value("files/download_dir", QDir::homePath()).toString());
    QFile localTargetFile(localTargetFolder.absoluteFilePath(fileName));
    pSession->downloadFile(path, localTargetFile.fileName());
}


// Download button pressed
void QwcFileBrowserWidget::on_fBtnDownload_clicked(bool)
{
    if(!pSession->wiredSocket()->sessionUser.privDownload) return;
    QListIterator<QModelIndex> i(fList->selectionModel()->selectedRows(0));
    while(i.hasNext()) {
        QModelIndex index = i.next();
        if(!index.isValid()) continue;
        QwcFileInfo tmpFile = index.data(Qt::UserRole+1).value<QwcFileInfo>();
        if (tmpFile.type == Qw::FileTypeFolder|| tmpFile.type == Qw::FileTypeDropBox|| tmpFile.type == Qw::FileTypeUploadsFolder) {
            // Folder download
            QSettings settings;
            QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
            pSession->downloadFolder(tmpFile.path, tmpDownloadFolder.path());
        } else { // Regular file
            downloadFile(tmpFile.path);
        }
    }
    pSession->doActionTransfers();
}


// Another file has been selected
void QwcFileBrowserWidget::on_fList_clicked(const QModelIndex&)
{
    QModelIndex tmpIdx = fList->currentIndex();
    if(tmpIdx.isValid()) {
        QwcFileInfo tmpFile = tmpIdx.data(Qt::UserRole+1).value<QwcFileInfo>();
        fBtnDownload->setEnabled(pSession->wiredSocket()->sessionUser.privDownload); // Only files can be downloaded now
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


void QwcFileBrowserWidget::on_fBtnUpload_clicked(bool)
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Upload File"), QDir::homePath());
    QStringListIterator i(files);
    while(i.hasNext()) {
        QString file = i.next();
        if(file.isEmpty()) continue;
        QString tmpFileName = file.section("/",-1,-1);
        QString tmpRemote = pModel->pCurrentPath+"/"+tmpFileName;
        pSession->uploadFile(file, tmpRemote);

    }
    pSession->doActionTransfers();
}


void QwcFileBrowserWidget::on_fBtnDelete_clicked(bool)
{
    QModelIndexList list = fList->selectionModel()->selectedRows(0);

    QMessageBox::StandardButton button = QMessageBox::question(this,
                                                               tr("Delete File"), tr("Are you sure you want to delete the selected %n item(s)?\nThis can not be undone!", "", list.count()),
                                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (button != QMessageBox::Yes) return;

    QListIterator<QModelIndex> i(list);
    while(i.hasNext()) {
        QModelIndex index = i.next();
        if(!index.isValid()) continue;
        QwcFileInfo tmpFile = index.data(Qt::UserRole+1).value<QwcFileInfo>();
        pSession->wiredSocket()->deleteFile(tmpFile.path);
    }

    // Request an updated list
    pModel->clearList();
    pModel->pWaitingForList = true;
    pSession->wiredSocket()->getFileList(pModel->pCurrentPath);
}


void QwcFileBrowserWidget::on_fBtnNewFolder_clicked(bool)
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
void QwcFileBrowserWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}


/// A file has been dropped, lets upload.
void QwcFileBrowserWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> tmpUrls = event->mimeData()->urls();
    QListIterator<QUrl> i(tmpUrls);
    while(i.hasNext()) {
        QUrl tmpUrl = i.next();
        QFileInfo fileInfo(tmpUrl.toLocalFile());
        QFile tmpFile(tmpUrl.toLocalFile());
        if(fileInfo.exists()) {
            QString tmpRemote = pModel->pCurrentPath+"/"+fileInfo.fileName();
            qDebug() <<this<< "Got a drop event:"<<tmpFile.fileName()<<"folder:"<<fileInfo.isDir()<<"file:"<<fileInfo.isFile();
            if(fileInfo.isDir())
                pSession->uploadFolder(fileInfo.filePath(), tmpRemote);
            else	pSession->uploadFile(tmpFile.fileName(), tmpRemote);

        }
    }
    pSession->doActionTransfers();
    event->acceptProposedAction();
}


/// File transfer completed. Refresh the view.
void QwcFileBrowserWidget::fileTransferDone(QwcTransferInfo transfer)
{
    if(transfer.type != Qw::TransferTypeUpload) return;
    pModel->clearList();
    pModel->pWaitingForList = true;
    pSession->wiredSocket()->getFileList( pModel->pCurrentPath );
}






