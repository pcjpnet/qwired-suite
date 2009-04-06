#ifndef QWCUSERLISTMODEL_H
#define QWCUSERLISTMODEL_H

#include <QAbstractListModel>
#include "QwcSocket.h"

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

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);


    void setChatID (int theValue) {chatID = theValue;}
    void setWiredSocket(QwcSocket *theSocket);
    int chatID;


private:
    QPointer<QwcSocket> wiredSocket;

private slots:
    void onDataUpdate(int theChatID);
    void userChanged(const QwcUserInfo theOld, const QwcUserInfo theNew);
    void userJoinedRoom(int theChatID, const QwcUserInfo theUser);
    void userLeftRoom(int theChatID, const QwcUserInfo theUser);
    void reloadPreferences();

};

#endif
