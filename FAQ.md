

## What platforms does Qwired Client/Server run on? ##
Qwired Client and Server are targeted towards the following operating systems. Since Qwired's source code does not depend on other libraries than Qt 4.6, all Qt-supported platforms should be working.

  * Mac OS X 10.4 (Tiger), 10.5 (Leopard), 10.6 (Snow Leopard) both PowerPC and Intel CPUs (Universal)
  * Windows XP, 2000, Vista, 7
  * Any modern X86 Linux (Debian, Ubuntu, etc.)

If you manage to compile Qwired Client/Server on any other platforms, please let us know!

## Where can I download the latest version? ##
You can find the latest nightly builds here: http://qwired.neo.de/nightlies/

## Can I compile Qwired Client/Server myself? ##
Yes. Please see [BuildingFromSVN](BuildingFromSVN.md) for instructions.
Please note that you **don't have** to compile Qwired yourself - instead you can use the nightly builds.

## Where can I find a stable version of Qwired Server? ##
Qwired Server is still in heavy development and requires more internal testing. It is included in the nightly builds and can be tested by any user. Please note that Qwired Server might be unstable and crash - so please do NOT use it in production environments, or where stability and security are a critical factor.

## Is Qwired Client/Server compatible with Mac OS X 10.3? ##
No. We kept compatibility as long as possible. Starting with Qt 4.6 it is not supported by our development tools anymore - and we are not going to put any extra effort in supporting a 6 year old, deprecated operating system.
If you have an old machine standing around that you'd like to use as a Qwired-server, please upgrade that machine to at least Mac OS X 10.4 or 10.5.