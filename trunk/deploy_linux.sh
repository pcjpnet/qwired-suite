#!/bin/sh


QTDEPLOY=contrib/deployment_mac/deployqt/deployqt
DMGNAME=~/Desktop/qwired_linux_svn_`svnversion`.tar.gz

echo "- Preparing qwired build to be deployed."
if [ -e bin/qwired ]; then
    echo "- Packing"
    tar -zcf $DMGNAME bin/qwired INSTALL README
    echo "- Uploading to Google"
    python contrib/googlecode_upload.py -p "qwired-suite" -s "Qwired Client Snapshot r`svnversion`" -l "Type-Archive,OpSys-Linux,Version-Snapshot" -u bastibense $DMGNAME
	echo "- Done"
    exit 0

fi
echo "Stop: No build found in bin."
