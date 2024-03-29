#include "QwcGlobals.h"
#include "QwcUserlistDelegate.h"
#include "QwcPrivateMessager.h"
#include "QwcUserInfo.h"
#include "QwcSocket.h"

#include <QtGui/QPainter>

QwcUserlistDelegate::QwcUserlistDelegate(QObject *parent) :
        QItemDelegate (parent)
{
    // Notification manager
    QwcSingleton *singleton = &WSINGLETON::Instance();
    connect(singleton, SIGNAL(applicationSettingsChanged()),
            this, SLOT(reloadPreferences()));
    reloadPreferences();

    m_socket = NULL;
}


QwcUserlistDelegate::~QwcUserlistDelegate()
{
}

void QwcUserlistDelegate::setSocket(QwcSocket *socket)
{
    m_socket = socket;
}


void QwcUserlistDelegate::reloadPreferences()
{
    QSettings settings;
    m_listFont.fromString(settings.value("interface/userlist/font",
                                         QApplication::font().toString()).toString());
    m_compactMode = settings.value("interface/userlist/compact",false).toBool();
    m_opacity = settings.value("interface/userlist/background_opacity", 30).toInt()/100.0;
}


QSize QwcUserlistDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (!index.isValid()) { return QSize(); }
    return QSize(160, m_compactMode ? 24 : 34);
}


void QwcUserlistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    if (!index.isValid()) { return; }
    const QwcUserInfo &user = m_socket->users()[index.data(Qt::UserRole).toInt()];

    /// Display mode: 0 = normal chat user icons, 1 = private messenger user icons with unread count
    int displayMode = 0;
    QwcPrivateMessagerSession session;

    painter->save();
    painter->translate(option.rect.topLeft());

    QRect itemRect(QPoint(0,0), option.rect.size());

    if (index.data(Qt::UserRole).canConvert<int>()) {
        // Chat mode
        displayMode = 0;
        session.userInfo = user;
    } else if (index.data(Qt::UserRole).canConvert<QwcPrivateMessagerSession>()) {
        // Messenger mode
        displayMode = 1;
        session = index.data(Qt::UserRole).value<QwcPrivateMessagerSession>();
    } else {
        // Something went wrong!
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);

    QSize iconSize(itemRect.height()-4, itemRect.height()-4);
    QImage userIconFullres = session.userInfo.userImage();
    QImage userIcon = userIconFullres.scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Nice Background
    if (m_opacity > 0.0) {
        painter->save();
        QImage userIconBackground;
        if (userIconFullres.height() == option.rect.height()) {
            userIconBackground = userIconFullres;
        } else {
            userIconBackground = userIconFullres.scaledToWidth(350, Qt::SmoothTransformation);
        }
        painter->setClipRect(itemRect);
        painter->setOpacity(m_opacity);
        painter->drawImage(0, option.rect.height()/2-userIconBackground.height()/2, userIconBackground);
        painter->restore();

    }

    // Draw the icon
    bool isConformingIcon = userIconFullres.height() == 34 && userIconFullres.width() >= 34;
    if (m_opacity == 0 || !isConformingIcon) {
        painter->drawImage(2, option.rect.height()/2-userIcon.height()/2, userIcon);
    }

    // Draw the user nickname and status
    QFont font = painter->font();

    // Nickname & Status
    painter->save();
    painter->translate(iconSize.width()+8, itemRect.height()/2);


    // Status
    bool statusTextAvailable = !session.userInfo.status().isEmpty();
    if (!m_compactMode && statusTextAvailable) {
        font.setBold(false);
        font.setPixelSize(10);
        painter->setFont(font);
        painter->drawText(0, painter->fontMetrics().ascent()+1, session.userInfo.status());

    }

    // Nickname
    font.setBold(true);
    font.setPixelSize(12);
    painter->setFont(font);

    // Move the nickname to the middle if there is no status
    if (m_compactMode || !statusTextAvailable) {
        painter->translate(0, painter->fontMetrics().ascent()/2);
    }

    // Paint the nickname
    painter->save();
    QColor statusColor(Qt::black);
    if (session.userInfo.isAdmin()) {
        statusColor = Qt::red;
    }
    painter->setPen(statusColor);
    painter->drawText(0, -2, session.userInfo.nickname());
    painter->restore();

    painter->restore();


    // Unread Count (only for message sessions)
    if (session.unreadCount) {
        painter->save();
        QRect badgeRect(0, 0, int(iconSize.width()), int(iconSize.height()*0.5));
        font.setPixelSize(int(badgeRect.height()*0.8));
        font.setBold(true);
        painter->translate(itemRect.width()-iconSize.width()-8, itemRect.height()/2 - badgeRect.height()/2);
        painter->setFont(font);
        painter->setPen(QColor(Qt::red).darker(130));
        painter->setBrush(Qt::red);
        painter->drawRoundedRect(badgeRect, 3, 3);
        painter->setOpacity(1);
        painter->setPen(QPen(Qt::white));
        painter->drawText(badgeRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(session.unreadCount));
        painter->restore();
    }

    // Option highlight
    if (option.state & QStyle::State_Selected) {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setOpacity(0.5);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(itemRect);
        painter->restore();
    } else {
        // Idle dimming
        if (session.userInfo.isIdle()) {
            painter->save();
            painter->setBrush(Qt::white);
            painter->setOpacity(0.5);
            painter->setPen(Qt::NoPen);
            painter->drawRect(itemRect);
            painter->restore();
        }
    }



    painter->restore();
}



