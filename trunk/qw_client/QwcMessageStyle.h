#ifndef QWCMESSAGESTYLE_H
#define QWCMESSAGESTYLE_H

#include <QString>

class QwcMessageStyle
{

public:
    QwcMessageStyle();
    bool loadStyle(const QString &path);


    /*! The root path to the topmost directory of the style. */
    QString styleRootPath;

    /*! The HTML snippet that is supposed to display a single message from a user. */
    QString incomingMessageHtml;
    /*! The HTML snippet that should be added if a message from the same person follows. */
    QString incomingNextMessageHtml;

    /*! The HTML snippet that is supposed to display a single outgoing message. */
    QString outgoingMessageHtml;
    /*! The HTML snippet that should be added if a second message is sent. */
    QString outgoingNextMessageHtml;
    
};

#endif
