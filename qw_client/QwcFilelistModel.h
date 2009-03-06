#ifndef QWCFILELISTMODEL_H
#define QWCFILELISTMODEL_H

#include "QwcFileInfo.h"
#include <QStandardItemModel>

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class QwcFilelistModel
	: public QStandardItemModel
{

Q_OBJECT

public:
    QwcFilelistModel(QObject *parent = 0);
    ~QwcFilelistModel();

//     QList<QwcFileInfo> pFiles;
//     
//     int rowCount(const QModelIndex &parent=QModelIndex()) const;
//     int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
//     QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
//     QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const;
//

	
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
			
	Qt::DropActions supportedDropActions() const;
	
	qulonglong pTotalSize;
	qulonglong pFreeSize;
	QString pCurrentPath;
	bool pWaitingForList;
    
    
public slots:
     void onServerFileListItem(QwcFileInfo file);
     void onServerFileListDone(QString thePath, qlonglong theFreeSpace);
     void clearList();

};

#endif
