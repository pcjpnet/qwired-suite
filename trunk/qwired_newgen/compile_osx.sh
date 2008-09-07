#!/bin/sh

echo "Qwired Build Script 1.0"
echo "-----------------------"

echo "- Preparing to build Qwired using:"
qmake -version

echo ""

if [ $? != 0 ]; then
  echo "Could not find qmake. You might have to install Qt4 (or fix your path if already installed).";
  exit 1;
fi

echo "- Cleaning up..."
make distclean &> /dev/null

echo "- Generating makefile (compiler log goes to make.log)..."
qmake -spec macx-g++ &> ./make.log

if [ $? != 0 ]; then
  echo "Running qmake failed. See make.log for more information.";
  exit 1;
fi

echo "- Compiling universal binary (takes up to 5 minutes on slow machines)..."
make -j2 &> ./make.log

if [ $? != 0 ]; then
  echo "Warning: Compilation failed. See make.log for more information.";
  exit 1;
else
  echo "- Completed. Revealing the build directory in Finder...";
  open ./bin/;
fi
