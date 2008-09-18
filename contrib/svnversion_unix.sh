#!/bin/sh

SRC="./general/svn_revision.h"
SVNREV=`svnversion`

echo "Setting up header file for revision: $SVNREV"
echo "" > $SRC
echo "#ifndef SVNREVISION_H" >> $SRC
echo "#define SVNREVISION_H" >> $SRC
echo "" >> $SRC
echo "#define SVN_REVISION \"$SVNREV\"" >> $SRC
echo "" >> $SRC
echo "#endif" >> $SRC
