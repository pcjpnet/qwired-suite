

# Introduction #
Most of the protocol is based on the Qwired protocol version 1.1 by Axel Andersson. This copy of the protocol specification aims to be a more accurate version of the original specification, and describes the extensions and changes that Qwired utilizes.

# Abstract #
The Qwired protocol is an application-level protocol for real time chat, messaging and file transfers. It is based on the client-server model, with many clients connected to one and the same server, allowing both public and private messaging. The main design principles are standardization and the unification of messaging and file transfer systems.

# Purpose #
The Qwired protocol is an application-level protocol for file transfers and user interaction. It is intended to fill a gap among Internet class protocols that either provide transfer services or chat services, usually mutually exclusively. Qwired joins these two types together to provide a complete user experience.

This specification defines the protocol referred to as "Protocol 1.1".
This protocol includes additional commands and server responses, to convey more information to the client.

# Extensions #
Qwired implementes a number of additional features, thus extends the original protocol with additional commands. Please see [QwiredProtocolVersion30](QwiredProtocolVersion30.md) for more information.

# Terminology #
  * **client** - A program that establishes connections for the purpose of sending commands and interpreting messages.
  * **command** - A command sent from a client to a server.
  * **identifier** - A unique 3-digit number that describes the type of message.
  * **field** - A part of a command or a message that conveys a particular pre-defined type of information.
  * **message** - A message sent from a server to a client.
  * **server** -  A program that accepts connections in order to service commands by sending back messages.


# Overall Operation #
The Qwired protocol is a textual command/message protocol. A client sends a command in a specific format to the server, and the server
interprets this command, and if needed, sends back a message with
requested information.

Some messages are sent asynchronously, meaning they are sent without having first received a command. These messages can be sent at any given time, and a client should be prepared to handle these as they come.

A single connection ("control connection") is used to send these commands and messages back and forth. A separate connection is used for transfers, in order to maintain the control connection while a transfer is running.

Qwired communication takes place over a TCP/IP connection using TLS. The default port is TCP 2000, but other ports can be used. The transfer port is the default port incremented by one, or 2001 by default.


# Protocol Compatibility #
This specification adds extended functionality to the previous version. In order to interoperate with implementations, a server or client must take care to be as compatible as possible.
The main thing to note is that the number of fields in commands and messages are not static; they will change between protocol versions.

To be compatible, a client should accept messages with more fields than the protocol version it was writter for defined, and a server should accept commands with only a partial set of defined fields.

The number of fields will, however, never go down. Fields will always retain their ordering. Should a field be obsoleted, it will remain and go unused.

The following commands were added in version 1.1:
  * [#BANNER](#BANNER.md)
  * [#COMMENT](#COMMENT.md)
  * [#STATUS](#STATUS.md)
  * [#TOPIC](#TOPIC.md)
  * [#TYPE](#TYPE.md)

The following messages were added in version 1.1:
  * [#203\_Server\_Banner](#203_Server_Banner.md)
  * [#340\_Client\_Image\_Change](#340_Client_Image_Change.md)
  * [#341\_Chat\_Topic](#341_Chat_Topic.md)

Commands and messages that have gained additional fields are marked where they are documented.


# Access Authentication #
Qwired requires all clients to provide authentication in order to log in to a server. This authentication consists of a login name and an optional password. Servers wishing to provide anonymous logins should provide a "guest" user with no password. Clients should use this login pair as the default.

Passwords are SHA-1 checksums of the actual passwords. No cleartext must be sent over the connection.

See [#Accounts](#Accounts.md) for more information on accounts.


# Protocol Parameters #
## Identification Numbers ##
The protocol uses identification numbers for a number of object types. These are unsigned integers.

  * ID = 1\*DIGIT

An identification number is assigned to each connected user, starting with 1 and incremented for each new user. In certain situations, the server software itself may participate in communications. When thus, the server assumes the user id 0.

An id number is also assigned to each new chat created. Unlike user ids however, these should be assigned randomly from the integer range used. The server should also take care to verify that each client communicating with a certain chat, is actually present on that chat, to prevent unauthorized eavesdropping.

The public chat is the chat available to everyone and the chat that is automatically joined upon logging in. The public chat is assigned the chat id 1.


## Control Characters ##
The following control characters from the US-ASCII table are used:

  * EOT    = `<US-ASCII EOT, end of transmission (4)>`
  * FS     = `<US-ASCII FS, file separator (28)>`
  * GS     = `<US-ASCII GS, group separator (29)>`
  * RS     = `<US-ASCII US, record separator (30)>`
  * SP     = `<US-ASCII SP, space (32)>`


## Strings ##
All string contents are sent in Unicode, UTF-8 encoding. There is no way to select a character set. Clients must be UTF-8 aware if they wish to participate in communications:

  * STRING    = `<any 8-bit sequence of data in UTF-8 encoding>`


## Base64 Data ##
In cases where binary data needs to be transmitted on the control connection, it is Base64 encoded.

  * BASE64    = `<base64 encoded binary data>`


## Booleans ##
When a value can be considered as an on/off switch, a boolean value  is used:

  * BOOLEAN    = `"0" | "1"`


## Version Strings ##
Version strings are used to convey the version of either the protocol or the applications involved. The protocol version string uses a fixed format:

  * protocol-version    = `1*DIGIT "." 1*DIGIT`

The application version string has a different format:

  * app-version    = `name "/" 1*DIGIT "." 1*DIGIT ["." 1*DIGIT] SP "(" os ")" [ SP "(" lib-version [";" SP lib-version] ")" ]`
  * name           = STRING

  * os             = `os-release ";" SP os-version ";" SP arch`
  * os-release     = STRING
  * os-version     = STRING
  * arch           = STRING

  * lib-version    = STRING

That is, the application version must always be followed by the operating system version, and optionally, by the versions of additional software used. For example:

`Qwired/1.0 (Darwin; 7.2.0; powerpc) (OpenSSL 0.9.7b 10 Apr 2003)`

Note that there is no particular format defined for the operation system version strings.


## Addresses ##
IP addresses are sent in textual representation, in the formats described in RFC 790 and RFC 1884:

  * IP    = STRING


## Date & Time Format ##
Date are sent in the ISO 8601 full "date-time" format, as per RFC 3339:

  * date-fullyear     = 4DIGIT
  * date-month        = 2DIGIT  ; 01-12
  * date-mday         = 2DIGIT  ; 01-28, 01-29, 01-30, 01-31
  * time-hour         = 2DIGIT  ; 00-23
  * time-minute       = 2DIGIT  ; 00-59
  * time-second       = 2DIGIT  ; 00-58, 00-59, 00-60
  * time-secfrac      = "." 1\*DIGIT
  * time-numoffset    = ("+" / "-") time-hour ":" time-minute
  * time-offset       = "Z" / time-numoffset

  * partial-time      = time-hour ":" time-minute ":" time-second [time-secfrac]
  * full-date         = date-fullyear "-" date-month "-" date-mday
  * full-time         = partial-time time-offset

  * date-time         = full-date "T" full-time

An example of such a date is:

`1996-12-19T16:39:57-08:00`

This represents 39 minutes and 57 seconds after the 16th hour of December 19th, 1996 with an offset of -08:00 from UTC.

## Privilege Specifications ##
Privilege specifications are sent in the following format:

  * privileges = `get-user-info FS broadcast FS post-news FS clear-news FS download FS upload FS upload-anywhere FS create-folders FS alter-files FS delete-files FS view-dropboxes FS create-accounts FS edit-accounts FS delete-accounts FS elevate-privileges FS kick-users FS ban-users FS cannot-be-kicked FS download-speed FS upload-speed FS download-limit FS upload-limit FS change-topic`

  * get-user-info         = BOOLEAN
  * broadcast             = BOOLEAN
  * post-news             = BOOLEAN
  * clear-news            = BOOLEAN
  * download              = BOOLEAN
  * upload                = BOOLEAN
  * upload-anywhere       = BOOLEAN
  * create-folders        = BOOLEAN
  * alter-files           = BOOLEAN
  * delete-files          = BOOLEAN
  * view-dropboxes        = BOOLEAN
  * create-accounts       = BOOLEAN
  * edit-accounts         = BOOLEAN
  * delete-accounts       = BOOLEAN
  * elevate-privileges    = BOOLEAN
  * kick-users            = BOOLEAN
  * ban-users             = BOOLEAN
  * cannot-be-kicked      = BOOLEAN
  * download-speed        = 1\*DIGIT
  * upload-speed          = 1\*DIGIT
  * download-limit        = 1\*DIGIT   ; Protocol 1.1
  * upload-limit          = 1\*DIGIT   ; Protocol 1.1
  * change-topic          = BOOLEAN   ; Protocol 1.1

This is a series of boolean values, indicating whether a user can or cannot perform certain commands.

See [#Accounts](#Accounts.md) for more information on accounts. See [#Privilege\_Specifications](#Privilege_Specifications.md) for more information on the file related privileges.


# Sequences #

## Login Sequence ##
The login sequence takes place directly after the client has established a connection with the server.

  1. The client initiates the sequence by sending the "[#HELLO](#HELLO.md)" command.
  1. The server responds with the [#200\_Server\_Information](#200_Server_Information.md) or [#511\_Banned](#511_Banned.md) message.
  1. The client sends the "[#NICK](#NICK.md)", "[#ICON](#ICON.md)", "[#STATUS](#STATUS.md)" and "[#CLIENT](#CLIENT.md)" commands to set values before becoming visible to other clients. Of these, "[#NICK](#NICK.md)" is required, "[#CLIENT](#CLIENT.md)" is optional but highly recommended, and "[#ICON](#ICON.md)" and "[#STATUS](#STATUS.md)" are optional.
  1. The clients sends the "[#USER](#USER.md)" command with the login name. `"guest"` should be used as the default login name.
  1. The client sends the "[#PASS](#PASS.md)" command with the checksum of the actual password. If the password is empty, no check-summing should be performed.
  1. The server responds with the [#201\_Login\_Succeeded](#201_Login_Succeeded.md) or [#510\_Login\_Failed](#510_Login_Failed.md) messages. If the login was successful, the client has now been added to the public user list and is available to other clients. The server will also reply a [#341\_Chat\_Topic](#341_Chat_Topic.md) message for the public chat, if a topic has been set.
  1. The client requests the user list using "[#WHO](#WHO.md)", with a value of `"1"` for the public user list.
  1. The server responds with the [#310\_User\_List](#310_User_List.md) and [#311\_User\_List\_Done](#311_User_List_Done.md) messages.
  1. Optionally, the client requests the active privileges mask using "[#PRIVILEGES](#PRIVILEGES.md)".
  1. The server responds with the [#602\_Privileges](#602_Privileges.md) message.
  1. Optionally, the client requests the news using "[#NEWS](#NEWS.md)".
  1. The server responds with the [#320\_News](#320_News.md) and [#321\_News\_Done](#321_News_Done.md) messages.


## Private Chat Sequence ##
The private chat sequence takes place when a user requests a private chat audience with another user.

  1. Client A initiates the sequence by sending the "PRIVCHAT" command.
  1. The server creates a private chat and responds with the 330 Private Chat Created message. Client A is added to the private chat.
  1. Client A invites client B to the private chat by sending the "INVITE" message, with the chat id of the new chat and the user id of client B.
  1. The server sends the 331 Private Chat Invitation message to client B, with the chat id of the new chat and the user id of client A.
  1. Client B sends a "JOIN" or "DECLINE" command with the chat id of the new chat. Following the "JOIN" command, client B sends a "WHO" command, requesting the user list of the private chat.
  1. In case of "JOIN", the server adds client B to the new chat and sends out 302 Client Join messages to the user list of the private chat. In addition, the server replies with a 341 Chat Topic message. The server also replies to "WHO" and sends the user list of the private chat to client B. Client A and client B can now communicate in a closed private chat. Other clients can be added by sending out new "INVITE" commands. In case of "DECLINE", the server sends out 332 Private Chat Declined messages to the user list of the private chat.
  1. When done, client A and client B send the "LEAVE" command to leave the private chat.
  1. The server sends out 303 Client Leave messages to the user list of the private chat.

# Files #

## File Listings ##

The following parameters are sent in file related messages:

  * file-type      = 1\*DIGIT  ; 0-3
  * folder-type    = 1\*DIGIT  ; 1-3

The file type is mapped by the server as follows:

  * 0 File
  * 1 Directory
  * 2 Uploads Directory
  * 3 Drop Box Directory

Note that for the directory types, the server should send the count of enclosed object as "size" in the listing messages.

Uploads directories and drop boxes are special directories that tie in with the permissions listed in section 5. Uploads and drop box directories  can be uploaded into by accounts with the "upload" privilege; the "upload-anywhere" privilege is required to upload into
a regular directory. Having uploads privileges should also enable "create-folders", but only in the directories where the client can upload.

Drop box directories can only be viewed by accounts with the "view-dropboxes" privilege; for others, the directory should appear with an empty listing.

## Transfers ##

Because servers should supporting queueing and resuming transfers, these sequences are a bit more complicated than usual.

Of note is the so-called Qwired checksum. These should be SHA-1 checksums of the first megabyte (1048576 octets) of the file. This
is defined to allow checksumming of very large files in real time, and to allow checksums of partially transferred files to match,
provided that they exceed 1 MB in size.

**Sequence, for downloads:**

  1. The client decides to download a file. If the file has been partially downloaded already, the client should issue a "STAT" command to get the Qwired checksum of the file, and compare that to the Qwired checksum of the local file. If the match is successful, the "GET" command should be issued with the appropriate offset.
  1. The server retains the download request, and may [issue 401](https://code.google.com/p/qwired-suite/issues/detail?id=401) Transfer Queued messages until a transfer slot is found. When found, the server issues 400 Transfer Ready, with a unique random textual key string to identify the transfer.
  1. The client retains the key received in the 400 messages, connects to the server's transfer port, and sends the "TRANSFER" command with the key to identify which transfer.
  1. If the key matches the queued transfer, the server begins sending the file; otherwise, the connection is closed.

**And, for uploads:**

  1. The client decides to upload a file, and sends the "PUT" command with the appropriate Qwired checksum and file size.
  1. The server checks if a partially transferred file exists at the location the client specified. If so, compares the Qwired checksum of that file to the one given by the client. If a fully transferred file exists at that path, a 521 File or Directory Exists should be issued. If the transfer is to be accepted, the server may begin sending out 401 Transfer Queued messages until a slot is found. Then, the 400 Transfer Ready is issued, with a unique key.
  1. Just as for downloads, the client connects to the transfer port, sends the "TRANSFER" command, and begins sending the file.

# Accounts #

There are two principal types of Qwired accounts, users and groups.

User accounts are the primary authentication mechanism, with login and password pairs, just like most systems. When the client has
authenticated, it is assigned a privilege mask by the server, which consists of all the values defined in section in 2.9. A client
should retain this mask by sending the "PRIVILEGES" command and save the resulting message. A client should not attempt to perform
actions that it knows it cannot complete.

The server needs to keep track of the mask, and update it accordingly for logged in clients when, for example, an edit accounts request
completes.

As mentioned in section 1.4 and elsewhere, the default account "guest" should be provided when server administrators wish to provide an
anonymous account.

User accounts can be assigned to group accounts. Group accounts are similar to user accounts, but lack the "group" and "password" fields,
for obvious reasons. Assigning a user account to a group is just a matter of specifying the group's name for the "group" field. If
the "group" field is empty, the user account belongs to no group.

When a user account is assigned to a group, the user account's privileges are ignored, and the group account's are used instead.
Thus, if user A has no limits on either download or upload rates, but group 1 has limits in place, assigning user A to group 1 will
result in all logins for user A to have limits.

# Commands #

## Command Format ##
A command from a client to a server includes a unique command, optionally followed by a single space and arguments. The command is  terminated with EOT.

  * command     = name [argument](SP.md) EOT
  * name        = (one of the documented commands below)

## Command Listing ##

### BAN ###
`"BAN" SP user FS message EOT`

  * user       = ID
  * message    = STRING

Ban the client "user" with the message "message". Performs the same action as "KICK", but also installs a temporary ban on "user". The duration of the time is user-defined.

Subject to the privilege "ban-users".

On failure, may return the errors 512, 515 or 516.

### BANNER ###
`"BANNER" EOT`

Request the server banner.

On success, returns a 203.

### BROADCAST ###
`"BROADCAST" SP message EOT`

  * message    = STRING

Send the broadcast message "message" to all connected users.

Subject to the privilege "broadcast".

On success, causes a 309.

On failure, may return the error 516.

### CLEARNEWS ###
`"CLEARNEWS" EOT`

Empty the news.

Subject to the privilege "clear-news".

On failure, may return the error 516.

### CLIENT ###
`"CLIENT" SP app-version EOT`

Send the client version information.

See section 2.6 for a specification of "app-version".

See section 3.1 for more information on the login sequence.

### COMMENT ###
`"COMMENT" SP path FS comment EOT`

  * path       = STRING
  * comment    = STRING

Set the comment of the file or folder at "path" to "comment".

Subject to the privilege "alter-files".

On failure, may return the errors 516 or 521.

### CREATEUSER ###
`"CREATEUSER" SP name FS password FS group FS privileges EOT`

  * name        = STRING
  * password    = STRING
  * group       = STRING

Create the user account "name", as a member of "group",
which can be empty.

See section 2.9 for a specification of "privileges".

Subject to the privilege "create-accounts".

On failure, may return the errors 514 or 516.

### CREATEGROUP ###
`"CREATEGROUP" SP name FS privileges EOT`

  * name    = STRING

Create the group account "name".

See section 2.9 for a specification of "privileges".

Subject to the privilege "create-accounts".

On failure, may return the errors 514 or 516.

### DECLINE ###
`"DECLINE" SP chat EOT`

  * chat    = ID

Decline a chat invitation to "chat".

### DELETE ###
`"DELETE" SP path EOT`

  * path    = STRING

Delete the file or folder at "path". All deletes are recursive.

Subject to the privilege "delete-files".

On failure, may return the errors 516 or 520.

### DELETEUSER ###
`"DELETEUSER" SP name EOT`

  * name    = STRING

Delete the user account "name".

Subject to the privilege "delete-accounts".

On failure, may return the errors 513 or 516.

### DELETEGROUP ###
`"DELETEGROUP" SP name EOT`

  * name    = STRING

Delete the group account "name".

Subject to the privilege "delete-accounts".

On failure, may return the errors 513 or 516.

### EDITUSER ###
`"EDITUSER" SP name FS password FS group FS privileges EOT`

  * name        = STRING
  * password    = STRING
  * group       = STRING

Modify the user account "name", setting new values or privileges. A user account cannot be renamed, so "name" must already exist.
The group membership can be changed by editing "group", which can be empty.

See section 2.9 for a specification of "privileges".

Subject to the privilege "edit-accounts".

On failure, may return the errors 513 or 516.

### EDITGROUP ###
`"EDITGROUP" SP name FS privileges EOT`

  * name    = STRING

Modify the group account "name", setting new privileges. A group account cannot be renamed, so "name" must already exist.

See section 2.9 for a specification of "privileges".

Subject to the privilege "edit-accounts".

On failure, may return the errors 513 or 516.

### FOLDER ###
`"FOLDER" SP path EOT`

  * path    = STRING

Create a new folder at "path".

Subject to the privilege "create-folders".

On failure, may return the errors 516 or 521.

### GET ###
`"GET" SP path FS offset EOT`

  * path      = STRING
  * offset    = 1\*DIGIT

Request a download of the file or folder "path", starting from byte
offset "offset". See section 4 for more details on transfers.

Subject to the privilege "download".

On success, returns a 400 or 401.

On failure, may return the errors 516, 520 or 523.

### GROUPS ###
`"GROUPS" EOT`

Get a listing of all the group accounts on the server.

Subject to the privilege "edit-accounts".

On success, returns a chain of 620 and a terminating 621.

On failure, may return the error 516.

### HELLO ###
`"HELLO" EOT`

Start a conversation with a server. See section 3.1 for more
information on the login sequence.

On success, returns a 200.

On failure, may return the error 511.

### ICON ###
`"ICON" SP icon FS image EOT`

  * icon     = 1\*DIGIT
  * image    = BASE64    ; Protocol 1.1

Change the icon to "icon", and the custom image to "image".

On success, causes a 304.

### INFO ###
`"INFO" SP user EOT`

  * user    = ID

Request client information for "user".

Subject to the privilege "get-user-info".

On success, returns a 308.

On failure, may return the errors 512 or 516.

### INVITE ###
`"INVITE" SP user FS chat EOT`

  * user    = ID
  * chat    = ID

Invite the client "user" to the chat "chat". See section 3.2 for more
information on private chat sequences.

### JOIN ###
`"JOIN" SP chat EOT`

  * chat    = ID

Join the chat "chat". See section 3.2 for more information on private
chat sequences.

### KICK ###
`"KICK" SP user FS message EOT`

  * user       = ID
  * message    = STRING

Kick the client "user" with the message "message".

Subject to the privilege "kick-users".

On success, causes a 306.

On failure, may return the errors 512 or 516.

### LEAVE ###
`"LEAVE" SP chat EOT`

  * chat    = ID

Leave the chat "chat". See section 3.2 for more information on private
chat sequences.

### LIST ###
`"LIST" SP path EOT`

  * path    = STRING

List the file contents at "path".

On success, returns a chain of 410 and a terminating 411.

On failure, may return the error 520.

### ME ###
`"ME" SP chat FS message EOT`

  * chat       = ID
  * message    = STRING

Send the action chat "message" to "chat".

On success, causes a 301.

### MOVE ###
`"MOVE" SP from FS to EOT`

  * from    = STRING
  * to      = STRING

Move the file or folder at "from" to "to".

Subject to the privilege "alter-files".

On failure, may return the errors 516, 520 or 521.

### MSG ###
`"MSG" SP user FS message EOT`

  * user       = ID
  * message    = STRING

Send the private message "message" to the user "user".

On failure, may return the errors 512.

### NEWS ###
`"NEWS" EOT`

Request the news.

On success, returns a chain of 320 and a terminating 321.

### NICK ###
`"NICK" SP nick EOT`

  * nick    = STRING

Change nick to "nick".

On success, causes a 304.

### PASS ###
`"PASS" SP password EOT`

  * password    = STRING

Send password. See section 3.1 for more information on the login
sequence.

On success, returns a 201.

On failure, may return the error 510.

### PING ###
`"PING" EOT`

Ping the server and let it return a 202 message. Should not affect the
idle time.

On success, returns a 202.

### POST ###
`"POST" SP message EOT`

  * message    = STRING

Post "message" to the news.

On success, causes a 322.

Subject to the privilege "post-news".

On failure, may return the error 516.

### PRIVCHAT ###
`"PRIVCHAT" EOT`

Create a new chat on the server.

On success, returns a 330.

### PRIVILEGES ###
`"PRIVILEGES" EOT`

Request the current privilege mask. See section 3.1 for more
information on the login sequence.

On success, returns a 602.

### PUT ###
`"PUT" SP path FS size FS checksum EOT`

  * path        = STRING
  * size        = 1\*DIGIT
  * checksum    = STRING

Request an upload of the file to be located at the remote path
"path", with size "size" and the checksum "checksum". See
section 4 for more details on transfers.

Subject to the privilege "upload".

On success, returns a 400 or 401.

On failure, may return the errors 516, 521, 522 or 523.

### READUSER ###
`"READUSER" SP name EOT`

  * name    = STRING

Get the account specification for the user "name".

Subject to the privilege "edit-accounts".

On success, returns a 600.

On failure, may return the error 516.

### READGROUP ###
`"READGROUP" SP name EOT`

  * name    = STRING

Get the account specification for the group "name".

Subject to the privilege "edit-accounts".

On success, returns a 601.

On failure, may return the error 516.

### SAY ###
`"SAY" SP chat FS message EOT`

  * chat       = ID
  * message    = STRING

Send the chat "message" to "chat".

On success, causes a 300.

### SEARCH ###
`"SEARCH" SP query EOT`

  * query    = STRING

Search the files for filenames containing "query".

On success, returns a chain of 420 and a terminating 421.

### STAT ###
`"STAT" SP path EOT`

Get the file information for the file or folder "path".

On success, returns a 402.

### STATUS ###
`"STATUS" SP status EOT`

  * status    = STRING

Change the status message to "status"

On success, causes a 304.

### TOPIC ###
`"TOPIC" SP chat FS topic EOT`

  * chat     = ID
  * topic    = STRING

Set the topic of "chat" to "topic".

If "chat" is the public chat, subject to the privilege
"change-topic".

On success, causes a 341.

On failure, may return the error 516.

### TRANSFER ###
`"TRANSFER" SP hash EOT`

  * hash    = STRING

Identify the transfer requested. See section 4 for more information
on files.

### TYPE ###
`"TYPE" SP path FS type EOT`

  * path    = STRING
  * type    = folder-type

Set the type of the folder at "path" to "type".

Subject to the privilege "alter-files".

On failure, may return the errors 516 or 520.

### USER ###
`"USER" SP login EOT`

  * login    = STRING

Send login name. See section 3.1 for more information on the login
sequence.

### USERS ###
`"USERS" EOT`

Get a listing of all the user accounts on the server.

Subject to the privilege "edit-accounts".

On success, returns a chain of 610 and a terminating 611.

On failure, may return the error 516.

### WHO ###
`"WHO" SP chat EOT`

  * chat   = ID

Get the user list for "chat".

On success, returns a chain of 310 and a terminating 311.


# Server Messages #

## Message Format ##
A message from a server to a client includes a unique three-digit number, followed by a space, then optional fields, and a terminator.

  * message       = identifier [argument](SP.md) EOT
  * identifier    = (one of the messages below)

The first digit of the identifier defines the class of response. The  last two digits do not have any categorization role. There are five  values for the first digit:

  * 2xx Information
  * 3xx Chat, news, messaging
  * 4xx Files, transfers
  * 5xx Errors
  * 6xx Administrative

## 200 Class Messages ##

### 200 Server Information ###
`"200" SP app-version FS protocol-version FS server-name FS server-description FS start-time FS files-count FS files-size`

  * server-name           = STRING
  * server-description    = STRING
  * start-time            = date-time
  * files-count           = 1\*DIGIT    ; Protocol 1.1
  * files-size            = 1\*DIGIT    ; Protocol 1.1

Basic information about the server.

See section 2.6 for a specification of "app-version" and
"protocol-version".

In response to "HELLO", or sent asynchronously.

### 201 Login Succeeded ###
`"201" SP user EOT`

  * user    = ID

Login succeeded, client was assigned user id "user".

In response to "PASS".

### 202 Ping Reply ###
`"202" SP "Pong" EOT`

In response to "PING".

### 203 Server Banner ###
`"203" SP image EOT`

  * image    = BASE64

Server banner. The image data is contained in "image".

In response to "BANNER".

## 300 Class Messages ##

### 300 Chat ###
`"300" SP chat FS user FS message EOT`

  * chat       = ID
  * user       = ID
  * message    = STRING

Standard chat message from "user" on "chat". The chat message is
contained within "message ".

`user ":" SP message`

### 301 Action Chat ###
`"301" SP chat FS user FS message EOT`

  * chat       = ID
  * user       = ID
  * message    = STRING

Action chat message from "user" on "chat". The chat message is
contained within "message". Typical output format:

`"***" SP user SP message`

### 302 Client Join ###
`"302" SP chat FS user FS idle FS admin FS icon FS nick FS login FS ip FS host FS status FS image EOT`

  * chat      = ID
  * user      = ID
  * idle      = BOOLEAN
  * admin     = BOOLEAN
  * icon      = 1\*DIGIT
  * nick      = STRING
  * login     = STRING
  * ip        = IP
  * host      = STRING
  * status    = STRING    ; Protocol 1.1
  * image     = BASE64    ; Protocol 1.1

The client "user" joined "chat". If "chat" is the public chat, the
client has joined the server.

The rest of the fields are basic information on the client.

### 303 Client Leave ###
`303 SP chat FS user EOT`

  * chat     = ID
  * user     = ID

The client "user" left "chat". If "chat" is the public chat, the
client has left the server.

### 304 Status Change ###
`"304" SP user FS idle FS admin FS icon FS nick FS status EOT`

  * user      = ID
  * idle      = BOOLEAN
  * admin     = BOOLEAN
  * icon      = 1\*DIGIT
  * nick      = STRING
  * status    = STRING    ; Protocol 1.1

The client "user" changed status. Any number of the given fields
have changed.

### 305 Private Message ###
`"305" SP user FS message EOT`

  * user        = ID
  * message     = STRING

The client "user" sent the receiver a private message. The actual
message is contained within "message".

### 306 Client Kicked ###
`"306" SP victim FS killer FS message EOT`

  * victim      = ID
  * killer      = ID
  * message     = STRING

The client "victim" was kicked by "killer", who commented with
"message".

### 307 Client Banned ###
`"307" SP victim FS killer FS message EOT`

  * victim      = ID
  * killer      = ID
  * message     = STRING

The client "victim" was banned by "killer", who commented with
"message".

### 308 Client Information ###
`"308" SP user FS idle FS admin FS icon FS nick FS login FS ip FS host FS client-version FS cipher-name FS cipher-bits FS login-time FS idle-time FS downloads FS uploads FS status FS image EOT`

  * user              = ID
  * idle              = BOOLEAN
  * admin             = BOOLEAN
  * icon              = 1\*DIGIT
  * nick              = STRING
  * login             = STRING
  * ip                = IP
  * host              = STRING

  * client-version    = [application-version]
  * cipher-name       = [STRING](STRING.md)
  * cipher-bits       = 1\*DIGIT
  * login-time        = date-time
  * idle-time         = date-time
  * downloads         = **(transfer [GS](GS.md))
  * uploads           =**(transfer [GS](GS.md))
  * status            = STRING    ; Protocol 1.1
  * image             = BASE64    ; Protocol 1.1

  * transfer          = path RS transferred RS size RS speed
  * path              = STRING
  * transferred       = 1\*DIGIT
  * size              = 1\*DIGIT
  * speed             = 1\*DIGIT

Extended client information for "user".

"cipher-name" and "cipher-bits" describe the name of the encryption
cipher the client connected using, and the strength in bits of said
cipher. These fields may be empty or zero, respectively.

"login-time" and "idle-time" respectively describe when the client
joined the server, and when the client was last active.

"downloads" and "uploads" are optional fields that contain any number
of "transfer", inter-spaced with RS. These fields may be empty.

In response to "INFO".

### 309 Broadcast Message ###
`"309" SP user FS message EOT`

  * user        = ID
  * message     = STRING

A server-wide broadcast message received from "user". The actual
message is contained within "message".

### 310 User List ###
`"310" SP chat FS user FS idle FS admin FS icon FS nick FS login FS ip FS host FS status FS image EOT`

  * chat      = ID
  * user      = ID
  * idle      = BOOLEAN
  * admin     = BOOLEAN
  * icon      = 1\*DIGIT
  * nick      = STRING
  * login     = STRING
  * ip        = IP
  * host      = STRING
  * status    = STRING    ; Protocol 1.1
  * image     = BASE64    ; Protocol 1.1

A client in the user list. Same message format as 302.

The user list comes sorted by the time each client joined,
descending.

In response to "WHO".

### 311 User List Done ###
`"311" SP chat EOT`

  * chat    = ID

End of user list for "chat".

In response to "WHO".

### 320 News ###
`"320" SP nick FS post-time FS post EOT`

  * nick         = STRING
  * post-time    = date-time
  * post         = STRING

A news post, by "nick", posted at "post-time". The actual post
is contained within "post".

The news comes sorted by the time it was posted, ascending.

In response to "NEWS".

### 321 News Done ###
`"321" SP "Done" EOT`

End of news transfer.

In response to "NEWS".

### 322 News Posted ###
`"322" SP nick FS post-time FS post EOT`

  * nick         = STRING
  * post-time    = date-time
  * post         = STRING

A new news post. Same message format as 320.

### 330 Private Chat Created ###
`"330" SP chat EOT`

  * chat     = ID

A new private chat has been created on the server, and the received
has been added to it.

See section 3.2 for more information on private chat sequences.

In response to "PRIVCHAT".

### 331 Private Chat Invitiation ###
`"331" SP chat FS user EOT`

  * chat     = ID
  * user     = ID

The client "user" has invited the received to the private chat
"chat".

See section 3.2 for more information on private chat sequences.

### 332 Private Chat Declined ###
`"332" SP chat FS user EOT`

  * chat     = ID
  * user     = ID

The client "user" declined the invitation to the chat "chat".

See section 3.2 for more information on private chat sequences.

### 340 Client Image Change ###
`"340" SP user FS image EOT`

  * user      = ID
  * image     = BASE64

The client "user" has changed its custom image. Image data
available in "image".

### 341 Chat Topic ###
`"341" SP chat FS nick FS login FS ip FS time FS topic EOT`

  * chat      = ID
  * nick      = STRING
  * login     = STRING
  * ip        = IP
  * time      = date-time
  * topic     = STRING

The topic for the chat "chat" was set by "nick", "login", "ip"
at "time". The actual topic is contained in "topic".

In reponse to "JOIN" and "PASS".

## 400 Class Messages ##

### 400 Transfer Ready ###
`"400" SP path FS offset FS hash EOT`

  * path      = STRING
  * offset    = 1\*DIGIT
  * hash      = STRING

The transfer of "path" is ready to begin. "hash" is a unique
identifier for this particular transfer.

See section 4 for more information on files.

In response to "GET" and "PUT".

### 401 Transfer Queued ###
`"401" SP path FS position EOT`

  * path        = STRING
  * position    = 1\*DIGIT

The transfer of "path" has been queued in line at "position". If
"position" is greater than 1, additional 401 messages will be sent
until the position reaches 0, at which point a 400 message will be
sent.

See section 4 for more information on files.

In response to "GET" and "PUT".

### 402 File Information ###
`"402" SP path FS type FS size FS created FS modified FS checksum FS comment EOT`

  * path        = STRING
  * type        = file-type
  * size        = 1\*DIGIT
  * created     = date-time
  * modified    = date-time
  * checksum    = STRING
  * comment     = STRING    ; Protocol 1.1

Extended file information for "path".

See section 4 for more information on files.

In response to "STAT".

### 410 File Listing ###
`"410" SP path FS type FS size FS created FS modified EOT`

  * path        = STRING
  * type        = file-type
  * size        = 1\*DIGIT
  * created     = date-time    ; Protocol 1.1
  * modified    = date-time    ; Protocol 1.1

A file in the file listing.

The file listing comes sorted by filename, descending.

See section 4 for more information on files.

In response to "LIST".

### 411 File Listing Done ###
`"411" SP path FS free EOT`

  * path    = STRING
  * free    = 1\*DIGIT

End of file listing for "path". "free" contains the number of octets
available in the file system that "path" resides on. This number
should only be sent for paths where the receiver has uploads
privileges, otherwise a value of zero should be sent. It is up to the
client to determine whether this zero means a full file system or a
lack of privileges.

In response to "LIST".

### 420 Search Listing ###
`"420" SP path FS type FS size FS created FS modified EOT`

  * path        = STRING
  * type        = file-type
  * size        = 1\*DIGIT
  * created     = date-time    ; Protocol 1.1
  * modified    = date-time    ; Protocol 1.1

A file in the search listing. Same message format as 410.

The sorting of the search listing is undefined.

See section 4 for more information on files.

In response to "SEARCH".

### 421 Search Listing Done ###
`"421" SP "Done" EOT`

End of search listing.

In response to "SEARCH".

## 500 Class Messages ##

### 500 Command Failed ###
`"500" SP "Command Failed" EOT`

An undefined internal error prevented the command from completing.

### 501 Command Not Recognized ###
`"501" SP "Command Not Recognized" EOT`

The command was not recognized.

### 502 Command Not Implemented ###
`"502" SP "Command Not Implemented" EOT`

The command has not been implemented by the server.

### 503 Syntax Error ###
`"503" SP "Syntax Error" EOT`

There was a syntax error in the command.

### 510 Login Failed ###
`"510" SP "Login Failed" EOT`

The login could not complete, the login and/or password provided did
not match a valid entry.

### 511 Banned ###
`"511" SP "Banned" EOT`

The login could not complete, the user is banned.

### 512 Client Not Found ###
`"512" SP "Client Not Found" EOT`

The server could not find the client referred to.

### 513 Account Not Found ###
`"513" SP "Account Not Found" EOT`

The server could not find the account referred to.

### 514 Account Exists ###
`"514" SP "Account Exists" EOT`

The account referred to already exists.

### 515 Cannot Be Disconnected ###
`"515" SP "Cannot Be Disconnected" EOT`

The user attempted to kick or ban another user that has the privilege
"cannot-be-disconnected".

### 516 Permission Denied ###
`"516" SP "Permission Denied" EOT`

The command could not complete, the client lacks sufficient
privileges.

### 520 File or Directory Not Found ###
`"520" SP "File or Directory Not Found" EOT`

The file or directory referred to could not be found.

### 521 File or Directory Exists ###
`"521" SP "File or Directory Exists" EOT`

There already exists a file or directory at the path referred to.

### 522 Checksum Mismatch ###
`"522" SP "Checksum Mismatch" EOT`

The two checksums do not match.

### 523 Queue Limit Exceeded ###
`"523" SP "Queue Limit Exceeded" EOT`

The client has exhausted its per-client transfer limit. The transfer
cannot be entered into the server queue.

## 600 Class Messages ##

### 600 User Specification ###
`"600" SP name FS password FS group FS privileges EOT`

  * name        = STRING
  * password    = STRING
  * group       = STRING

A specification of a user account.

See section 2.9 for a specification of "privileges".

See section 5 for more information on accounts.

In response to "READUSER".

### 601 Group Specification ###
`"601" SP name FS privileges EOT`

  * name    = STRING

A specification of a group account.

See section 2.9 for a specification of "privileges".

See section 5 for more information on accounts.

In response to "READGROUP".

### 602 Privileges Specification ###
`"602" SP privileges EOT`

The privileges mask for the receiver.

See section 2.8 for a specification of "privileges".

See section 5 for more information on accounts.

In response to "PRIVILEGES".

### 610 User Listing ###
`"610" SP name EOT`

  * name    = STRING

A user account in the user accounts listing.

The sorting of the user accounts listing is undefined.

See section 5 for more information on accounts.

In response to "USERS".

### 611 User Listing Done ###
`"611" SP "Done" EOT`

End of user listing.

In response to "USERS".

### 620 Group Listing ###
`"620" SP name EOT`

  * name    = STRING

A group account in the group accounts listing.

The sorting of the group accounts listing is undefined.

See section 5 for more information on accounts.

In response to "GROUPS".

### 621 Group Listing Done ###
`"621" SP "Done" EOT`

End of group listing.

In response to "GROUPS".