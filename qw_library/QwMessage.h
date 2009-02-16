#ifndef QWMESSAGE_H
#define QWMESSAGE_H

#include <QByteArray>
#include <QVariant>
#include <QStringList>
#include <QStringListIterator>
#include <QDataStream>

/*! \class QwMessage

    This class defines a message (frame) that is exchanged between the client and the server
    while the protocol is established. Basically, this class allows access to fields and values,
    stores a command name and generate the serialized representation of the data in the frame.
*/


class QwMessage : QObject
{

    Q_OBJECT

public:
    QwMessage();
    QwMessage(const QByteArray frameData);

    void appendArg(const QString data);
    QString getStringArgument(int index);

    QStringList arguments;
    QByteArray commandName;
    QByteArray generateFrame() const;
    void generateFrame(QDataStream stream) const;


};



#endif // QWMESSAGE_H

qRegisterMetaType<QwMessage>("QwMessage");
