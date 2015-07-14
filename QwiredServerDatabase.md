# Qwired Server: Database Structure #

# Database Structure #

The initial database structure is created when Qwired Server launches for the first time and there is no database file. You can find the SQL schema here:

http://code.google.com/p/qwired-suite/source/browse/trunk/qw_server/resources/server_db.sql

## Tables ##
  * **qws\_config** - Contains all configuration key/value pairs. See table below for more information.
  * **qws\_news** - Contains all news post entries.
  * **qws\_files** - Contains temporary indexing information about files that are available in the server files-root. This is used to speed up remote searching of files.
  * **qws\_groups** - Contains all user account groups.
  * **qws\_accounts** - Contains all user account information, might reference to entries in _qws\_groups_ if a user account is part of a group.

# Database Keys #

| **Configuration Key** | **Description** | **Example** |
|:----------------------|:----------------|:------------|
|`server/banlist`       |A list of semicolon (;) separated wildcards of IP-addresses that are permanently banned from the server. |`10.0.0.*;192.168.*.*` |
|`server/name`          |A short name for the server. |`Qwired Server` |
|`server/description`   |A one-line description for the server. This will show up in tracker listings. |`This is my great server.` |
|`server/port`          |The TCP-port the server should listen on. By default this is 2000, 2001 is the port for file transfers, and 2002 the port for tracker connections. |`2000`       |
|`server/address`       |The local IP address the server should listen on. |`127.0.0.1`  |
|`server/certificate_data` |The SSL certificate (PEM) that is used for encryption. |n/a          |
|`files/root`           |The path to the files directory of the server. These files will be offered to connected clients. |`files`      |


## Privileges Flags ##
Every account or group entry contains a mask field which defines the user's privileges.

| **Privilege** | **Key** |
|:--------------|:--------|
| get-user-info | i       |
| broadcast     | b       |
| post-news     | p       |
| clear-news    | C       |
| download      | d       |
| upload        | u       |
| upload-anywhere | U       |
| create-folders | d       |
| alter-files   | a       |
| delete-files  | r       |
| view-dropboxes | D       |
| create-accounts | A       |
| edit-accounts | M       |
| delete-accounts | R       |
| elevate-privileges | E       |
| kick-users    | K       |
| ban-users     | B       |
| cannot-be-kicked | P       |
| change-topic  | t       |
| server-console _(new in protocol 3.0)_ | x       |