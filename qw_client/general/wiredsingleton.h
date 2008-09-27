/***************************************************************************
 * QWired - a free Wired client.
 * Copyright (C) 2008 Bastian Bense
 ***************************************************************************
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or           *
 * (at your option) any later version.                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU        *
 * General Public License for more details.                                *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the                                 *
 * Free Software Foundation, Inc.,                                         *
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA                   *
 ***************************************************************************/

 
#ifndef WIREDSINGLETON_H
#define WIREDSINGLETON_H

#include <QObject>
#include <QPointer>
#include <QColor>
#include <QSettings>
#include <QMenu>
#include <QSystemTrayIcon>

class ClassWiredSession;

/**
A basic singleton to handle global notification within the app.
	@author Bastian Bense <bastibense@gmail.com>
 */
class WiredSingleton
	: public QObject
{
	
Q_OBJECT

public:
    WiredSingleton();
    ~WiredSingleton();
	void addSession(ClassWiredSession *session);
	void createTrayIcon();
	QList<ClassWiredSession*> pSessions;
	QPointer<QMenu> pTrayMenu;
	QPointer<QSystemTrayIcon> pTrayIcon;

	// Global functions
	static QString systemMonospaceFont();
	static QColor colorFromPrefs(QString theKey, QColor theDefault);

private slots:
	void sessionDestroyed(QObject *obj);
	void showTrayMenu(QSystemTrayIcon::ActivationReason);
	
signals:
	void prefsChanged();

public slots:
	void notifyPrefsChanged();
	void cleanUp();
	
};

#endif
