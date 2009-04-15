#ifndef QWCFILETRANSFERMODEL_H
#define QWCFILETRANSFERMODEL_H

#include <QAbstractListModel>

#include "QwcSocket.h"
#include "QwcTransferSocket.h"

class QwcFiletransferModel : public QAbstractListModel
{
	Q_OBJECT
		
public:
    QwcFiletransferModel(QObject *parent = 0);
    ~QwcFiletransferModel();
	
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
	int rowCount(const QModelIndex &parent=QModelIndex()) const;

    void setSocket(QwcSocket *theSocket);
	
    QPointer<QwcSocket> pSocket;

//protected:
// 	void timerEvent(QTimerEvent *event);
	
private slots:
    void updateTransfers(const QwcTransferInfo theTransfer);
	void reloadTransfers();
};

#endif
