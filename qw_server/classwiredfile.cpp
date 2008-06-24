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
#include "classwiredfile.h"


// Load data from the server file listing responses.
ClassWiredFile::ClassWiredFile(QList<QByteArray> theParams)
{
	path = QString::fromUtf8( theParams.value(0) );
	type = theParams.value(1).toInt();
	size = theParams.value(2).toInt();
	created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
}


ClassWiredFile::~ClassWiredFile()
{
}


ClassWiredFile::ClassWiredFile()
{
	path="";
	type=0;
	size=0;
}

// Return the name of the file without all the slashes.
QString ClassWiredFile::fileName() const
{
	return path.section("/", -1);
}

void ClassWiredFile::setFromStat(QList<QByteArray> theParams)
{
	path = QString::fromUtf8( theParams.value(0) );
	type = theParams.value(1).toInt();
	size = theParams.value(2).toInt();
	created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
	checksum = QString::fromUtf8( theParams.value(5) );
	comment = QString::fromUtf8( theParams.value(6) );
}
