#include "QwcGlobals.h"
#include "QwcUserlistDelegate.h"
#include "QwcPrivateMessager.h"
#include "QwcUserInfo.h"

#include <QApplication>
#include <QPainter>

QwcUserlistDelegate::QwcUserlistDelegate(QObject *parent) : QItemDelegate (parent)
{
    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(reloadPreferences()) );
    reloadPreferences();
}


QwcUserlistDelegate::~QwcUserlistDelegate()
{
}


void QwcUserlistDelegate::reloadPreferences() {
    QSettings s;
    pListFont.fromString(s.value("interface/userlist/font", QApplication::font().toString()).toString());
    pCompactMode = s.value("interface/userlist/compact",false).toBool();
    pAlternateRowBg = s.value("interface/userlist/alternate_bg",false).toBool();
}


QSize QwcUserlistDelegate::sizeHint ( const QStyleOptionViewItem &, const QModelIndex &index ) const {
    if(!index.isValid()) return QSize();
    if(pCompactMode) {
        return QSize(160,24);
    } else {
        return QSize(160,34);
    }
}


void QwcUserlistDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /// Display mode: 0 = normal chat user icons, 1 = private messenger user icons with unread count
    int displayMode = 0;
    QwcPrivateMessagerSession session;

    painter->save();
    painter->translate(option.rect.topLeft());
    QRect itemRect(QPoint(0,0), option.rect.size());

    if (index.data(Qt::UserRole).canConvert<QwcUserInfo>()) {
        displayMode = 0;
        session.userInfo = index.data(Qt::UserRole).value<QwcUserInfo>();
    } else if (index.data(Qt::UserRole).canConvert<QwcPrivateMessagerSession>()) {
        displayMode = 1;
        session = index.data(Qt::UserRole).value<QwcPrivateMessagerSession>();
    } else {
        // Something went wrong!
        return;
    }



    painter->setRenderHint(QPainter::Antialiasing);

    QSize iconSize(itemRect.height()-4, itemRect.height()-4);
    QPixmap userIcon = session.userInfo.iconAsPixmap().scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Nice Background
    painter->save();
    painter->setClipRect(itemRect);
    painter->translate(0, itemRect.height()/2);
    painter->scale(8, 8);
    painter->translate(0, -iconSize.height()*0.5);
    painter->setOpacity(0.2);
    painter->drawPixmap(0, 0, userIcon);
    painter->restore();

    // Draw the icon
    painter->drawPixmap(2, 2, userIcon);

    // Draw the user nickname and status
    QFont font = painter->font();

    // Nickname & Status
    painter->save();
    painter->translate(iconSize.width()+8, itemRect.height()/2);


    // Status
    if (!session.userInfo.pStatus.isEmpty()) {
        font.setBold(false);
        font.setPixelSize(10);
        painter->setFont(font);
        painter->drawText(0, painter->fontMetrics().ascent()+1, session.userInfo.pStatus);
        painter->restore();
    }

    // Nickname
    font.setBold(true);
    font.setPixelSize(12);
    painter->setFont(font);

    // Move the nickname to the middle if there is no status
    if (session.userInfo.pStatus.isEmpty()) {
        painter->translate(0, painter->fontMetrics().ascent()/2);
    }

    painter->drawText(0, -2, session.userInfo.pNick);


    // Unread Count (only for message sessions)
    if (session.unreadCount) {
        painter->save();
        QRect badgeRect(0, 0, iconSize.width(), iconSize.height()*0.5);
        font.setPixelSize(badgeRect.height()*0.8);
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
        painter->setOpacity(0.3);
        painter->setBrush(option.palette.color(QPalette::Highlight));
        painter->drawRect(itemRect);
        painter->restore();
    }

    painter->restore();

    /*
if (!index.data(Qt::UserRole).canConvert<QwcUserInfo>()) { return; }
    QwcUserInfo userInfo = index.data(Qt::UserRole).value<QwcUserInfo>();

    //ClassWiredUser usr = index.data(Qt::UserRole).value<ClassWiredUser>();
//    QString status = index.data ( Qt::UserRole+4 ).toString();
//    QString nick = index.data ( Qt::DisplayRole ).toString();
//    bool admin = index.data ( Qt::UserRole+1 ).toBool();
//    bool idle = index.data ( Qt::UserRole+2 ).toBool();

    painter->save();
    painter->translate(0, option.rect.y());
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Set nick color
    QColor nickColor=Qt::black;
    QColor statusColor=Qt::black;
    if (userInfo.pAdmin) nickColor=Qt::red;

    // Alternate Bg
//    if(pAlternateRowBg && (index.row() % 2)==1) {
//        painter->fillRect(0,0,option.rect.width(), option.rect.height(), QColor(238,238,255) );
//    }

    // Background
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(0, 0, option.rect.width(), option.rect.height(), Qt::gray);
    }

    if (userInfo.pIdle) {
        painter->setOpacity(0.5);
    }

    // User Icon/Picture
    painter->save();

    QPixmap icon = userInfo.iconAsPixmap();

    // Fallback icon
    if (icon.isNull()) {
        icon = QPixmap(":/icons/icon_missing.png");
    }

    // Resize
    if(pCompactMode) {
        icon = icon.scaled(22, 22, Qt::KeepAspectRatio);
    }

    painter->drawPixmap(1, 1, icon);
    painter->restore();

    if(pCompactMode || userInfo.pStatus.isEmpty()) {
        // Draw nick only
        QFont f = pListFont;
        painter->save();
        QFontMetrics fm(f);
        painter->setFont(f);
        painter->setPen(nickColor);
        if(pCompactMode) {
            painter->drawText(28, (option.rect.height()/2)+fm.descent(), userInfo.pNick);
        } else {
            painter->drawText(40, (option.rect.height()/2)+fm.descent(), userInfo.pNick);
        }
        painter->restore();

    } else {
        // Draw nick + status
        QFont f = pListFont;

        // Status
        painter->save();
        f.setPointSizeF( f.pointSize()*0.85 );
        QFontMetrics fm ( f );
        painter->setFont ( f );
        painter->setPen ( statusColor );
        painter->drawText ( 40, ( option.rect.height() /2 ) +fm.ascent() +1, userInfo.pStatus );
        painter->restore();

        // Nick
        painter->save();
        f = pListFont;
        painter->setFont ( f );
        painter->setPen ( nickColor );
        painter->drawText ( 40, ( option.rect.height() /2 )-1, userInfo.pNick );
        painter->restore();

    }
    painter->restore();
    */
}



