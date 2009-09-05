#include "QwcSession.h"
#include "QwcSingleton.h"

#include <QtNetwork/QNetworkProxy>
#include <QtGui/QApplication>

/*! \class QwcSingleton
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief The application-wide singelton to synchronize parts of the application at runtime.
*/

// Declare override-method for Mac dock
#ifdef Q_WS_MAC
void qt_mac_set_dock_menu(QMenu *menu);
#endif

QwcSingleton::QwcSingleton(QObject *parent) : QObject(parent)
{
    pTrayMenu = new QMenu();
}

QwcSingleton::~QwcSingleton()
{
    pTrayMenu->deleteLater();
}


/// Return the default monospace font for the current operating system.
QString QwcSingleton::systemMonospaceFont()
{
#ifdef Q_WS_X11
    return "Monospace";
#endif
#ifdef Q_WS_WIN
    return "Andale Mono";
#endif
#ifdef Q_WS_MAC
    return "Monaco";
#endif
};


/// Return a color key from the preferences.
/// @param theKey The name of the key.
/// @param theDefault The default color, if nothing was found in the preferences.
QColor QwcSingleton::colorFromPrefs(QString theKey, QColor theDefault)
{
    QSettings conf;
    if(conf.contains(theKey)) {
        return conf.value(theKey).value<QColor>();
    } else {
        return theDefault;
    }
}


/*! Check the bookmarks for auto-connect enabled servers and create a session for each. Otherwise
    create a new, blank session and leave it unconnected.
*/
void QwcSingleton::createInitialSessions()
{
    QSettings settings;
    int bookmarkCount = settings.beginReadArray("bookmarks");
    int autoconnectedBookmarks = 0;

    // Run through all bookmarks and check if they have the autoconnect flag set.
    for (int i = 0; i < bookmarkCount; i++) {
        settings.setArrayIndex(i);
        if (settings.value("autoconnect").toBool()) {
            QwcSession *newSession = new QwcSession();
            newSession->connectWidget->fAddress->insert(settings.value("address").toString());
            newSession->connectWidget->fLogin->insert(settings.value("login").toString());
            newSession->connectWidget->fPassword->insert(settings.value("password").toString());
            newSession->connectWidget->btnConnect->click();
            addSession(newSession);
            autoconnectedBookmarks++;
        }
    }

    // No bookmarks exist. Create a new blank session.
    if (autoconnectedBookmarks == 0) {
        QwcSession *blankSession = new QwcSession();
        addSession(blankSession);
    }

    settings.endArray();
}


/*! Prepare the application and emit the applicationSettingsChanged() signal, so that other parts
    of the application can re-load their settings, in case they just changed.
*/
void QwcSingleton::emitApplicationSettingsChanged()
{
    // Configure the proxying, if needed
    QSettings settings;
    if (settings.value("proxy/type").toInt() != 0) {
        QNetworkProxy proxy((QNetworkProxy::ProxyType)settings.value("proxy/type").toInt());
        proxy.setHostName(settings.value("proxy/host").toString());
        proxy.setPort(settings.value("proxy/port").toInt());
        proxy.setUser(settings.value("proxy/username").toString());
        proxy.setPassword(settings.value("proxy/password").toString());
        QNetworkProxy::setApplicationProxy(proxy);
    }

    emit applicationSettingsChanged();
}


/*! Add a session object to the list of sessions and connect the destroyed() signal if it gets
    removed.
*/
void QwcSingleton::addSession(QwcSession *session)
{
    sessions.append(session);
    connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(sessionDestroyed(QObject*)));

    // Tray menu
    QMenu *tmpMenu = new QMenu(pTrayMenu);
    session->setTrayMenuAction(tmpMenu);
    pTrayMenu->addMenu(tmpMenu);
}


/*! Called automatically when a session object is destroyed. Used to do some clean up work.
*/
void QwcSingleton::sessionDestroyed(QObject *obj)
{
    QwcSession *session = static_cast<QwcSession*>(obj);
    if (!session) { return; }
    if (!sessions.contains(session)) { return; }
    sessions.removeAll(session);
}


/*! Create the system tray icon for Qwired.
*/
void QwcSingleton::createTrayIcon()
{
#ifdef Q_WS_MAC
    if(!pTrayIcon)
        qt_mac_set_dock_menu(pTrayMenu);
#else
    if (!pTrayIcon) {
        pTrayIcon = new QSystemTrayIcon(this);
        pTrayIcon->setIcon(QIcon(":/icons/qwired_logo_18.png"));
        pTrayIcon->show();
        connect(pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(showTrayMenu(QSystemTrayIcon::ActivationReason)));
    }
#endif

}





void QwcSingleton::showTrayMenu(QSystemTrayIcon::ActivationReason reason)
{
    if (pTrayMenu && reason == QSystemTrayIcon::Trigger) {
        pTrayMenu->exec(QCursor::pos());
    }
}

void QwcSingleton::cleanUp()
{
    if(pTrayIcon)
        pTrayIcon->deleteLater();
    // 	this->deleteLater();
}



