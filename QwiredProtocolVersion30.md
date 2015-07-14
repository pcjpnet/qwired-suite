

# Introduction #
Qwired provides a number of additional features, and therefore requires a number of additional commands.

This specification effectively extends the [Qwired Protocol 1.1](QwiredProtocolVersion11.md).

Older clients should log in and request an older protocol version, so that basic connectivity is guranteed.

# Protocol Parameters #
## Privilege Specifications ##
Privilege specifications are sent in the following format:

  * privileges
    1. get-user-info FS
    1. broadcast FS
    1. post-news FS
    1. clear-news FS
    1. download FS
    1. upload FS
    1. upload-anywhere FS
    1. create-folders FS
    1. alter-files FS
    1. delete-files FS
    1. view-dropboxes FS
    1. create-accounts FS
    1. edit-accounts FS
    1. delete-accounts FS
    1. elevate-privileges FS
    1. kick-users FS
    1. ban-users FS
    1. cannot-be-kicked FS
    1. download-speed FS
    1. upload-speed FS
    1. download-limit FS
    1. upload-limit FS
    1. change-topic FS
    1. **server-console**

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
  * **server-console        = BOOLEAN   ; Protocol 3.0**

This is a series of boolean values, indicating whether a user can or cannot perform certain commands.

See section 5 for more information on accounts. See section 4 for more information on the file related privileges.

# Commands #
## Command Listing ##

### ABORT ###
`"ABORT" SP tid EOT`

  * tid = STRING

Abort a running or server-queued file transfer.

On failure, may return the error 516.

Subject to _server-console_ privilege.

### CONFIGSET ###
`"CONFIGSET" SP key FS value EOT`

  * key = STRING
  * value = BASE64

Set a configuration parameter in the database.

On failure, may return the error 516.

Subject to _server-console_ privilege.

### CONFIGGET ###
`"CONFIGGET" SP key EOT`

  * key = STRING

Read a configuration parameter in the database.

On failure, may return the error 516, on success returns a 815 Configuration Value message.

Subject to _server-console_ privilege.

### LOG ###
`"LOG" SP enable EOT`

  * enable = BOOL

Enable real-time transmission of log messages to the client.

On failure, may return the error 516, or begins transmission of 800 Log Message messages.

Subject to _server-console_ privilege.

### REINDEX ###
`"REINDEX" EOT`

Re-Index the files database.

On failure, may return the error 516.

Subject to _server-console_ privilege.

### SHUTDOWN ###
`"SHUTDOWN" EOT`

Shut down the server daemon.

On failure, may return the error 516.

Subject to _server-console_ privilege.

### STATISTICS ###
`"STATISTICS" EOT`

Request server statistics.

On failure, may return the error 516; on success a 814 Server Statistics message.

Subject to _server-console_ privilege.

### TRANSFERS ###
`"TRANSFERS" EOT`

Get a list of active file transfers on the server.

On failure, may return the error 516; on success a list of 810 Transfer List and a final 811 Transfer List Done message.

Subject to _server-console_ privilege.

# Messages #

## 800 Class Messages ##

### 800 Log Message ###
`"800" SP timestamp FS type FS origin FS text EOT`

  * timestamp = date-time
  * type = STRING
  * origin = STRING
  * text = STRING

A server log message has been emitted by _origin_, a string that identifies the source of the information (server core, user, etc.). _text_ contains the information about the event. _type_ defines the type of the message, which can be "INFO", "WARN", "FATAL" or "DEBUG".

This message is subject to the _LOG_ command.

### 810 Transfer List ###
`"810" SP status FS tid FS type FS user FS path FS done FS total FS speed EOT`

  * status = 1\*DIGIT
  * tid = STRING
  * type = 1\*DIGIT
  * user = ID
  * path = STRING
  * done = 1\*DIGIT
  * total = 1\*DIGIT
  * speed = 1\*DIGIT

Specifies a single active or queued transfer on the server. _status_ indicates the status of the transfer where 0 indicates a server-queued transfer, and 1 indicates a running transfer. _tid_ is a unique UUID for the specific transfer. type is the type of transfer, where 0 indicates a download (to the client), and 1 indicates an upload (to the server). _path_ is the file (on the server file system) being transferred. _done_, _total_ and _speed_ define the amount of bytes already transferred, to be transferred and being transferred per second.

In response to `TRANSFERS`.

### 811 Transfer List Done ###
`"811" SP "Done" EOT`

End of the transfer list.

In response to `TRANSFERS`.


### 814 Server Statistics ###
`"814" SP users FS transfers FS uploads-speed FS downloads-speed FS total-sent FS total-received EOT`

  * users = 1\*DIGIT
  * transfers = 1\*DIGIT
  * uploads-speed = 1\*DIGIT
  * downloads-speed = 1\*DIGIT
  * total-sent = 1\*DIGIT
  * total-received = 1\*DIGIT

In response to `STATISTICS`.


### 815 Configuration Value ###
`"815" SP key FS value EOT`

  * key = STRING
  * value = BASE64

In response to `CONFIGGET`.