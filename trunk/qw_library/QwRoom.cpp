#include "QwRoom.h"

/*! \class QwRoom
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-08
    \brief This class defines a chat (room). It manages the list of users, the topic and some other
      information about the room.
*/

QwRoom::QwRoom()
{
    pChatId = 0;
    pTopic = "(no topic)";
    pTopicDate = QDateTime::currentDateTime().toTimeSpec(Qt::UTC);
    pTopicSetter.setNickname("Qwired");
}

QwRoom::~QwRoom()
{
}

