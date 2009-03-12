
#include "QwcGlobals.h"
#include "QwcUserlistModel.h"
#include "QwcUserInfo.h"
//#include <QtGui>
//#include <QtCore>

QwcUserlistModel::QwcUserlistModel(QObject *parent) : QAbstractListModel(parent)
{
    chatID = 1;

    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
}


QwcUserlistModel::~QwcUserlistModel()
{ }


Qt::ItemFlags QwcUserlistModel::flags (const QModelIndex & index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
//    if (index.isValid()) {
//        return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
//    } else {
//        return Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
//    }
}


QStringList QwcUserlistModel::mimeTypes() const
{
    return QStringList();
}

QMimeData *QwcUserlistModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QModelIndex tmpIdx = indexes.first();
    QByteArray data = tmpIdx.data(Qt::UserRole).toByteArray();
    mimeData->setData("application/x-userid", data);
    return mimeData;
}

bool QwcUserlistModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    qDebug() << "drop mime data";
    if (action == Qt::IgnoreAction)
        return true;

    if (data->hasFormat("application/x-userid")) {
        QByteArray dat = data->data("application/x-userid");
        int tmpID = dat.toInt();
        wiredSocket->inviteClientToChat(chatID, tmpID);
        return true;
    }
    return false;
}


/*! This reimplementation returns a QVariant<QwcUserInfo> in the Qt::UserRole field which contains
    the information about the selected row/entry.
*/
QVariant QwcUserlistModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) { return QVariant(); }
    if (index.row() >= wiredSocket->userCountByChat(chatID)) { return QVariant(); }
    QwcUserInfo userInfo = wiredSocket->userByIndex(chatID, index.row());

    if (role == Qt::UserRole) {
        return QVariant::fromValue(userInfo);
    }

/*
    if(role == Qt::DisplayRole) {

        return wu.pNick;
    } else if( role == Qt::DecorationRole ) { // Nickname
        QwcUserInfo wu = wiredSocket->userByIndex(chatID, index.row());
        QImage img;
        if(!wu.pImage.isEmpty()) {
            img = QImage::fromData(wu.pImage);
            img = img.scaled(32,32,Qt::KeepAspectRatio);
        }
        return img;
    } else if( role == Qt::UserRole+4 ) { // Status text
        QwcUserInfo wu = wiredSocket->userByIndex(chatID, index.row());
        return wu.pStatus;
    } else if( role == Qt::UserRole+1 ) { // Admin Flag
        QwcUserInfo wu = wiredSocket->userByIndex(chatID, index.row());
        return wu.pAdmin;
    } else if( role == Qt::UserRole+2 ) { // Idle Flag
        QwcUserInfo wu = wiredSocket->userByIndex(chatID, index.row());
        return wu.pIdle;
    } else if( role == Qt::UserRole) { // User ID
        QwcUserInfo wu = wiredSocket->userByIndex(chatID, index.row());
        return wu.pUserID;
    } else {
        return QVariant();
    }

    */
}


/*! Returns the row count of the current model.
*/
int QwcUserlistModel::rowCount(const QModelIndex &) const
{
    if (!wiredSocket) { return 0; }
    return wiredSocket->userCountByChat(chatID);
}


/*! Set the socket for the current model and automatically connect the right signals to add, update
    and remove the user entries for a given chat.
*/
void QwcUserlistModel::setWiredSocket(QwcSocket *theSocket)
{
    wiredSocket = theSocket;
    if (!wiredSocket ) { return; }
    connect( wiredSocket, SIGNAL(onServerUserlistDone(int)), this, SLOT(onDataUpdate(int)) );
    connect( wiredSocket, SIGNAL(onServerUserChanged(const QwcUserInfo, const QwcUserInfo)),
             this, SLOT(onServerUserChanged(const QwcUserInfo, const QwcUserInfo)) );
    connect( wiredSocket, SIGNAL(onServerUserJoined(int, const QwcUserInfo)),
             this, SLOT(onServerUserJoined(int, const QwcUserInfo)) );
    connect( wiredSocket, SIGNAL(onServerUserLeft(int, const QwcUserInfo)),
             this, SLOT(onServerUserLeft(int, const QwcUserInfo)) );
}


// Data base been updated.
void QwcUserlistModel::onDataUpdate(int theChatID)
{
    if (!wiredSocket) { return; }
    if (theChatID != chatID) { return; }
    reset();
}


/*! This is called from the application singleton and is supposed to refresh the whole list in case
    the fonts or display settings changed.
*/
void QwcUserlistModel::reloadPreferences()
{
    reset();
}


/*! This slot is called from the socket if a user name, status or other information has changed.
*/
void QwcUserlistModel::onServerUserChanged(const QwcUserInfo, const QwcUserInfo theNew)
{
    if (!wiredSocket) { return; }
    int userIndex = wiredSocket->userIndexByID(theNew.pUserID);
    emit dataChanged(createIndex(userIndex,0), createIndex(userIndex,0));
}


/*! This slot is called from the socket if a user has joined the server or a room.
*/
void QwcUserlistModel::onServerUserJoined(int theChatID, const QwcUserInfo theUser)
{
    Q_UNUSED(theUser);
    if (!wiredSocket) { return; }
    if (theChatID != chatID) { return; }
    reset();
}


/*! This slot is called from the socket if a user has left the room/server.
*/
void QwcUserlistModel::onServerUserLeft(int theChatID, const QwcUserInfo)
{
    if (!wiredSocket) { return; }
    if (theChatID != chatID) { return; }
    reset();
}


bool QwcUserlistModel::removeRows(int row, int count, const QModelIndex &)
{
    beginRemoveRows(QModelIndex(), row, row+count-1);
    endRemoveRows();
    return true;
}


bool QwcUserlistModel::insertRows(int row, int count, const QModelIndex &)
{
    beginInsertRows(QModelIndex(), row, row+count-1);
    endInsertRows();
    return true;
}






