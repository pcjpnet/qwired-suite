#!/bin/sh


QTDEPLOY=contrib/deployment_mac/deployqt/deployqt
DMGNAME=~/Desktop/qwired_linux_svn_`svnversion`.tar.gz

echo "- Preparing qwired build to be deployed."
if [ -e bin/qwired ]; then
    echo "- Packing"
    tar -zcf $DMGNAME bin/qwired INSTALL README
	echo "- Done"
    exit 0

fi
echo "Stop: No build found in bin."
