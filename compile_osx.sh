#!/bin/sh

QW_CLIENT_PRO=QwiredSuite.pro
QW_BUILD_DIR=build_mac

echo "Qwired Build Script 1.1"
echo "-----------------------"

echo "- Preparing to build Qwired using:"
qmake -version

echo "- Project: $QW_CLIENT_PRO"
echo "- Build directory: $QW_BUILD_DIR"

echo ""
echo "Okay... Let's see... *think* *think*"
echo ""

sleep 1

echo "- Creating temporary directory - we are doing our magic in $QW_BUILD_DIR"
mkdir "$QW_BUILD_DIR"
cd "$QW_BUILD_DIR"


if [ $? != 0 ]; then
  echo "Could not find qmake. You might have to install Qt4 (or fix your path if already installed).";
  exit 1;
fi

echo "- Cleaning up..."
make distclean &> /dev/null

echo "- Generating makefile..."
qmake -spec macx-g++ -Wnone "CONFIG+=debug_and_release" "../$QW_CLIENT_PRO"

if [ $? != 0 ]; then
  echo "Running qmake failed. See make.log for more information.";
  exit 1;
fi

echo "- Compiling universal binary (takes up to 5 minutes on slow machines)..."
echo "  There will be a lot of text - but that is OK - unless it suddenly stops with a error message."
echo "  (starting in 3 seconds)"
echo ""
sleep 3
make -j4

if [ $? != 0 ]; then
  echo "Warning: Compilation failed. See make.log for more information.";
  exit 1;
else
  echo "- Completed. Revealing the build directory in Finder... Enjoy your fresh Qwired! :-)";
  echo "  If you want to send your fresh Qwired to your friends, you will have to prepare it"
  echo "  for deployment. You can easily do that by executing the ./deploy_osx.sh script."
  sleep 4
  open bin;
fi
