#include "QwcMessageStyle.h"

#include <QtDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

QwcMessageStyle::QwcMessageStyle()
{
}


/*! Load a message style from disk. \a path points to the topmost directory of the style, which
    contains the \a Contents folder.
    \return Returns true if successful.
*/
bool QwcMessageStyle::loadStyle(const QString &path)
{
    QDir targetDir(path);

    if (!targetDir.exists()) {
        qDebug() << this << "Unable to load style directory:" << path;
        return false;
    }

    // Load the style files
    QFile inputContentFile(targetDir.absoluteFilePath("Contents/Incoming/Content.html"));
    if (inputContentFile.open(QIODevice::ReadOnly)) {
        incomingMessageHtml = QString::fromUtf8(inputContentFile.readAll());
        qDebug() << this << "Loaded" << inputContentFile.fileName();
    }

    QFile inputNextContentFile(targetDir.absoluteFilePath("Contents/Incoming/NextContent.html"));
    if (inputNextContentFile.open(QIODevice::ReadOnly)) {
        incomingNextMessageHtml = QString::fromUtf8(inputNextContentFile.readAll());
        qDebug() << this << "Loaded" << inputNextContentFile.fileName();
    }

    // Load the style files
    QFile outputContentFile(targetDir.absoluteFilePath("Contents/Outgoing/Content.html"));
    if (outputContentFile.open(QIODevice::ReadOnly)) {
        incomingMessageHtml = QString::fromUtf8(outputContentFile.readAll());
        qDebug() << this << "Loaded" << outputContentFile.fileName();
    }

    QFile outputNextContentFile(targetDir.absoluteFilePath("Contents/Outgoing/NextContent.html"));
    if (outputNextContentFile.open(QIODevice::ReadOnly)) {
        incomingNextMessageHtml = QString::fromUtf8(outputNextContentFile.readAll());
        qDebug() << this << "Loaded" << outputNextContentFile.fileName();
    }

    qDebug() << this << "Style loaded";
    styleRootPath = path;
    return true;
}
