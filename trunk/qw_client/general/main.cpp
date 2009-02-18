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


#include <QtCore>
#include <QtGui>
#include "classwiredsession.h"
#include "wiredcommon.h"

int main (int argc, char *argv[]) {
	Q_INIT_RESOURCE(application);
	QApplication app(argc, argv);
	
	// Set the plugins directory correctly.
	// This has to be done for bundles in OS X if they are packed using the deployqt tool.
#ifdef Q_WS_MAC
	qDebug() << "Running in mac mode.";
	QDir dir(QApplication::applicationDirPath());
 	dir.cdUp();
 	dir.cd("Plugins");
 	QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif

/*#ifdef Q_WS_X11
	QApplication::setStyle(new QCleanlooksStyle);
#endif*/
	
	QCoreApplication::setOrganizationName("NeoSoftware");
	QCoreApplication::setOrganizationDomain("neo.de");
	QCoreApplication::setApplicationName("Qwired");

	QSettings tmpPrefs;

	// Translation loader
	QTranslator translator;
	QString tmpLocaleShort = QLocale::system().name().section ( "_",0,0 );
	QStringList tmpLocaleList;
	if (tmpPrefs.contains("general/language") && tmpPrefs.value("general/language").toString()!="_auto_" ) {
		tmpLocaleList.append(QString(":lang_%1.qm").arg(tmpPrefs.value("general/language").toString()));
	} else {
		tmpLocaleList.append(QLocale::system().name()); // long locale, from file
		tmpLocaleList.append(tmpLocaleShort); // short locale, from file
		tmpLocaleList.append(QString(":%1.qm").arg(QLocale::system().name())); // long locale, from res
		tmpLocaleList.append(QString(":lang_%1.qm").arg(tmpLocaleShort) );
	}
	QStringListIterator i(tmpLocaleList);
	while (i.hasNext()) {
		QString tmpName = i.next();
		if(translator.load(tmpName)) {
			app.installTranslator(&translator);
			break;
		} else {
			qDebug() << "Trying to load locale:"<<tmpName<<": Failed (trying next)";
		}
	}

	// Create the inital connection
	ClassWiredSession *tmpConn = new ClassWiredSession();

	// Singleton
	QObject::connect(&app, SIGNAL(aboutToQuit()), &WSINGLETON::Instance(), SLOT(cleanUp()));
	WSINGLETON::Instance().addSession(tmpConn);
        WSINGLETON::Instance().createTrayIcon();

	return app.exec();
}

