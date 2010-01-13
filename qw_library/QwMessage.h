#ifndef QWMESSAGE_H
#define QWMESSAGE_H

#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QStringListIterator>
#include <QtCore/QDataStream>

/*! \class QwMessage

    This class defines a message (frame) that is exchanged between the client and the server
    while the protocol is established. Basically, this class allows access to fields and values,
    stores a command name and generate the serialized representation of the data in the frame.
*/


class QwMessage
{

public:
    QwMessage();
    QwMessage(const QByteArray frameData);

    QwMessage & appendArg(const QString data);
    QwMessage & appendArg(qlonglong number);
    QwMessage & appendArg(const QByteArray &ba);

    QString stringArg(int index) const;

    QStringList arguments;
    QString commandName;
    QByteArray generateFrame() const;
    void generateFrame(QDataStream stream) const;


};

Q_DECLARE_METATYPE(QwMessage);


#endif // QWMESSAGE_H
