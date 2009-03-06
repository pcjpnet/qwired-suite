#include "QwcSession.h"
#include "QwcSingleton.h"

/*! \class QwcSingleton
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief The application-wide singelton to synchronize parts of the application at runtime.
*/

// Declare override-method for Mac dock
#ifdef Q_WS_MAC
	void qt_mac_set_dock_menu(QMenu *menu);
#endif

QwcSingleton::QwcSingleton()
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
		return "FreeMono";
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


/// Notify all connected objects about the changed prefs, so they can reload.
void QwcSingleton::notifyPrefsChanged()
{
	emit prefsChanged();
}


/// Add a session object to the list of sessions and connect the destroyed() signal
/// if it gets removed.
void QwcSingleton::addSession(QwcSession *session)
{
	pSessions.append(session);
	connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(sessionDestroyed(QObject*)));

	// Tray menu
	QMenu *tmpMenu = new QMenu(pTrayMenu);
	session->setTrayMenuAction(tmpMenu);
        pTrayMenu->addMenu(tmpMenu);
}


/// Called automatically when a session object is destroyed. Used to do some clean up work.
void QwcSingleton::sessionDestroyed(QObject *obj)
{
    QwcSession *session = static_cast<QwcSession*>(obj);
	if(pSessions.contains(session))
		pSessions.removeAll(session);

}


void QwcSingleton::createTrayIcon()
{
#ifdef Q_WS_MAC
	if(!pTrayIcon)
		qt_mac_set_dock_menu(pTrayMenu);
#else
	if(!pTrayIcon) {
		pTrayIcon = new QSystemTrayIcon(this);
		pTrayIcon->setIcon(QIcon(":/icons/qwired_logo_18.png"));
// 		pTrayIcon->setContextMenu(pTrayMenu);
		pTrayIcon->show();
		connect(pTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showTrayMenu(QSystemTrayIcon::ActivationReason)));
	}
#endif

}


void QwcSingleton::showTrayMenu(QSystemTrayIcon::ActivationReason reason)
{
	if(pTrayMenu && reason==QSystemTrayIcon::Trigger) {
		pTrayMenu->exec(QCursor::pos());
	}
}

void QwcSingleton::cleanUp()
{
	if(pTrayIcon)
		pTrayIcon->deleteLater();
// 	this->deleteLater();
}

void QwcSingleton::makeNewConnection(QString address)     // <-- Very basic functionality
{
    QwcSession *tmpConn = new QwcSession();
    addSession(tmpConn);
    tmpConn->onDoConnect(address, QString(""), QString(""));
    tmpConn->pConnectWindow->fContainer->setCurrentIndex(1);
    tmpConn->pConnectWindow->fStatus->setText(tr("Connecting..."));
}


