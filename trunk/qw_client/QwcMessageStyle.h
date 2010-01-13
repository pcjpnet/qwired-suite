#ifndef QWCMESSAGESTYLE_H
#define QWCMESSAGESTYLE_H

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDateTime>
#include <QtGui/QFont>

class QWebFrame;
class QwUser;

class QwcMessageStyle
{
public:
    QwcMessageStyle();

    bool loadStyle(QString path);

    const QString& chatTemplate() const;

    QUrl styleBaseUrl() const;

    static bool clearChat(QWebFrame *frame);
    static bool addChatMessage(QWebFrame *frame, const QwUser &user, const QString &text, bool isEmote);
    static bool addStatusMessage(QWebFrame *frame, const QString &text);
    static bool addBroadcastMessage(QWebFrame *frame, const QwUser &user, const QString &text);

    static bool setTopicContents(QWebFrame *frame, const QString &text, const QString &user,
                                 QDateTime dateTime);
    static bool setStyleOverride(QWebFrame *frame, const QString &css);

    static QString replaceEmoticons(const QString &text, bool enableEmoticons);
    static QString cssFromFont(const QFont &font);

private:
    QString m_stylePath;
    QString m_chatTemplate;

};

#endif
