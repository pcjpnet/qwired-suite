# Introduction #

This page describes the protocol extensions to the latest Wired 1.1 protocol. Most of the protocol semantics are the same, some commands become obsolete and are replaced by more powerful commands.

The protocol is internally identified by the version number 3.0. Because of the risk of having a 2.x protocol for the original Wired, the version 3.0 is used. Right now it is not likely that a "original" 3.0 protocol will exist, because Zanka Software is developing a completely new, XML-based protocol for the future.


## PREVIEW Command ##

The _PREVIEW_ command allows a client to receive a scaled down version of a media file. Because of the large amount of CPU needed for this to work, previews should be generated on the server and stored permanently. Also video files are unlikely to be supported right away.

### Syntax ###
A _PREVIEW_ command is sent to the server, which will respond with a
```
PREVIEW <path> EOT
```

### Server Response ###
```
460 <path> <data> EOT
```


# Tracker Subprotocol #

## In-Bound UPDATE ##
The in-bound UPDATE command allows updating of server statistics using an in-bound TLS-TCP connection instead of the dirty UDP encryption approach.

  * **UPDATE**
    * hash = STRING
    * users = 1\*DIGIT
    * guest-ok = BOOLEAN
    * guest-download-ok = BOOLEAN
    * files-count = 1\*DIGIT
    * files-size = 1\*DIGIT