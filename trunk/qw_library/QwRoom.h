#ifndef QWROOM_H
#define QWROOM_H

#include <QList>
#include <QString>
#include <QDateTime>

#include "QwRoom.h"
#include "QwMessage.h"
#include "QwUser.h"

class QwRoom
{

public:
    QwRoom();
    ~QwRoom();

    int pChatId;
    QList<int> pUsers;
    QList<int> pInvitedUsers;
    QString pTopic;
    QwUser pTopicSetter;
    QDateTime pTopicDate;

};

#endif
