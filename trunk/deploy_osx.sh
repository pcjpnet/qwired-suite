#!/bin/sh

# If $1 = "upload" then upload it to the google store


# The directory to the Qt installation on OS X
QTDIR=/Developer/Applications/Qt/plugins

# The plugin dylibs that have to be shipped with the app
QTPLUGINS=$QTDIR/imageformats

# The paths to the tools
QTDEPLOY=contrib/deployment_mac/deployqt/deployqt

# The name of the DMG that will be created
DMGNAME=~/Desktop/qwired_mac_svn_`svnversion`.dmg


# --- NO PARAMS BEYOND THIS POINT ---
# -----------------------------------

echo "- Preparing qwired build to be deployed."
if [ -d bin/qwired.app ]; then
    echo "- Renaming qwired to Qwired.app"
    mv bin/qwired.app bin/Qwired.app

    echo "- Copying framework files to bundle"
    chmod ogu+x $QTDEPLOY
    $QTDEPLOY bin/Qwired.app $QTPLUGINS

    echo "- Packing"
    hdiutil create -format UDZO -srcdir bin -volname "Qwired r`svnversion`" $DMGNAME

    if [ "$1" == "upload" ]; then
      echo "- Uploading to Google"
      python contrib/googlecode_upload.py -p "qwired-suite" -s "Qwired Client Snapshot r`svnversion`" -l "Type-Package,OpSys-OSX,Version-Snapshot" -u bastibense $DMGNAME
    fi

    echo "- DMG moved to your Desktop!"
    exit 0

fi
echo "Stop: No application bundle found in bin."
