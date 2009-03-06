#ifndef QWCUSERLISTMODEL_H
#define QWCUSERLISTMODEL_H

#include <QAbstractListModel>
#include "QwcSocket.h"

/**
This model manages the userlists found in the forum and private chats.

	@author Bastian Bense <bastibense@gmail.com>
*/

class QwcUserlistModel : public QAbstractListModel
{

	Q_OBJECT

public:
    QwcUserlistModel(QObject *parent = 0);
    ~QwcUserlistModel();
	int rowCount(const QModelIndex &parent=QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	bool insertRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
	
	void setChatID (int theValue) {chatID = theValue;}
    void setWiredSocket(QwcSocket *theSocket);
	int chatID;
	Qt::ItemFlags flags (const QModelIndex & index) const;
 	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

private:
    QPointer<QwcSocket> wiredSocket;
	
private slots:
	void onDataUpdate(int theChatID);
    void onServerUserChanged(const QwcUserInfo theOld, const QwcUserInfo theNew);
    void onServerUserJoined(int theChatID, const QwcUserInfo theUser);
    void onServerUserLeft(int theChatID, const QwcUserInfo theUser);
	void reloadPreferences();

};

#endif
