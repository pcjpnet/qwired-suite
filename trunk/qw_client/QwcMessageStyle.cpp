#include "QwcMessageStyle.h"

#include <QtCore/QFile>
#include <QtCore/QDir>

#include "QwUser.h"

#include <QtCore/QDateTime>
#include <QtGui/QTextDocument>
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>

#include <QtDebug>

QwcMessageStyle::QwcMessageStyle()
{
}


/*! Load the style from the given \a path (must be a *.QwiredMessageStyle directory).
    \return Returns true if successful.
*/
bool QwcMessageStyle::loadStyle(QString path)
{

    QDir styleDir(path);
    if (!styleDir.exists()) { return false; }

    m_stylePath = path;

    // Chat template
    QFile chatTemplateFile(styleDir.filePath("chat_template.html"));
    if (chatTemplateFile.exists()) {
        chatTemplateFile.open(QIODevice::ReadOnly);
        m_chatTemplate = chatTemplateFile.readAll();
    }

    return true;
}


/*! Returns the chat template HTML code.
*/
const QString& QwcMessageStyle::chatTemplate() const
{
    return m_chatTemplate;
}


/*! Returns the style base URL to be used when setting the HTML code of a QWebView.
    This is to allow images to be visible due to internal security restrictions in WebKit.
*/
QUrl QwcMessageStyle::styleBaseUrl() const
{
    return QUrl::fromLocalFile(m_stylePath + "/");
}


/*! Clears the chat contents element of QWebFrame \a frame.
*/
bool QwcMessageStyle::clearChat(QWebFrame *frame)
{
    if (!frame) { return false; }

    QWebElement targetElement = frame->findFirstElement("#chat_items");
    targetElement.setInnerXml("");
    return true;
}


bool QwcMessageStyle::addChatMessage(QWebFrame *frame, const QwUser &user, const QString &text,
                                     bool isEmote)
{
    if (!frame) { return false; }

    QWebElement targetElement = frame->findFirstElement("#chat_items");
    QWebElement templateElement;
    if (isEmote) {
        templateElement = frame->findFirstElement("#emote_item_template");
    } else {
        templateElement = frame->findFirstElement("#chat_item_template");
    }

    // Fill the new item with contents
    QWebElement chatItem = templateElement.clone();
    chatItem.removeAttribute("id");

    if (isEmote) {
        chatItem.findFirst(".emote_time").setInnerXml(QTime::currentTime()
                                                     .toString(Qt::SystemLocaleShortDate));
        chatItem.findFirst(".emote_date").setInnerXml(QDate::currentDate()
                                                     .toString(Qt::SystemLocaleShortDate));
        chatItem.findFirst(".emote_sender").setInnerXml(Qt::escape(user.nickname()));
        chatItem.findFirst(".emote_text").setInnerXml(text);
    } else {
        chatItem.findFirst(".chat_time").setInnerXml(QTime::currentTime()
                                                     .toString(Qt::SystemLocaleShortDate));
        chatItem.findFirst(".chat_date").setInnerXml(QDate::currentDate()
                                                     .toString(Qt::SystemLocaleShortDate));
        chatItem.findFirst(".chat_sender").setInnerXml(Qt::escape(user.nickname()));
        chatItem.findFirst(".chat_text").setInnerXml(text);
    }

    targetElement.appendInside(chatItem);
    return true;
}


bool QwcMessageStyle::addStatusMessage(QWebFrame *frame, const QString &text)
{
    if (!frame) { return false; }

    QWebElement templateElement = frame->findFirstElement("#chat_event_template");
    QWebElement targetElement = frame->findFirstElement("#chat_items");

    // Fill the new item with contents
    QWebElement chatItem = templateElement.clone();
    chatItem.removeAttribute("id");
    chatItem.findFirst(".chat_event_text").setInnerXml(Qt::escape(text));
    chatItem.findFirst(".chat_event_time").setInnerXml(QTime::currentTime()
                                                       .toString(Qt::SystemLocaleShortDate));
    chatItem.findFirst(".chat_event_date").setInnerXml(QDate::currentDate()
                                                       .toString(Qt::SystemLocaleShortDate));
    targetElement.appendInside(chatItem);
    return true;
}


bool QwcMessageStyle::addBroadcastMessage(QWebFrame *frame, const QwUser &user, const QString &text)
{
    if (!frame) { return false; }

    QWebElement templateElement = frame->findFirstElement("#chat_broadcast_template");
    QWebElement targetElement = frame->findFirstElement("#chat_items");

    // Fill the new item with contents
    QWebElement chatItem = templateElement.clone();
    chatItem.removeAttribute("id");
    chatItem.findFirst(".broadcast_sender").setInnerXml(Qt::escape(user.nickname()));
    chatItem.findFirst(".broadcast_text").setInnerXml(Qt::escape(text));
    chatItem.findFirst(".broadcast_time").setInnerXml(QTime::currentTime()
                                                           .toString(Qt::SystemLocaleShortDate));
    chatItem.findFirst(".broadcast_date").setInnerXml(QDate::currentDate()
                                                           .toString(Qt::SystemLocaleShortDate));
    targetElement.appendInside(chatItem);
    return true;
}


bool QwcMessageStyle::setTopicContents(QWebFrame *frame, const QString &text, const QString &user,
                                       QDateTime dateTime)
{
    if (!frame) { return false; }

    QWebElement topicTextElement = frame->findFirstElement("#qw_topic_text");
    topicTextElement.setInnerXml(text);

    QWebElement topicSetterElement = frame->findFirstElement("#qw_topic_setter");
    topicSetterElement.setInnerXml(Qt::escape(user));

    QWebElement topicDateElement = frame->findFirstElement("#qw_topic_date");
    topicDateElement.setInnerXml(Qt::escape(dateTime.toString()));

    return true;
}


/*! Set the CSS-contents of the \c style_override element in the frame. This allows for overriding
    CSS styling settings.
*/
bool QwcMessageStyle::setStyleOverride(QWebFrame *frame, const QString &css)
{
    if (!frame) { return false; }
    QWebElement overrideElement = frame->findFirstElement("#style_override");
    overrideElement.setPlainText(css);
    return true;
}


QString QwcMessageStyle::replaceEmoticons(const QString &text, bool enableEmoticons)
{
    Q_UNUSED(enableEmoticons);

    // URL
    QRegExp urlRegex("([a-z0-9]+://[a-zA-Z0-9._-]+(:[0-9]*)?([a-z0-9/\\?&=_\\.\\-%]*))",
                     Qt::CaseInsensitive);
    QString processText = text.trimmed();
    processText = Qt::escape(processText);
    processText.replace(urlRegex, "<a href=\"\\1\">\\1</a>");

//    if (enableEmoticons) {
//        QHash<QString,QString> pEmotes;
//        pEmotes[":):)"] = ":/icons/emotes/face-smile-big.png";
//        pEmotes[":D"] = ":/icons/emotes/face-grin.png";
//        pEmotes[":-D"] = ":/icons/emotes/face-grin.png";
//        pEmotes[":O"] = ":/icons/emotes/face-surprise.png";
//        pEmotes[";)"] = ":/icons/emotes/face-wink.png";
//        pEmotes[";-)"] = ":/icons/emotes/face-wink.png";
//        pEmotes[":)"] = ":/icons/emotes/face-smile.png";
//        pEmotes[":-)"] = ":/icons/emotes/face-smile.png";
//        pEmotes[":>"] = ":/icons/emotes/face-smile.png";
//        pEmotes[":("] = ":/icons/emotes/face-sad.png";
//        pEmotes[":-("] = ":/icons/emotes/face-sad.png";
//        pEmotes[";("] = ":/icons/emotes/face-crying.png";
//        pEmotes[";-("] = ":/icons/emotes/face-crying.png";
//        pEmotes[":*"] = ":/icons/emotes/face-kiss.png";
//        pEmotes[":-*"] = ":/icons/emotes/face-kiss.png";
//
//        QHashIterator<QString,QString> it(pEmotes);
//        while (it.hasNext()) {
//            it.next();
//            processText.replace(it.key(), QString("<img src=\"qrc%1\">").arg(it.value()));
//        }
//    }

    return processText;
}


/*! Returns a CSS snippet which defines the properties of \a font.
*/
QString QwcMessageStyle::cssFromFont(const QFont &font)
{
    return QString("font-family: \"%1\"; font-size: %2pt; font-weight: %3;")
            .arg(font.family())
            .arg(font.pointSizeF())
            .arg(font.weight());
}
