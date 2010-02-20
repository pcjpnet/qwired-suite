#include "QwsServerController.h"
#include "QwsClientSocket.h"

#include <QtCore/QUrl>
#include <QtCore/QProcess>
#include <QtCore/QUuid>

extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

QwsServerController::QwsServerController(QObject *parent) : QObject(parent)
{
    statsTotalSent = 0;
    statsTotalReceived = 0;

    sessionIdCounter = 20;
    roomIdCounter = 10;

    // Initialize the transfer pool
    transferPool = new QwsTransferPool;

    // Initialize the main room (public chat, server user list)
    QwRoom *publicRoom = new QwRoom;
    publicRoom->pChatId = 1;
    publicRoom->pTopic = tr("Welcome to Qwired Server!");
    rooms[1] = publicRoom;

    // Configuration Defaults
    m_banDuration = 30 * 60; // 30 minutes default (will be overwritten by config)
    m_maxTransfersPerClient = 2;
    m_delayedUserImagesEnabled = true;

    m_lua = NULL;
}


QwsServerController::~QwsServerController()
{
    delete transferPool;
    if (m_lua) {
        qDebug() << "Destroying Lua engine";
        lua_close(m_lua);
        m_lua = NULL;
    }
}


/*! Read the configuration file from the disk and store it in our configuration hash for quick
    parameter access during runtime.
 */
bool QwsServerController::loadConfiguration()
{
    m_lua = luaL_newstate();
    int result = luaL_loadfile(m_lua, "./qwserver.lua");
    if (result == 0) {
        // Load the default libraries
        luaL_openlibs(m_lua);

        // Call the script

        result = lua_pcall(m_lua, 0, 0, 0);
        if (result == 0) {

            m_banDuration = getConfig("ban_duration", 60 * 30).toInt();
            m_maxTransfersPerClient = getConfig("max_transfers_client", 2).toInt();
            m_delayedUserImagesEnabled = getConfig("enable_delayed_usericons", true).toBool();

            reloadBanlistConfiguration();
            reloadTrackerConfiguration();
            return true;
        } else {
            // Error during lua_pcall()
            qDebug() << "Fatal: Unable to execute configuration file:" << lua_tostring(m_lua, -1);
            return false;
        }


    } else {
        // Error during lua_loadfile()
        qDebug() << "Warning: Unable to load configuration file.";
        return false;
    }

    return false;
}


/*! Calls the hook_write_account(userLogin, options, action) hook in the Lua configuration which should
    handle this event and return a boolean.
    \param[in] user The user/group to be updated. If this is a Null-Object, the target will be
                    deleted.
*/
bool QwsServerController::hook_writeAccount(const QwsUser &user, bool deleteAccount)
{
    if (!m_lua) { return false; }

    QwsUser userInfo;

    // get the hook function
    lua_getglobal(m_lua, "hook_write_account");
    if (!lua_isfunction(m_lua, -1)) {
        qwLog(tr("Unable to store account - no hook_write_account() function found."));
        lua_pop(m_lua, 1); // hook_write_account
        return false;
    }

    // push the arguments
    lua_pushstring(m_lua, user.loginName().toUtf8());  // userLogin

    if (user.type() == Qws::UserTypeAccount) {  // userType
        lua_pushstring(m_lua, "user");
    } else {
        lua_pushstring(m_lua, "group");
    }

    // create the options table
    if (deleteAccount) {
        lua_pushnil(m_lua);

    } else {
        QHash<QString,QVariant> optionItems;

        if (user.type() == Qws::UserTypeAccount) {
            optionItems["user_group"] = user.groupName();
            optionItems["user_password"] = user.password();
        }

        optionItems["download_speed_limit"] = user.downloadSpeedLimit();
        optionItems["upload_speed_limit"] = user.uploadSpeedLimit();
        optionItems["download_limit"] = user.downloadLimit();
        optionItems["upload_limit"] = user.uploadLimit();

        optionItems["p_get_user_info"] = bool(user.privileges() & Qws::PrivilegeGetUserInfo);
        optionItems["p_send_broadcast"] = bool(user.privileges() & Qws::PrivilegeSendBroadcast);
        optionItems["p_post_news"] = bool(user.privileges() & Qws::PrivilegePostNews);
        optionItems["p_clear_news"] = bool(user.privileges() & Qws::PrivilegeClearNews);
        optionItems["p_download_files"] = bool(user.privileges() & Qws::PrivilegeDeleteFiles);
        optionItems["p_upload_files"] = bool(user.privileges() & Qws::PrivilegeUpload);
        optionItems["p_upload_anywhere"] = bool(user.privileges() & Qws::PrivilegeUploadAnywhere);
        optionItems["p_create_folders"] = bool(user.privileges() & Qws::PrivilegeCreateFolders);
        optionItems["p_alter_files"] = bool(user.privileges() & Qws::PrivilegeAlterFiles);
        optionItems["p_delete_files"] = bool(user.privileges() & Qws::PrivilegeDeleteFiles);
        optionItems["p_view_drop_boxes"] = bool(user.privileges() & Qws::PrivilegeViewDropboxes);
        optionItems["p_create_accounts"] = bool(user.privileges() & Qws::PrivilegeCreateAccounts);
        optionItems["p_edit_accounts"] = bool(user.privileges() & Qws::PrivilegeEditAccounts);
        optionItems["p_delete_accounts"] = bool(user.privileges() & Qws::PrivilegeDeleteAccounts);
        optionItems["p_elevate_privileges"] = bool(user.privileges() & Qws::PrivilegeElevatePrivileges);
        optionItems["p_kick_users"] = bool(user.privileges() & Qws::PrivilegeKickUsers);
        optionItems["p_ban_users"] = bool(user.privileges() & Qws::PrivilegeBanUsers);
        optionItems["p_can_not_be_disconnected"] = bool(user.privileges() & Qws::PrivilegeCanNotBeKicked);
        optionItems["p_set_chat_topic"] = bool(user.privileges() & Qws::PrivilegeChangeChatTopic);

        lua_newtable(m_lua);
        QHashIterator<QString,QVariant> it(optionItems);
        while (it.hasNext()) {
            it.next();
            lua_pushstring(m_lua, it.key().toUtf8());
            if (it.value().type() == QVariant::Bool) {
                lua_pushboolean(m_lua, it.value().toBool());
            } else {
                lua_pushstring(m_lua, it.value().toString().toUtf8());
            }
            lua_settable(m_lua, -3);
        }
    }

    int result = lua_pcall(m_lua, 3, 0, 0);
    if (result != 0) {
        // function failed
        qwLog(tr("Unable to execute hook_write_account(): %1").arg(lua_tostring(m_lua, -1)));
        lua_pop(m_lua, 1); // error
        return false;
    }

    return true;
}


/*! Call the script hook which retrieves a user from.
*/
QwsUser QwsServerController::hook_readAccount(const QString &login, Qws::UserType type)
{
    if (!m_lua) { return QwsUser(); }

    QwsUser userInfo;

    // get the hook function
    lua_getglobal(m_lua, "hook_read_account");
    if (!lua_isfunction(m_lua, -1)) {
        qwLog(tr("Unable to authenticate user - no authentication function found."));
        lua_pop(m_lua, 1); // hook_check_login
        return QwsUser();
    }

    // push the arguments
    lua_pushstring(m_lua, login.toUtf8());

    // type
    if (type == Qws::UserTypeAccount) {
        lua_pushstring(m_lua, "user");
    } else {
        lua_pushstring(m_lua, "group");
    }

    int result = lua_pcall(m_lua, 2, 1, 0);
    if (result != 0) {
        // function failed
        qwLog(tr("Unable to execute hook_read_account: %1").arg(lua_tostring(m_lua, -1)));
        lua_pop(m_lua, 1); // error
        return QwsUser();
    }

    // retrieve the options
    if (!lua_istable(m_lua, -1)) {
        qwLog(tr("Incorrect return value for hook_read_account."));
        lua_pop(m_lua, 1); // return value
        return QwsUser();
    }

    // iterate over info table
    Qws::Privileges privs = 0;
    lua_pushnil(m_lua); // first key
    while (lua_next(m_lua, -2)) {
        if (!lua_isstring(m_lua, -2)) {
            lua_pop(m_lua, 1); // clear value for next iteration
            continue;
        }

        const QString key = QString::fromUtf8(lua_tostring(m_lua, -2));
        bool flag = lua_toboolean(m_lua, -1);
        if (key == "user_group") {
            userInfo.setGroupName(QString::fromUtf8(lua_tostring(m_lua, -1)));
        } else if (key == "download_speed_limit") {
            userInfo.setDownloadSpeedLimit(lua_tointeger(m_lua, -1));
        } else if (key == "upload_speed_limit") {
            userInfo.setUploadSpeedLimit(lua_tointeger(m_lua, -1));
        } else if (key == "download_limit") {
            userInfo.setDownloadLimit(lua_tointeger(m_lua, -1));
        } else if (key == "upload_limit") {
            userInfo.setUploadLimit(lua_tointeger(m_lua, -1));
        } else if (key == "user_password") {
            userInfo.setCryptedPassword(QString::fromUtf8(lua_tostring(m_lua, -1)));
        } else if (key == "p_get_user_info" && flag) {
            privs |= Qws::PrivilegeGetUserInfo;
        } else if (key == "p_send_broadcast" && flag) {
            privs |= Qws::PrivilegeSendBroadcast;
        } else if (key == "p_post_news" && flag) {
            privs |= Qws::PrivilegePostNews;
        } else if (key == "p_clear_news" && flag) {
            privs |= Qws::PrivilegeClearNews;
        } else if (key == "p_download_files" && flag) {
            privs |= Qws::PrivilegeDownload;
        } else if (key == "p_upload_files" && flag) {
            privs |= Qws::PrivilegeUpload;
        } else if (key == "p_upload_anywhere" && flag) {
            privs |= Qws::PrivilegeUploadAnywhere;
        } else if (key == "p_create_folders" && flag) {
            privs |= Qws::PrivilegeCreateFolders;
        } else if (key == "p_alter_files" && flag) {
            privs |= Qws::PrivilegeAlterFiles;
        } else if (key == "p_delete_files" && flag) {
            privs |= Qws::PrivilegeDeleteFiles;
        } else if (key == "p_view_drop_boxes" && flag) {
            privs |= Qws::PrivilegeViewDropboxes;
        } else if (key == "p_create_accounts" && flag) {
            privs |= Qws::PrivilegeCreateAccounts;
        } else if (key == "p_edit_accounts" && flag) {
            privs |= Qws::PrivilegeEditAccounts;
        } else if (key == "p_delete_accounts" && flag) {
            privs |= Qws::PrivilegeDeleteAccounts;
        } else if (key == "p_elevate_privileges" && flag) {
            privs |= Qws::PrivilegeElevatePrivileges;
        } else if (key == "p_kick_users" && flag) {
            privs |= Qws::PrivilegeKickUsers;
        } else if (key == "p_ban_users" && flag) {
            privs |= Qws::PrivilegeBanUsers;
        } else if (key == "p_can_not_be_disconnected" && flag) {
            privs |= Qws::PrivilegeCanNotBeKicked;
        } else if (key == "p_set_chat_topic" && flag) {
            privs |= Qws::PrivilegeChangeChatTopic;
        } else if (flag) {
            qwLog(tr("Warning: Unknown info key: %1").arg(key));
        }
        lua_pop(m_lua, 1); // value
    }
    userInfo.setPrivileges(privs);

    return userInfo;
}


QList<QwsUser> QwsServerController::hook_readAccountsAndGroups()
{
    if (!m_lua) { return QList<QwsUser>(); }
    QList<QwsUser> foundAccountsAndGroups;

    // get the hook function
    lua_getglobal(m_lua, "hook_read_accounts");
    if (!lua_isfunction(m_lua, -1)) {
        qwLog(tr("Hook function hook_read_accounts() not found!"));
        lua_pop(m_lua, 1); // hook_read_accounts
        return QList<QwsUser>();
    }

    // push the arguments
    int result = lua_pcall(m_lua, 0, 2, 0);
    if (result != 0) {
        // function failed
        qwLog(tr("Unable to execute hook_read_accounts: %1").arg(lua_tostring(m_lua, -1)));
        lua_pop(m_lua, 1); // error
        return QList<QwsUser>();
    }

    // retrieve the options
    if (!lua_istable(m_lua, -1)) {
        qwLog(tr("Incorrect return value for hook_read_accounts."));
        lua_pop(m_lua, 1); // return value
        return QList<QwsUser>();
    }

    // Read the groups (second result)
    lua_pushnil(m_lua); // first key
    while (lua_next(m_lua, -2)) {
        if (lua_isstring(m_lua, -1)) {
            QwsUser item;
            item.setType(Qws::UserTypeGroup);
            item.setLoginName(QString::fromUtf8(lua_tostring(m_lua, -1)));
            foundAccountsAndGroups << item;
        }
        lua_pop(m_lua, 1); // clear value for next iteration
    }
    lua_pop(m_lua, 1); // group result


    // Read the users (first result)
    lua_pushnil(m_lua); // first key
    while (lua_next(m_lua, -2)) {
        if (lua_isstring(m_lua, -1)) {
            QwsUser item;
            item.setType(Qws::UserTypeAccount);
            item.setLoginName(QString::fromUtf8(lua_tostring(m_lua, -1)));
            foundAccountsAndGroups << item;
        }
        lua_pop(m_lua, 1); // clear value for next iteration
    }
    lua_pop(m_lua, 1); // users result

    return foundAccountsAndGroups;
}



/*! Return a configuration pamater identified by \a key. If the configuration parameter does not
    exist in the database, \a defaultValue is return. If there is a database error, a Null-QVariant
    will be returned.
*/
QVariant QwsServerController::getConfig(const QString key, const QVariant defaultValue)
{
    if (!m_lua) { return QVariant(); }
    QVariant configValue;

    lua_getglobal(m_lua, key.toUtf8());
    if (lua_isnil(m_lua, -1)) {
        // Return default value
        configValue = defaultValue;
    } else if (defaultValue.type() == QVariant::Bool) {
        // Return bool
        configValue = lua_toboolean(m_lua, -1);
    } else {
        // String
        configValue = QString::fromUtf8(lua_tostring(m_lua, -1));
    }
    lua_pop(m_lua, 1);
    return configValue;
}



/*! Reload the tracker configuration and delete any previous tracker sockets.
*/
void QwsServerController::reloadTrackerConfiguration()
{
    if (!m_lua) { return; }
    if (!trackerController) {
        trackerController = new QwsTrackerController(this);
        trackerController->setServerInformation(&m_serverInformation);
    }
    trackerController->resetController();

    // Push the table on the stack
    lua_getglobal(m_lua, "register_trackers");

    if (!lua_istable(m_lua, -1)) {
        qwLog(tr("Invalid value for register_trackers option - will not register with any trackers!"));
        lua_pop(m_lua, 1);
        return;
    }

    // Push the key onto the stack
    lua_pushnil(m_lua); // first item
    while (lua_next(m_lua, -2)) {
        if (lua_isstring(m_lua, -1)) {
            QString urlString = QString::fromUtf8(lua_tostring(m_lua, -1));
            QUrl trackerUrl(urlString, QUrl::TolerantMode);
            trackerController->addTrackerServer(trackerUrl.host(), trackerUrl.port(2002));
            qwLog(tr("Configured to register with tracker: %1").arg(trackerUrl.toString()));
        }
        lua_pop(m_lua, 1); // pop value, keep key
    }
    lua_pop(m_lua, 1); // register_trackers
}

/*! Reload the permanent banlist configration from the database.
*/
void QwsServerController::reloadBanlistConfiguration()
{
    if (!m_lua) { return; }
    // Delete all old permament bans from the banlist first
    QMutableListIterator<QPair<QString,QDateTime> > i(m_banList);
    while (i.hasNext()) {
        QPair<QString, QDateTime> banlistItem = i.next();
        if (!banlistItem.second.isNull()) { continue; }
        i.remove();
    }

    // Push the table on the stack
    lua_getglobal(m_lua, "server_banlist");
    if (!lua_istable(m_lua, -1)) {
        qwLog(tr("Invalid value for server_banlist option - will not ban any addresses!"));
        lua_pop(m_lua, 1);
        return;
    }

    // Push the key onto the stack
    lua_pushnil(m_lua); // key
    while (lua_next(m_lua, -2) != 0) {
        if (lua_isstring(m_lua, -1)) {
            QString banlistItem = QString::fromUtf8(lua_tostring(m_lua, -1));
            m_banList.append(qMakePair(banlistItem, QDateTime()));
            qwLog(tr("Added '%1' to permanent banlist.").arg(banlistItem));
        }
        lua_pop(m_lua, 1); // pop value, keep key
    }
    lua_pop(m_lua, 1); // register_trackers
}


/*! Returns the active transfers identified by \a userId.
*/
QList<QwsClientTransferSocket*> QwsServerController::transfersWithUserId(int userId)
{
    QList<QwsClientTransferSocket*> resultList;

    QListIterator<QPointer<QwsClientTransferSocket> > j(transferSockets);
    while (j.hasNext()) {
        QPointer<QwsClientTransferSocket> socket = j.next();
        if (socket.isNull()) { continue; }
        if (socket->info().targetUserId == userId) {
            resultList.append(socket);
        }
    }
    return resultList;
}


/*! Start the TCP listener in order to accept connections.
*/
bool QwsServerController::startServer()
{

    // Load the server information
    m_serverInformation.serverVersion = "Qwired Server/1.0";
    m_serverInformation.name = getConfig("server_name", "Qwired Server").toString();
    m_serverInformation.description = getConfig("server_description", "A freshly installed Qwired server.").toString();
    m_serverInformation.protocolVersion = "3.0";
    m_serverInformation.startTime = QDateTime::currentDateTime();
    m_serverInformation.filesCount = 0;
    m_serverInformation.filesSize = 0;

    QString certificateData;
    int tmpPort = getConfig("server_port", 2000).toInt();
    QString tmpAddress = getConfig("server_address", "0.0.0.0").toString();

    QString certificateFileName = getConfig("certificate_file", "").toString();
    QFileInfo certificateFileInfo(certificateFileName);


    if (!certificateFileInfo.exists()) {
        qwLog(tr("Trying to generate new certificate..."));
        // Try to create a new one
        QProcess process;
        process.start(getConfig("certificate_generation_command", "").toString());
        process.waitForFinished();
        if (process.exitCode() != 0) {
            qwLog(tr("Unable to generate new certificate, please check the "
                     "certificate_generation_command:\n%1\n%2 (command returned %3)")
                  .arg(getConfig("certificate_generation_command", "").toString())
                  .arg(QString::fromUtf8(process.readAll()))
                  .arg(process.exitCode()));
            return false;
        }
        qwLog(tr("Successfully crated new certificate file."));

    }

    // Try to load it from the file
    QFile certificateFile(certificateFileName);
    if (!certificateFile.open(QIODevice::ReadOnly)) {
        qwLog(tr("Unable to open certificate file: %1").arg(certificateFile.errorString()));
        return false;
    }
    certificateData = certificateFile.readAll();


    // Check if the files directory exists
    QDir filesDirectory(getConfig("files_root", "files").toString());
    if (!filesDirectory.exists()) {
        qwLog(tr("Files directory is missing. Creating a new one."));
        // We use "." to create the directory itself
        filesDirectory.mkpath(".");
    }


    sessionTcpServer = new QwSslTcpServer(this);
    connect(sessionTcpServer, SIGNAL(newSslConnection()),
            this, SLOT(acceptSessionSslConnection()));

    sessionTcpServer->setCertificateFromData(certificateData);
    if (!sessionTcpServer->listen(QHostAddress(tmpAddress), tmpPort)) {
        qwLog(tr("Fatal: Unable to listen on TCP port %1. %2. Terminating.")
              .arg(tmpPort).arg(sessionTcpServer->errorString()));
        return false;
    } else {
        qwLog(tr("Started control socket listener on  %2:%1...").arg(tmpPort).arg(tmpAddress));
    }


    // File transfer socket
    this->transferTcpServer = new QwSslTcpServer(this);
    connect(transferTcpServer, SIGNAL(newSslConnection()),
            this, SLOT(acceptTransferSslConnection()));

    transferTcpServer->setCertificateFromData(certificateData);
    if (!transferTcpServer->listen(QHostAddress(tmpAddress), tmpPort+1)) {
        qwLog(tr("Fatal: Unable to listen on TCP port %1. %2. File transfers won't work.").arg(tmpPort+1).arg(sessionTcpServer->errorString()));
    } else {
        qwLog(tr("Started transfer socket listener on  %2:%1...").arg(tmpPort+1).arg(tmpAddress));
    }


    return true;
}



/*! Start a new file-indexer thread and purge the contents of the index table. Only one indexer
    thread can run at the same time (checked internally).
*/
void QwsServerController::reindexFiles()
{
    qDebug() << this << "Starting new file indexer thread.";
    if (!filesIndexerThread.isNull()) { return; }
    filesIndexerThread = new QwsFileIndexerThread(this);
    connect(filesIndexerThread, SIGNAL(logMessage(QString)),
            this, SLOT(qwLog(QString)));
    filesIndexerThread->filesRootPath = getConfig("files_root", "files").toString();
    filesIndexerThread->run();
}



/*! Write a message to the server log, or standard output, or attached GUI client.
*/
void QwsServerController::qwLog(QString message)
{
    QTextStream stream(stdout);
    stream << message << endl;
    emit serverLogMessage(message);
}



/*! Accepts a new connection. Connected to the newConnect() signal of sessionTcpServer.
*/
void QwsServerController::acceptSessionSslConnection()
{
    QSslSocket *newSocket = sessionTcpServer->nextPendingSslSocket();

    // Check if the client was banned
    QMutableListIterator<QPair<QString,QDateTime> > i(m_banList);
    while (i.hasNext()) {
        QPair<QString, QDateTime> banlistItem = i.next();
        if (banlistItem.second.isNull() // permanent ban
            || (!banlistItem.second.isNull() && banlistItem.second > QDateTime::currentDateTime()) ) {
            // This is a perma-ban or a temporary ban that has not expired yet
            QRegExp regexp(banlistItem.first, Qt::CaseInsensitive, QRegExp::Wildcard);
            if (regexp.exactMatch(newSocket->peerAddress().toString())) {
                qwLog(tr("Dropping new connection from %1 (banned)")
                      .arg(newSocket->peerAddress().toString()));
                newSocket->disconnectFromHost();
                newSocket->deleteLater();
                return;
            }
        } else {
            qwLog(tr("Removing expired ban for %1").arg(banlistItem.first));
            i.remove();
        }
    }

    QwsClientSocket *clientSocket = new QwsClientSocket(this);
    clientSocket->setServerController(this);
    clientSocket->serverInfo = &m_serverInformation;
    clientSocket->user.setUserId(++sessionIdCounter);
    clientSocket->setSslSocket(newSocket);
    clientSocket->user.clientIpAddress() = clientSocket->socket()->peerAddress().toString();
    clientSocket->resolveHostname();

    qwLog(tr("[%1] Accepted new session connection from %2")
          .arg(clientSocket->user.userId())
          .arg(newSocket->peerAddress().toString()));

    connect(clientSocket, SIGNAL(connectionLost()),
            this, SLOT(handleSocketDisconnected()));

    qRegisterMetaType<QwMessage>();
    qRegisterMetaType<Qws::SessionState>();

    connect(clientSocket, SIGNAL(requestedUserlist(int)),
            this, SLOT(handleUserlistRequest(int)));
    connect(clientSocket, SIGNAL(sessionStateChanged(Qws::SessionState)),
            this, SLOT(handleSocketSessionStateChanged(Qws::SessionState)));
    connect(clientSocket, SIGNAL(requestedChatRelay(int,QString,bool)),
            this, SLOT(relayChatToRoom(int,QString,bool)));
    connect(clientSocket, SIGNAL(requestedMessageRelay(int,QString)),
            this, SLOT(relayMessageToUser(int,QString)));
    connect(clientSocket, SIGNAL(broadcastedMessage(QwMessage,int,bool)),
            this, SLOT(broadcastMessage(QwMessage,int,bool)));
    connect(clientSocket, SIGNAL(userStatusChanged()),
            this, SLOT(relayUserStatusChanged()));
    connect(clientSocket, SIGNAL(requestedRoomTopicChange(int,QString)),
            this, SLOT(changeRoomTopic(int,QString)));
    connect(clientSocket, SIGNAL(requestedRoomTopic(int)),
            this, SLOT(sendRoomTopic(int)));
    connect(clientSocket, SIGNAL(requestedNewRoom()),
            this, SLOT(createNewRoom()));
    connect(clientSocket, SIGNAL(requestedUserInviteToRoom(int,int)),
            this, SLOT(inviteUserToRoom(int,int)));
    connect(clientSocket, SIGNAL(receivedMessageJOIN(int)),
            this, SLOT(handleMessageJOIN(int)));
    connect(clientSocket, SIGNAL(receivedMessageDECLINE(int)),
            this, SLOT(handleMessageDECLINE(int)));
    connect(clientSocket, SIGNAL(receivedMessageLEAVE(int)),
            this, SLOT(handleMessageLEAVE(int)));
    connect(clientSocket, SIGNAL(receivedMessageBAN_KICK(int,QString,bool)),
            this, SLOT(handleMessageBAN_KICK(int,QString,bool)));
    connect(clientSocket, SIGNAL(receivedMessageINFO(int)),
            this, SLOT(handleMessageINFO(int)) );
    connect(clientSocket, SIGNAL(modifiedUserAccount(QString)),
            this, SLOT(handleModifiedUserAccount(QString)));
    connect(clientSocket, SIGNAL(modifiedUserGroup(QString)),
            this, SLOT(handleModifiedUserGroup(QString)));

    connect(clientSocket, SIGNAL(receivedMessageGET(QwsFile)),
            this, SLOT(handleMessageGET(QwsFile)));
    connect(clientSocket, SIGNAL(receivedMessagePUT(QwsFile)),
            this, SLOT(handleMessagePUT(QwsFile)));

    clientSocket->filesRootPath =  getConfig("files_root", "files").toString();

    m_sockets[clientSocket->user.userId()] = clientSocket;
}


/*! This slot is called whenever a client disconnects. Here we clean up behind the client and
    make sure everyone knows about it.
*/
void QwsServerController::handleSocketDisconnected()
{
    QwsClientSocket *client = qobject_cast<QwsClientSocket*>(sender());
    if (!client) { return; }

    qwLog(tr("%1 [%2] has disconnected.").arg(client->user.nickname()).arg(client->user.userId()));

    // Remove the user from the chat rooms
    QHashIterator<int, QwRoom*> i(rooms);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        QwRoom *itemRoom = i.value();
        if (itemRoom && itemRoom->pUsers.contains(client->user.userId())) {
            removeUserFromRoom(i.key(), client->user.userId());
        }
    }

    // Delete all transfers
    int deletedTransfers = transferPool->deleteTransfersWithUserId(client->user.userId());
    qDebug() << this << "Removed transfers from pool:" << deletedTransfers;
    QListIterator<QPointer<QwsClientTransferSocket> > j(transferSockets);
    while (j.hasNext()) {
        QPointer<QwsClientTransferSocket> socket = j.next();
        if (socket.isNull()) { continue; }
        if (socket->info().targetUserId == client->user.userId()) {
            qDebug() << this << "Aborting transfer" << socket;
            socket->abortTransfer();
        }
    }

    // Remove the client from the list of clients
    m_sockets.remove(client->user.userId());
    sender()->deleteLater();
}


/*! This is called whenever the state of a client changes.
*/
void QwsServerController::handleSocketSessionStateChanged(const Qws::SessionState state)
{
    QwsClientSocket *client = qobject_cast<QwsClientSocket*>(sender());
    if (!client) { return; }
    if (state == Qws::StateActive) {
        // Client became active (logged in)
        addUserToRoom(1, client->user.userId());
        qwLog(tr("[%1] '%2' successfully logged in as [%3].")
              .arg(client->user.userId())
              .arg(client->user.nickname())
              .arg(client->user.loginName()));
    }
}


/*! This is called when the status (and information) about a user changes.
*/
void QwsServerController::relayUserStatusChanged()
{
    QwsClientSocket *client = qobject_cast<QwsClientSocket*>(sender());
    if (!client) { return; }
    QwMessage reply("304");
    client->user.userStatusEntry(reply);
    broadcastMessage(reply, 1, true);
}


/*! This slot is called when a client requests information about another client.
*/
void QwsServerController::handleMessageINFO(const int userId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!m_sockets.contains(userId)) {
        user->sendError(Qw::ErrorClientNotFound);
        return;
    }

    QwsClientSocket *target = m_sockets[userId];
    if (!target) { return; }

    // Send the information
    QwMessage reply("308");
    target->user.userInfoEntry(reply);

    QList<QwsClientTransferSocket*> transfers = this->transfersWithUserId(user->user.userId());
    //QList<QwsTransferInfo> queuedTransfers = this->transferPool->findTransfersWithUserId(user->user.pUserID);

    // Transfers
    QString transfersDownload;
    QString transfersUpload;

    QListIterator<QwsClientTransferSocket*> i(transfers);
    while (i.hasNext()) {
        QwsClientTransferSocket *item = i.next();
        if (!item) { continue; }

        QString *transferInfo = 0;
        if (item->info().type == Qw::TransferTypeDownload) {
            transferInfo = &transfersDownload;
        } else {
            transferInfo = &transfersUpload;
        }

        if (!transferInfo) { continue; }
        transferInfo->append(item->info().file.remotePath() + "\x1E");
        transferInfo->append(QString::number(item->info().bytesTransferred) + "\x1E");
        transferInfo->append(QString::number(item->info().file.size()) + "\x1E");
        transferInfo->append(QString::number(item->info().currentTransferSpeed) + "\x1E"); // current speed

        if (i.hasNext()) {
            transferInfo->append("\x1D"); // RS
        }
    }

    // Replace the fields of the info message
    reply.arguments.replace(13, transfersDownload);
    reply.arguments.replace(14, transfersUpload);

    user->sendMessage(reply);
}




/*! This method adds a user to a given room and notifies all other users in the same room about this
    change, respectively sending out "client joined" messages.
*/
void QwsServerController::addUserToRoom(const int roomId, const int userId)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to add user" << userId << "to room" << roomId << "(no such room)";
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!m_sockets.contains(userId)) {
        qDebug() << this << "Unable to add user" << userId << "to room" << roomId << "(no such user)";
        return;
    }
    QwsClientSocket *user = m_sockets[userId];
    room->pUsers.append(userId);

    // Notify the other users
    QwMessage reply("302");
    reply.appendArg(QByteArray::number(roomId));
    user->user.userListEntry(reply);
    broadcastMessage(reply, roomId, false);
}

/*! This method removes a user from the a room and notifies all other users in the same room about
    this change, respectively sending out "client left" messages.
*/
void QwsServerController::removeUserFromRoom(const int roomId, const int userId)
{
    qDebug() << this << "Removing user" << userId << "from room" << roomId;
    if (!rooms.contains(roomId)) { return; }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(userId)) { return; }

    if (room->pUsers.contains(userId)) {
        room->pUsers.removeAll(userId);
        // Notify the users.
        QwMessage reply("303");
        reply.appendArg(QString::number(roomId));
        reply.appendArg(QString::number(userId));
        broadcastMessage(reply, roomId, false);
    }
    room->pInvitedUsers.removeOne(userId);

    // Delete the chat if it's the last one
    if (roomId != 1 && room->pUsers.isEmpty()) {
        qDebug() << this << "Deleting empty room" << roomId;
        rooms.remove(roomId);
        delete room;
    }
}


/*! This method handles the request for a user list of a specific room/chat. Since rooms are within
    server-scope, the request is handled here.
*/
void QwsServerController::handleUserlistRequest(const int roomId)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Warning: Invalid room id:" << roomId;
        return;
    }

    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (m_sockets.contains(itemId)) {
            QwsClientSocket *itemSocket = m_sockets[itemId];
            if (itemSocket && itemSocket->sessionState() == Qws::StateActive) {
                // Send a user list item
                QwMessage reply("310");
                reply.appendArg(QByteArray::number(roomId));
                itemSocket->user.userListEntry(reply, m_delayedUserImagesEnabled);
                user->sendMessage(reply);
            }
        }
    }

    // Send end of list
    QwMessage reply("311");
    reply.appendArg(QByteArray::number(roomId));
    user->sendMessage(reply);

    // Send delayed user images
    if (m_delayedUserImagesEnabled) {
        QListIterator<int> i(room->pUsers);
        while (i.hasNext()) {
            int itemId = i.next();
            if (m_sockets.contains(itemId)) {
                QwsClientSocket *itemSocket = m_sockets[itemId];
                if (itemSocket && itemSocket->sessionState() == Qws::StateActive) {
                    // Send the delayed image data
                    QwMessage reply("340");
                    reply.appendArg(QByteArray::number(itemId));
                    reply.appendArg(itemSocket->user.iconData());
                    user->sendMessage(reply);
                }
            }
        }
    }


}


/*! This method relays a chat mesage to all participants of a chat room. If \a isEmote is true
    the message will be sent as a emote-message (ME).
*/
void QwsServerController::relayChatToRoom(const int roomId, const QString text, const bool isEmote)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Warning: Invalid room id:" << roomId;
        return;
    }

    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);

    // Prepare the message
    QwMessage reply(isEmote ? "301 " : "300 ");
    reply.appendArg(QByteArray::number(roomId));
    reply.appendArg(QByteArray::number(user->user.userId()));
    reply.appendArg(text);

    while (i.hasNext()) {
        int itemId = i.next();
        if (m_sockets.contains(itemId)) {
            QwsClientSocket *itemSocket = m_sockets[itemId];
            if (itemSocket && itemSocket->sessionState() == Qws::StateActive) {
                itemSocket->sendMessage(reply);
            }
        }
    }
}


/*! This method relays a private message to another user.
*/
void QwsServerController::relayMessageToUser(const int userId, const QString text)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }
    
    if (m_sockets.contains(userId)) {
        QwsClientSocket *targetUser = m_sockets[userId];
        QwMessage reply("305");
        reply.appendArg(QByteArray::number(user->user.userId()));
        reply.appendArg(text);
        targetUser->sendMessage(reply);
    } else {
        user->sendError(Qw::ErrorClientNotFound);
    }
}


/*! This method relays a message to other clients in a room. If \a sendToSelf is true, the message
    will also be sent to the sender's client.
*/
void QwsServerController::broadcastMessage(const QwMessage message, const int roomId, const bool sendToSelf)
{
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Warning: Invalid room id:" << roomId;
        return;
    }

    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        qDebug() << this << "Warning: Invalid sender socket. Can not cast.";
        return;
    }

    // Run trough the list of users in the room
    QwRoom *room = rooms[roomId];
    QListIterator<int> i(room->pUsers);
    while (i.hasNext()) {
        int itemId = i.next();
        if (!m_sockets.contains(itemId)) { continue; }
        QwsClientSocket *itemSocket = m_sockets[itemId];
        if (itemSocket && itemSocket->sessionState() == Qws::StateActive) {
            if (sendToSelf || itemSocket != user) {
                itemSocket->sendMessage(message);
            }
        }
    }
}


/*! This method changes the topic of a room and notifies all clients within that room about it.
*/
void QwsServerController::changeRoomTopic(const int roomId, const QString topic)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) {
        return;
    }
    if (!rooms.contains(roomId)) {
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.userId())) {
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    // Update the room
    room->pTopic = topic;
    room->pTopicDate = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    room->pTopicSetter = user->user;
    // Notify everyone in the same room
    QwMessage reply("341");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(room->pTopicSetter.nickname());
    reply.appendArg(room->pTopicSetter.loginName());
    reply.appendArg(room->pTopicSetter.clientIpAddress());
    reply.appendArg(room->pTopicDate.toUTC().toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    broadcastMessage(reply, roomId, true);
}


/*! This method sends the room topic of the room identified by \a roomId back to the user.
*/
void QwsServerController::sendRoomTopic(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.userId())) {
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }

    // Send the information
    QwMessage reply("341");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(room->pTopicSetter.nickname());
    reply.appendArg(room->pTopicSetter.loginName());
    reply.appendArg(room->pTopicSetter.clientIpAddress());
    reply.appendArg(room->pTopicDate.toUTC().toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    user->sendMessage(reply);
}


/*! This method creates a new private chat room and responds to the client with a confirmation and
    the ID of the newly created room.
*/
void QwsServerController::createNewRoom()
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }

    // Create a new room
    QwRoom *newRoom = new QwRoom;
    newRoom->pTopic = tr("%1's Private Chat").arg(user->user.nickname());
    newRoom->pTopicDate = QDateTime::currentDateTime();
    newRoom->pTopicSetter = user->user;
    newRoom->pUsers.append(user->user.userId());
    newRoom->pChatId = ++roomIdCounter;
    rooms[newRoom->pChatId] = newRoom;

    // Send the information back to the client
    QwMessage reply("330");
    reply.appendArg(QString::number(newRoom->pChatId));
    user->sendMessage(reply);

    qDebug() << this << "Created new room with id" << newRoom->pChatId;
}


/*! This method invites a user to a room and notifies him of that.
*/
void QwsServerController::inviteUserToRoom(const int userId, const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }

    if (userId == user->user.userId()) {
        qDebug() << this << "User tried to invite himself to room" << roomId;
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }

    if (!rooms.contains(roomId)) {
        user->sendError(Qw::ErrorCommandFailed);
        qDebug() << this << "Room with id" << roomId << "does not exist!";
        return;
    }
    if (!m_sockets.contains(userId)) {
        user->sendError(Qw::ErrorClientNotFound);
        qDebug() << this << "Client with id" << roomId << "does not exist!";
        return;
    }
    QwsClientSocket *targetUser = m_sockets[userId];
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.userId())) {
        user->sendError(Qw::ErrorPermissionDenied);
        qDebug() << this << "User with id"<< user->user.userId()<<"was not in room"<<roomId;
        return;
    }
    if (room->pInvitedUsers.contains(userId)) {
        // Ignoring a double-invite here.
        user->sendError(Qw::ErrorCommandFailed);
        qDebug() << this << "User with id"<<userId<<"was already in list of invited of room" << roomId;
        return;
    }
    room->pInvitedUsers.append(userId);

    // Notify users in the same room about the invitation
    QwMessage reply2("301");
    reply2.appendArg(QString::number(roomId));
    reply2.appendArg(QString::number(user->user.userId()));
    reply2.appendArg(QString("invited %1 to join this room... [server]")
                     .arg(m_sockets[userId]->user.nickname()));
    broadcastMessage(reply2, roomId, true);

    // Send the invitation to the other user.
    QwMessage reply("331"); // 331 Private Chat Invitiation
    reply.appendArg(QString::number(roomId));
    reply.appendArg(QString::number(user->user.userId()));
    targetUser->sendMessage(reply);
}


/*! This method adds a user to a room after a JOIN command.
*/
void QwsServerController::handleMessageJOIN(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to join channel"<<roomId<<" (does not exist)";
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }

    QwRoom *room = rooms[roomId];
    if (!room->pInvitedUsers.contains(user->user.userId())) {
        qDebug() << this << "Unable to join channel"<<roomId<<" (user was not invited)";
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    room->pInvitedUsers.removeAll(user->user.userId());
    room->pUsers.append(user->user.userId());

    // Send the information back to the client
    QwMessage reply("302"); // 302 Client Join
    reply.appendArg(QString::number(roomId));
    user->user.userListEntry(reply);
    broadcastMessage(reply, roomId, false);

    // Send the client the chat topic
    reply = QwMessage("341");
    reply.appendArg(QString::number(room->pChatId));
    reply.appendArg(room->pTopicSetter.nickname());
    reply.appendArg(room->pTopicSetter.loginName());
    reply.appendArg(room->pTopicSetter.clientIpAddress());
    reply.appendArg(room->pTopicDate.toUTC().toString(Qt::ISODate)+"+00:00");
    reply.appendArg(room->pTopic);
    user->sendMessage(reply);

}


/*! This method adds a user to a room after a JOIN command.
*/
void QwsServerController::handleMessageDECLINE(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to decline invitation"<<roomId<<" (does not exist)";
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }

    QwRoom *room = rooms[roomId];
    if (!room->pInvitedUsers.contains(user->user.userId())) {
        qDebug() << this << "Unable to room invitation"<<roomId<<" (user was never invited)";
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    room->pInvitedUsers.removeAll(user->user.userId());

    // Notify the other users that this user declined.
    QwMessage reply("332");
    reply.appendArg(QString::number(roomId));
    reply.appendArg(QString::number(user->user.userId()));
    broadcastMessage(reply, roomId, true);
}


/*! This method removes a user to a room after a LEAVE command.
*/
void QwsServerController::handleMessageLEAVE(const int roomId)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!rooms.contains(roomId)) {
        qDebug() << this << "Unable to part user from room"<<roomId<<" (does not exist)";
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    QwRoom *room = rooms[roomId];
    if (!room->pUsers.contains(user->user.userId())) {
        qDebug() << this << "Unable to part user from room"<<roomId<<" (user was not in room)";
        user->sendError(Qw::ErrorCommandFailed);
        return;
    }
    removeUserFromRoom(roomId, user->user.userId());
}


/*! This method is called when a user needs to be kicked/banned from the server by an administrator.
*/
void QwsServerController::handleMessageBAN_KICK(const int userId, const QString reason, const bool isBan)
{
    qDebug() << this << "Handling a BAN/KICK for user"<<userId<<"ban="<<isBan;
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }
    if (!m_sockets.contains(userId)) {
        qDebug() << this << "Unable to kick/ban user"<<userId<<" (does not exist)";
        user->sendError(Qw::ErrorClientNotFound);
        return;
    }
    QwsClientSocket *targetUser = m_sockets[userId];
    if (user->user.privileges().testFlag(Qws::PrivilegeCanNotBeKicked)) {
        qDebug() << this << "Unable to kick user - protected!";
        user->sendError(Qw::ErrorCannotBeDisconnected);
        return;
    }

    // Notify the other clients about this
    QwMessage reply(isBan ? "307" : "306");
    reply.appendArg(QString::number(userId));
    reply.appendArg(QString::number(user->user.userId()));
    reply.appendArg(reason);
    broadcastMessage(reply, 1, true);

    // Add it to the temporary banlist
    if (isBan) {
        m_banList.append(qMakePair(user->socket()->peerAddress().toString(),
                                 QDateTime::currentDateTime().addSecs(m_banDuration))); // 30 minutes
        qDebug() << this << "Adding address to banlist:" << user->socket()->peerAddress().toString() << "till" << QDateTime::currentDateTime().addSecs(60);
    }

    // Now do the justice
    targetUser->disconnectClient();
}


/*! This method is called when a user needs to be kicked/banned from the server by an administrator.
*/
void QwsServerController::handleModifiedUserAccount(const QString name)
{
    QHashIterator<int,QwsClientSocket*> i(m_sockets);
    while (i.hasNext()) {
        i.next();
        QwsClientSocket *item = i.value();
        if (!item) continue;
        if (item->user.loginName() == name) {
            // Reload the account
            item->user.loadFromDatabase();

            // Send new privileges
            QwMessage reply("602");
            item->user.appendPrivilegeFlagsForREADUSER(reply);
            item->sendMessage(reply);

            // Broadcast a change
            reply = QwMessage("304");
            item->user.userStatusEntry(reply);
            broadcastMessage(reply, 1, true);

            // Update the transfer sockets, if any
            QList<QwsClientTransferSocket*> transferSockets = this->transfersWithUserId(item->user.userId());
            QListIterator<QwsClientTransferSocket*> it(transferSockets);
            while (it.hasNext()) {
                QwsClientTransferSocket *transferSocket = it.next();
                if (!transferSocket) { continue; }
                if (transferSocket->info().type == Qw::TransferTypeDownload) {
                    transferSocket->setMaximumTransferSpeed(item->user.downloadSpeedLimit());
                } else {
                    transferSocket->setMaximumTransferSpeed(item->user.uploadSpeedLimit());
                }
            }



        }
    }


}


/*! This method is called when a user needs to be kicked/banned from the server by an administrator.
*/
void QwsServerController::handleModifiedUserGroup(const QString name)
{
    QHashIterator<int,QwsClientSocket*> i(m_sockets);
    while (i.hasNext()) {
        i.next();
        QwsClientSocket *item = i.value();
        if (!item) continue;
        if (item->user.groupName() == name) {
            // Reload the account
            item->user.loadFromDatabase();

            // Send new privileges
            QwMessage reply("602");
            item->user.appendPrivilegeFlagsForREADUSER(reply);
            item->sendMessage(reply);

            // Broadcast a change
            reply = QwMessage("304");
            item->user.userStatusEntry(reply);
            broadcastMessage(reply, 1, true);

            // Update the transfer sockets, if any
            QList<QwsClientTransferSocket*> transferSockets = this->transfersWithUserId(item->user.userId());
            QListIterator<QwsClientTransferSocket*> it(transferSockets);
            while (it.hasNext()) {
                QwsClientTransferSocket *transferSocket = it.next();
                if (!transferSocket) { continue; }

                if (transferSocket->info().type == Qw::TransferTypeDownload) {
                    transferSocket->setMaximumTransferSpeed(item->user.downloadSpeedLimit());
                } else {
                    transferSocket->setMaximumTransferSpeed(item->user.uploadSpeedLimit());
                }
            }
        }
    }

}


/*! GET - A client has requested a file from the server.
*/
void QwsServerController::handleMessageGET(const QwsFile file)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }

    // Add the request to the list of transfers
    QwsTransferInfo transfer;
    transfer.file = file;
    transfer.hash = QUuid::createUuid().toString();
    transfer.type = Qw::TransferTypeDownload;
//    transfer.offset = file.offset;
    transfer.transferSpeedLimit = user->user.downloadSpeedLimit();
    transfer.targetUserId = user->user.userId();
    transferPool->appendTransferToQueue(transfer);

    qwLog(tr("[%1] requested download of '%2' - assigned ID '%4'.")
          .arg(user->user.userId())
          .arg(transfer.file.remotePath()).arg(transfer.hash));

    checkTransferQueue(user->user.userId());


}


/*! PUT - A client has requested a file upload to the server.
*/
void QwsServerController::handleMessagePUT(const QwsFile file)
{
    QwsClientSocket *user = qobject_cast<QwsClientSocket*>(sender());
    if (!user) { return; }

    // Add the request to the list of transfers
    QwsTransferInfo transfer;
    transfer.file = file;
    transfer.hash = QUuid::createUuid().toString();
    transfer.type = Qw::TransferTypeUpload;
//    transfer.offset = file.offset; // the size of the existing partial file
    transfer.transferSpeedLimit = user->user.uploadSpeedLimit();
    transfer.targetUserId = user->user.userId();
    transferPool->appendTransferToQueue(transfer);

    qwLog(tr("[%1] requested upload of '%2' with ID '%3'.")
          .arg(user->user.userId())
          .arg(transfer.file.remotePath())
          .arg(transfer.hash));

    checkTransferQueue(user->user.userId());
}



void QwsServerController::checkTransferQueue(int userId)
{
    //qDebug() << this << "Checking transfer queue of" << userId;
    if (!m_sockets.contains(userId)) { return; }
    QwsClientSocket *socket = m_sockets[userId];
    if (!socket) { return; }

    // Active transfers are currently running transfers (connected ones).
    QList<QwsClientTransferSocket*> activeTransfers = transfersWithUserId(userId);
    // Waiting transfers are transfers that are ready, but where the transfer connection has not
    // yet been established.
    QList<QwsTransferInfo> waitingTransfers = transferPool->findWaitingTransfersWithUserId(userId);
    // Queued transfers are server-side queued tranfers that are not yet started.
    QList<QwsTransferInfo> queuedTransfers = transferPool->findTransfersWithUserId(userId);

    qDebug() << this << "User" << userId << "Active=" << activeTransfers.count() << "Queued=" << queuedTransfers.count();

    if (queuedTransfers.isEmpty()) {
        qDebug() << this << "No more queued transfers for:" << userId;
        return;
    };

    if (activeTransfers.count()+waitingTransfers.count() < m_maxTransfersPerClient) {
        // There are less transfers active than allowed. We can start the next transfers.
        // Take the next transfer from the stack and notify the client.
        QwsTransferInfo nextTransfer = transferPool->firstTransferWithUserId(userId);

        // Update the state of the transfer
        nextTransfer.state = Qw::TransferInfoStateRequested;
        transferPool->appendTransferToQueue(nextTransfer);

        // Notify the client of the ready transfer.
        qDebug() << this << "Sending transfer ready for transfer" << nextTransfer.hash;
        QwMessage reply("400"); // 400 - transfer ready

        QString effectiveFilePath = nextTransfer.file.remotePath();
        if (nextTransfer.type == Qw::TransferTypeUpload && effectiveFilePath.endsWith(".WiredTransfer")) {
            // Remove the suffix so the remote client can identify the transfer properly.
            effectiveFilePath.chop(14);
        }
        reply.appendArg(effectiveFilePath);
        reply.appendArg(QString::number(nextTransfer.file.offset));
        reply.appendArg(nextTransfer.hash);
        socket->sendMessage(reply);

    }


    // Notify the client of re-queueing of the remaining transfers (if any)
    int transferPosition = 1;
    QListIterator<QwsTransferInfo> i(waitingTransfers);
    while (i.hasNext()) {
        QwsTransferInfo item = i.next();
        if (item.state != Qw::TransferInfoStateQueued) { continue; }
        QwMessage reply("401"); // 401 - transfer queued

        QString effectiveFilePath = item.file.remotePath();
        if (item.type == Qw::TransferTypeUpload && effectiveFilePath.endsWith(".WiredTransfer")) {
            // Remove the suffix so the remote client can identify the transfer properly.
            effectiveFilePath.chop(14);
        }

        reply.appendArg(effectiveFilePath);
        reply.appendArg(QString::number(transferPosition++));
        socket->sendMessage(reply);
    }

}


void QwsServerController::handleTransferDone(const QwsTransferInfo transfer)
{
    Q_UNUSED(transfer);
    qDebug() << this << "Handle transfer done.";
    QwsClientTransferSocket *socket = qobject_cast<QwsClientTransferSocket*>(sender());
    if (!socket) { return; }

    qwLog(tr("[%1] completed download of '%2'.")
          .arg(socket->info().targetUserId)
          .arg(socket->info().file.remotePath()));

    if (transferSockets.contains(socket)) {
        // Update statistics
        if (socket->info().type == Qw::TransferTypeDownload) {
            statsTotalSent += socket->info().bytesTransferred-socket->info().file.offset;
        } else {
            statsTotalReceived += socket->info().bytesTransferred-socket->info().file.offset;
        }

        transferSockets.removeOne(socket);
        checkTransferQueue(socket->info().targetUserId);
        socket->deleteLater();
    }

}


void QwsServerController::handleTransferError(Qws::TransferSocketError error, const QwsTransferInfo transfer)
{
    Q_UNUSED(transfer);
    QwsClientTransferSocket *socket = qobject_cast<QwsClientTransferSocket*>(sender());
    if (!socket) { return; }

    qwLog(tr("[%1] failed transfer of '%2'.")
          .arg(socket->info().targetUserId)
          .arg(socket->info().file.remotePath()));

    if (transferSockets.contains(socket)) {
        transferSockets.removeOne(socket);
        if (error == Qws::TransferSocketErrorNetwork) {
            checkTransferQueue(socket->info().targetUserId);
        }
        socket->deleteLater();
    }

}


/*! Accepts a new transfer connection. Connected to the newConnect() signal of transferTcpServer.
*/
void QwsServerController::acceptTransferSslConnection()
{
    QSslSocket *newSocket = transferTcpServer->nextPendingSslSocket();
    QwsClientTransferSocket *transferSocket = new QwsClientTransferSocket(this);
    connect(transferSocket, SIGNAL(transferDone(QwsTransferInfo)),
            this, SLOT(handleTransferDone(QwsTransferInfo)));
    connect(transferSocket, SIGNAL(transferError(Qws::TransferSocketError,QwsTransferInfo)),
            this, SLOT(handleTransferError(Qws::TransferSocketError, QwsTransferInfo)));
    transferSocket->setSocket(newSocket);
    transferSocket->setTransferPool(transferPool);
    transferSockets.append(transferSocket);
    qwLog(tr("Accepted new transfer connection from %1").arg(newSocket->peerAddress().toString()));
}
