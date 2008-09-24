/***************************************************************************
 *   Copyright (C) 2007 by Bastian Bense   *
 *   bb@bense.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "classwiredsession.h"
#include "wiredsingleton.h"

// Declare override-method for Mac dock
#ifdef Q_WS_MAC
	void qt_mac_set_dock_menu(QMenu *menu);
#endif

WiredSingleton::WiredSingleton()
{
	pTrayMenu = new QMenu();
}

WiredSingleton::~WiredSingleton()
{
	pTrayMenu->deleteLater();
}


/// Return the default monospace font for the current operating system.
QString WiredSingleton::systemMonospaceFont()
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
QColor WiredSingleton::colorFromPrefs(QString theKey, QColor theDefault)
{
	QSettings conf;
	if(conf.contains(theKey)) {
		return conf.value(theKey).value<QColor>();
	} else {
		return theDefault;
	}
}


/// Notify all connected objects about the changed prefs, so they can reload.
void WiredSingleton::notifyPrefsChanged()
{
	emit prefsChanged();
}


/// Add a session object to the list of sessions and connect the destroyed() signal
/// if it gets removed.
void WiredSingleton::addSession(ClassWiredSession *session)
{
	pSessions.append(session);
	connect(session, SIGNAL(destroyed(QObject*)), this, SLOT(sessionDestroyed(QObject*)));

	// Tray menu
	QMenu *tmpMenu = new QMenu(pTrayMenu);
	session->setTrayMenuAction(tmpMenu);
	pTrayMenu->addMenu(tmpMenu);
}


/// Called automatically when a session object is destroyed. Used to do some clean up work.
void WiredSingleton::sessionDestroyed(QObject *obj)
{
	ClassWiredSession *session = static_cast<ClassWiredSession*>(obj);
	if(pSessions.contains(session))
		pSessions.removeAll(session);

}


void WiredSingleton::createTrayIcon()
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


void WiredSingleton::showTrayMenu(QSystemTrayIcon::ActivationReason reason)
{
	if(pTrayMenu && reason==QSystemTrayIcon::Trigger) {
		pTrayMenu->exec(QCursor::pos());
	}
}




