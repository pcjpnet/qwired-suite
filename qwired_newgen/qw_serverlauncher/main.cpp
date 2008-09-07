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


#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QSettings>
#include <QtDebug>
#include "widgets/servermonitor.h"

 int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(Resources);

	QApplication app(argc, argv);


	// Translation loader
	QSettings tmpPrefs;
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
	

	ServerMonitor *widget = new ServerMonitor();
	widget->show();

	return app.exec();
}