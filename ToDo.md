On this page is a rough overview of things that should be done for a 1.0 release of both the server and client software.


---

## High Priority ##
### Client ###
  * Allow auto-resume after reconnecting to a server.
  * Fix events in the client (sounds, messages, etc.)
  * Fix broadcast message feature for both sending and receiving.
  * Create a default bookmark on first start, also a default tracker.
  * Implement basic preview-feature for files.

### Server ###
  * Implement logging to syslog, file or stdout (or allow to disable it).
  * Folder type is not sent during file listings.


---

## Medium Priority ##
### Client ###
  * Allow user to set large Qwired-like icons in preferences

### Server ###
  * Implement WebKit support for the chat widget (possibly with support for Adium styles?)


---

## Low Priority ##

### Tracker ###
  * Implement tracker protocol into server to use it as a tracker
  * Extend protocol, allow banners in tracker listings