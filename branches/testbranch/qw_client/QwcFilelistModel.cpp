#include "QwcFilelistModel.h"
#include "QwcGlobals.h"
#include <QVariant>
#include <math.h>


QwcFilelistModel::QwcFilelistModel(QObject *parent) : QStandardItemModel(parent)
{
    pTotalSize=0;
    pWaitingForList=false;

    QStringList tmpHeaders;
    tmpHeaders << tr("Name") << tr("Size") << tr("Modified");
    this->setHorizontalHeaderLabels(tmpHeaders);
}


/*! Called when the file list is complete.
*/
void QwcFilelistModel::onServerFileListDone(QString thePath, qlonglong theFreeSpace)
{
    if (thePath == pCurrentPath && pWaitingForList ) {
        pWaitingForList = false;
        pFreeSize = theFreeSpace;
    }
}



/*! A new file list item is available. Add it to the list.
*/
void QwcFilelistModel::onServerFileListItem(QwcFileInfo file)
{
    if (file.path.startsWith(pCurrentPath) && pWaitingForList) {
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


/*! Remove all file items from the model and clear it.
*/
void QwcFilelistModel::clearList()
{
    this->setRowCount(0);
    pTotalSize = 0;
}


/*! (reimplementation) Returns true if files can be dropped onto the browser.
*/
bool QwcFilelistModel::dropMimeData(const QMimeData *, Qt::DropAction, int, int, const QModelIndex &)
{
    return true;
}


/*! (reimplemented) Returns the supported drop actions.
*/
Qt::DropActions QwcFilelistModel::supportedDropActions() const
{
    return Qt::CopyAction;
}



