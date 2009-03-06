#ifndef QWCFILELISTMODEL_H
#define QWCFILELISTMODEL_H

#include "QwcFileInfo.h"
#include <QStandardItemModel>

class QwcFilelistModel : public QStandardItemModel
{

    Q_OBJECT

public:
    QwcFilelistModel(QObject *parent = 0);
    ~QwcFilelistModel();
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
