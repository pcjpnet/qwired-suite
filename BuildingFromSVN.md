# Building the latest Qwired Snapshot from SVN #

## If you just want to test the latest changes and features, you can simply downloads a pre-compiled nightly build for your platform at http://qwired.neo.de/nightlies/! ##


---


## Installing SVN and Checking out Qwired ##
In order to compile Qwired, you have to check out the latest source code from the Subversion/SVN tree.

### Linux/Ubuntu ###

Subversion is provided in the standard repositories.

  1. Open a Terminal and type the following commands:
  1. `sudo apt-get install subversion`   _(you will be asked for your password)_
  1. `cd ~`
  1. `svn co http://qwired-suite.googlecode.com/svn/trunk/ qwired-suite`

The directory `~/qwired-suite` now contains the current source code and some information about the SVN connection itself. The checkout only needs to be done once. After that you just update your copy of the source code by doing the following:

  1. `cd ~/qwired-suite`
  1. `svn update`
  1. `make clean`

If you happen to have problems (especially while translating), clean up the SVN tree by typing:

  1. `cd ~/qwired-suite`
  1. `svn -R revert .`


### Windows ###
To check out the source under Windows, you have to install a small SVN client. The best client so far is TortoiseSVN.

  1. Download TortoiseSVN from http://tortoisesvn.net/downloads and install it.
  1. Reboot (if you did not do that already).
  1. On the Desktop, create a new folder called "qwired-suite".
  1. Right click on the new folder and select "SVN Checkout..." from the context menu.
  1. In the check out window enter the URL: `http://qwired-suite.googlecode.com/svn/trunk/`
  1. Click "OK"
  1. If you do not have OpenSSL installed already (Microsoft thought it would be a good idea to NOT ship it with Windows), download and install it (around 1MB) from http://www.slproweb.com/download/Win32OpenSSL_Light-0_9_8l.exe

The checkout process might take a short while because it has to download all the sources from the SVN server. The checkout has to be done only once.

If you want to update to the latest source code, just right click on the QwiredSF folder and click "SVN Update". Obviously, after updating to a newer version of the source code, you'll have to compile it again by issuing the "make" command.

If you have problems compiling or translating, right-click on the folder and select "Check for Modifications...". You will be presented a list of modified items. Revert them and try again.

**Note:** For Windows Vista users, there is a fix available for MinGW. Please [read this article](http://sourceforge.net/forum/forum.php?forum_id=782021) and download the updated version of gcc from there.


### Mac OS X ###
There are plenty of graphical Subversion clients for Mac OS X (such as SmartSVN). But for this, the command line client should do the job.

  1. Download SVN from http://downloads.open.collab.net/binaries.html and install it. Open a Terminal and execute the following commands:
  1. `cd ~/`
  1. `svn co http://qwired-suite.googlecode.com/svn/trunk/ qwired-suite`

Checking out the source code will take some time, get yourself a bowl of milk and continue reading.


---


## Compiling ##
### Linux (Ubuntu >=9.10) ###
Installation on Ubuntu is trivial, most tools required to build Qwired are provided by the package repositories. We need libgstreamer in order to build in support for media (notification sounds, etc.).

Unfortunately Ubuntu 9.10 does not ship with Qt 4.6. You have to build it yourself (with Phonon and WebKit support).

  1. Open a terminal and execute:
  1. See Installing SVN and Checking out Qwired
  1. `cd ~/qwired-suite/`
  1. `qmake`
  1. `make` _(if you have 2 or more CPU cores, append '-j2' or '-j4')_


### Mac OS X ###
Compilation on Mac OS X requires you to install the Developer Tools (Xcode).

  1. Install Xcode/Developer tools on Mac OS X (can be optained for free from Apple's Developer Connection).
  1. Download the Qt4-framework as precompiled package from Trolltech's FTP (the latest 4.3.x will do): [ftp://ftp.trolltech.com:21//qt/source/qt-mac-opensource-4.3.4.dmg](ftp://ftp.trolltech.com:21//qt/source/qt-mac-opensource-4.3.4.dmg)
  1. Install the package (default install should be fine).
  1. See Installing SVN and Checking out Qwired
  1. `cd ~/qwired-suite/`
  1. `sh compile_osx.sh`

The compile\_osx.sh script will execute the needed commands to compile the source code. If errors occour, check the contents of the file _make.log_.


### Windows ###
Installation on Windows is very similar to Mac OS X.

  1. Download the Qt4-framework (compiler is included) from: [ftp://ftp.trolltech.com:21//qt/source/qt-win-opensource-4.3.4-mingw.exe](ftp://ftp.trolltech.com:21//qt/source/qt-win-opensource-4.3.4-mingw.exe) or, alternatively here: http://trolltech.com/developer/downloads/qt/windows
  1. Install the framework and the compiler you just downloaded.
  1. See Installing SVN and Checking out Qwired
  1. Right-click on "My Computer" and select Properties.
  1. Click the "Advanced" tab, then the "Environment Variables" button.
  1. In the "System Variables" list, find the "Path" variable and double-click it.
  1. Add `C:\MinGW\bin;C:\Qt\4.3.4\bin` to the end of the value of the variable. Don't forget to separate any previous values by using the **;** character.
  1. Click OK and close the properties window.
  1. Open a Shell/Terminal.
  1. `cd Desktop\qwired-suite\`
  1. `qmake`
  1. `make`

The compiler should now do its job and compile the latest Qwired source code. This will take some moments. You will find a qwired.exe in the bin directory after the compilation finishes.