#ifndef QWSUSER_H
#define QWSUSER_H

#include "QwUser.h"

class QwsUser : public QwUser
{

public:
    QwsUser();

    void appendPrivilegeFlagsForREADUSER(QwMessage &message);
    void setPrivilegesFromEDITUSER(QwMessage &message, int fieldOffset);

    void privilegesFlags(QwMessage &message) const;

    void userListEntry(QwMessage &message) const;
    void userStatusEntry(QwMessage &message) const;
    void userInfoEntry(QwMessage &message) const;

    // Database storage
    bool loadFromDatabase();
    bool writeToDatabase();
    bool deleteFromDatabase();



};

#endif // QWSUSER_H