#!/bin/sh


QTDEPLOY=contrib/deployment_mac/deployqt/deployqt
DMGNAME=~/Desktop/qwired_mac_svn_`svnversion`.dmg

echo "- Preparing qwired build to be deployed."
if [ -d bin/qwired.app ]; then
    echo "- Renaming qwired to Qwired.app"
    mv bin/qwired.app bin/Qwired.app

    echo "- Copying framework files to bundle"
    chmod ogu+x $QTDEPLOY
    $QTDEPLOY bin/Qwired.app

    echo "- Packing"
    hdiutil create -format UDZO -srcdir bin -volname "Qwired r`svnversion`" $DMGNAME

    echo "- DMG moved to your Desktop!"
    exit 0

fi
echo "Stop: No application bundle found in bin."
