#include "QwMessage.h"
#include <QtDebug>

QwMessage::QwMessage()
{
}


QwMessage::QwMessage(const QByteArray frameData)
{
    QString frameArguments;
    int spacePosition = frameData.indexOf('\x20');
    if (spacePosition > -1) {
        // Frame contains a space - separate command form rest.
        commandName = QString::fromUtf8(frameData.left(spacePosition));
        frameArguments = QString::fromUtf8(frameData.mid(spacePosition + 1));
    } else {
        // Frame contains command only.
        commandName = QString::fromUtf8(frameData);
    }

    if (!frameArguments.isEmpty()) {
        // Parse the frame arguments
        arguments = frameArguments.split('\x1C');
    }

}


/*! Append an argument to the list of arguments of this message.
*/
QwMessage& QwMessage::appendArg(const QString data)
{
    arguments.append(data);
    return *this;
}

QwMessage & QwMessage::appendArg(qlonglong number)
{
    arguments.append(QString::number(number));
    return *this;
}

QwMessage & QwMessage::appendArg(const QByteArray &ba)
{
    arguments.append(QString::fromUtf8(ba));
    return *this;
}

/*! Return the argument at position \a index or an empty value. The first item is 0.
*/
QString QwMessage::stringArg(int index) const
{
    QString result = arguments.value(index);
    if (result.isNull()) {
        return "";
    } else {
        return result;
    }
}


/*! Generate the serialized version of the data contained in this message object and return it.
*/
QByteArray QwMessage::generateFrame() const
{
    QByteArray buffer;
    buffer += commandName.toUtf8();
    buffer += '\x20'; // space

    // append the arguments
    QStringListIterator i(arguments);
    while (i.hasNext()) {
        QString argument = i.next();
        buffer += argument.toUtf8();
        if (i.hasNext()) {
            buffer += '\x1C'; // FS
        }
    }
    return buffer;
}


/*! Uses generateFrame() to generate a serialized version of the message and writes it to the
    data stream \a stream including the EOT-character.
*/
void QwMessage::generateFrame(QDataStream stream) const
{
    stream << this->generateFrame();
    stream << (uchar)0x04;
}
