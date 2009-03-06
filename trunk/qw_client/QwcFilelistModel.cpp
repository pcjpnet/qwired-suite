#include "QwcFilelistModel.h"

#include <QVariant>

#include <math.h>
#include "QwcGlobals.h"

QwcFilelistModel::QwcFilelistModel(QObject *parent) : QStandardItemModel(parent)
{
    pTotalSize=0;
    pWaitingForList=false;

    QStringList tmpHeaders;
    tmpHeaders << tr("Name") << tr("Size") << tr("Modified");
    this->setHorizontalHeaderLabels(tmpHeaders);

}

QwcFilelistModel::~QwcFilelistModel()
{ }

//
// External signals
//

// The listing is comlete, udpate the view.
void QwcFilelistModel::onServerFileListDone(QString thePath, qlonglong theFreeSpace) {
    if( thePath==pCurrentPath && pWaitingForList ) {
        pWaitingForList = false;
        pFreeSize = theFreeSpace;
    }
}

// Add a new file list item as soon it is returned from the server.
void QwcFilelistModel::onServerFileListItem(QwcFileInfo file) {
    if( file.path.startsWith(pCurrentPath) && pWaitingForList ) {
        // Add an item
        QList<QStandardItem*> tmpList;
        QStandardItem *item;

        QVariant tmpData;
        tmpData.setValue(file);

        item = new QStandardItem(); // name
        item->setDragEnabled(file.type==0);
        item->setText(file.fileName());
        item->setIcon(file.fileIcon());
        item->setData(file.fileName(), Qt::UserRole);
        item->setData(tmpData, Qt::UserRole+1);
        tmpList.append(item);

        item = new QStandardItem(); // size
        item->setText( file.type==0 ? QwcFileInfo::humanReadableSize(file.size) : QString::number(file.size) );
        item->setData(file.size, Qt::UserRole);
        item->setData(tmpData, Qt::UserRole+1);
        tmpList.append(item);

        item = new QStandardItem(); // changed
        item->setText(file.modified.toString(Qt::SystemLocaleDate));
        item->setData(file.modified.toTime_t(), Qt::UserRole);
        item->setData(tmpData, Qt::UserRole+1);
        tmpList.append(item);

        this->appendRow(tmpList);
        pTotalSize += file.size;
    }
}

// Removes all items from the file list
void QwcFilelistModel::clearList() {
    this->setRowCount(0);
    pTotalSize = 0;
}

// Returns true if stuff can be dropped onto the list/view.
bool QwcFilelistModel::dropMimeData(const QMimeData *, Qt::DropAction, int, int, const QModelIndex &) {
    return true;
}

// Returns which drop/drag actions are supported.
Qt::DropActions QwcFilelistModel::supportedDropActions() const {
    return Qt::CopyAction;
}



