#ifndef QWGLOBALS_H
#define QWGLOBALS_H

namespace Qw {
    enum ProtocolError {
        ErrorCommandFailed = 500,
        ErrorCommandNotRecognized,
        ErrorCommandNotImplemented,
        ErrorSyntaxError,
        ErrorLoginFailed = 510,
        ErrorBanned,
        ErrorClientNotFound,
        ErrorAccountNotFound,
        ErrorAccountExists,
        ErrorCannotBeDisconnected,
        ErrorPermissionDenied,
        ErrorFileOrDirectoryNotFound = 520,
        ErrorFileOrDirectoryExists,
        ErrorChecksumMismatch,
        ErrorQueueLimitExceeded
    };
}

#endif // QWGLOBALS_H
