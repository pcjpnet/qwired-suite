#ifndef QWCGLOBALS_H
#define QWCGLOBALS_H

#include "singleton.h"
#include "QwcSingleton.h"

namespace Qwc {
    enum Event {
        ServerConnected,
        ServerDisconnected,
        ServerError,
        UserJoined,
        UserChangedNick,
        UserChangedStatus,
        UserLeft,
        ChatReceived,
        MessageReceived,
        NewsPosted,
        BroadcastReceived,
        TransferStarted,
        TransferFinished
    };
}

const QString QWIRED_VERSION = "0.1.0";

typedef Singleton<QwcSingleton> WSINGLETON;

#endif

