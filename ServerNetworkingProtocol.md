# Introduction #
This draft describes a protocol extension to the Wired protocol that allows networking of servers.
Server networking allows servers to synchronize their user lists, chats, news and files; practically making multiple independent servers look like one big server to connected users.


## Session Handling ##
Servers connect to each others using the default port (TCP: 2000 and 2001) or a custom port. Authentication between servers is done using OpenSSL certificates to ensure integrity and security.

After successfully establishing a session connection, both servers send each other a list of connected clients and users, their status and other detailed information. Authentication of users can be done transparently (thus if a user connects to server A and has an account on server B, he will be able to log in).


## Files and Transfers ##
Implementing files and transfers is tricky. The easiest way would be to set up a files synchronization between the servers, because sending requested files through the server to a client would result in too much overhead.

A connected client sees the files of the server it is connected to, not the files on the other servers in the grid.


## Sychronization of Servers ##
The following features need to be implemented to have a smooth server networking experience:
  * Synchronization of user lists between servers
  * Synchronzation and broadcasting login/logouts between servers
  * Transparently synchronizing user IDs between servers and passing command to other servers
  * Netsplit-handling (if the connection between servers fail; the servers itself will continue to work)
  * Automatic reconnecting of servers after a netsplit