# Introduction #
This is a list of known bugs and issues in Qwired Client and Server.

### Notification sounds don't work on Linux (Ubuntu, etc.) ###
Qwired Client currently uses _QSound_ to play notification sounds. While this uses the native system API (like QuickTime or DirectMedia) on Mac OS X and Windows, it uses the old and deprecated NAS (Network Audio System) on Linux, which is not available in most recent Linux distributions. This is technically a Qt limitation and we don't believe it's worth to integrate Phonon and add additional bloat just to play some sounds.