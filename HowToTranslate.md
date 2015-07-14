# Introduction #

Qwired uses Qt's translation engine to allow easy localization of the user interface. This page describes the steps neccessary to localize Qwired into your language.


# Details #

## Step 1: Download the Qt-SDK ##
First you'll need the Qt SDK, which can be downloaded [here](http://qtsoftware.com/downloads/). The SDK contains Qt Linguist, the translation utility which is needed for translating Qwired. Also you it might be useful to install Apple's Xcode.

## Step 2: Download the localization files (.ts) ##
After that you'll need a prepared translation file for your language. If you don't have a Subversion client, you'll find the most recent TS files [here](http://code.google.com/p/qwired-suite/source/browse/trunk/qw_client#qw_client/lang).

You'll find two kinds of files: the ".ts"-files, which are relevant for you, and their respective ".qm"-file - which is a automatically generated version of the ".ts"-files (you don't have to edit those!).

Please make sure that you always download the latest translation files before you begin translating. Others might have modified or fixed the translation files in the meantime!
SVN is there for a reason, please use it.


## Step 3: Translate ##
The most important step is, of course, the translating itself. Qt offers a convenient way of doing this: Qt Linguist.
In Step 1 you downloaded the Qt-SDK, which includes the Qt Linguist application. This application allows you to open the ".ts"-files directly.

The process is pretty much straightforward. Please refer to the provided Qt Linguist Help (Hint: Help menu!) for more details on the user interface and the translation process.

## Step 4: Upload the updated translation file (patch) ##
To make the translation files available to everyone else, you'll have to upload the updated translation files. Unless you have write-access to the SVN-tree (right now there are just two people with write access), you should attach the file to the respective issue:

  * [Dutch](http://code.google.com/p/qwired-suite/issues/detail?id=27)
  * [Swedish](http://code.google.com/p/qwired-suite/issues/detail?id=46)
  * [Japanese](http://code.google.com/p/qwired-suite/issues/detail?id=28)

Please note that creating a patch instead of uploading the whole file really helps to speed up the process of merging your changes into the official code tree. See HowToProvidePatches for a detailed guide on creating patches.

**Obviously the list above is not complete. Please open an issue for your respective language - we'll react to it by creating a ".ts"-file which you can edit!**