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
ClassWiredFile::ClassWiredFile(QList<QByteArray> theParams) {
	path = QString::fromUtf8( theParams.value(0) );
	type = theParams.value(1).toInt();
	size = theParams.value(2).toInt();
	created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
	//qDebug() << "[files] created ClassWiredFile for"<<path<<"type="<<type<<"size="<<size;
}


ClassWiredFile::~ClassWiredFile()
{ }


ClassWiredFile::ClassWiredFile() {
	path="";
	type=0;
	size=0;
}

// Return the name of the file without all the slashes.
QString ClassWiredFile::fileName() const {
	return path.section("/", -1);
}

void ClassWiredFile::setFromStat(QList<QByteArray> theParams) {
	path = QString::fromUtf8( theParams.value(0) );
	type = theParams.value(1).toInt();
	size = theParams.value(2).toInt();
	created = QDateTime::fromString( theParams.value(3), Qt::ISODate );
	modified = QDateTime::fromString( theParams.value(4), Qt::ISODate );
	checksum = QString::fromUtf8( theParams.value(5) );
	comment = QString::fromUtf8( theParams.value(6) );
}



/**
 * Return a human readable representation of the size of a file.
 * @param theBytes The raw amount of octets.
 * @return A human readable string representing the number of bytes.
 */
QString ClassWiredFile::humanReadableSize(qlonglong theBytes) {
	qlonglong a=1024;
	float b=1024;
	
	if(theBytes<0) {
		return QString("-");
	} else if(theBytes < a) {
		return QString("%1").arg(theBytes);
	} else if(theBytes < a*a) {
		return QString("%1 KB").arg(theBytes/b);
	} else if(theBytes < a*a*a) {
		return QString("%1 MB").arg(float(theBytes/b/b), 0, 'f', 2);
	} else if(theBytes < a*a*a*a) {
		return QString("%1 GB").arg(float(theBytes/b/b/b), 0, 'f', 2);
	} else if(theBytes < a*a*a*a*a) {
		return QString("%1 TB").arg(float(theBytes/b/b/b/b), 0, 'f', 2);
	}
	return "?";
}


/**
 * Return a small icon for this file/type.
 */
QIcon ClassWiredFile::fileIcon() const {
	QHash<QString,QString> tmpTypes;
	tmpTypes["jpg"] = ":/icons/files/files-image.png";
	tmpTypes["jpeg"] = ":/icons/files/files-image.png";
	tmpTypes["gif"] = ":/icons/files/files-image.png";
	tmpTypes["psd"] = ":/icons/files/files-image.png";
	tmpTypes["svg"] = ":/icons/files/files-image.png";
	tmpTypes["pdf"] = ":/icons/files/files-image.png";
	tmpTypes["tif"] = ":/icons/files/files-image.png";
	tmpTypes["tiff"] = ":/icons/files/files-image.png";
	
	tmpTypes["zip"] = ":/icons/files/files-archive.png";
	tmpTypes["tar"] = ":/icons/files/files-archive.png";
	tmpTypes["gz"] = ":/icons/files/files-archive.png";
	tmpTypes["sit"] = ":/icons/files/files-archive.png";
	tmpTypes["dmg"] = ":/icons/files/files-archive.png";
	tmpTypes["7z"] = ":/icons/files/files-archive.png";
	tmpTypes["rar"] = ":/icons/files/files-archive.png";

	tmpTypes["doc"] = ":/icons/files/files-office.png";
	tmpTypes["xls"] = ":/icons/files/files-office.png";
	tmpTypes["odt"] = ":/icons/files/files-office.png";
	tmpTypes["ppt"] = ":/icons/files/files-office.png";

	tmpTypes["h"] = ":/icons/files/files-script.png";
	tmpTypes["cpp"] = ":/icons/files/files-script.png";
	tmpTypes["c"] = ":/icons/files/files-script.png";
	
	tmpTypes["txt"] = ":/icons/files/files-text.png";
	tmpTypes["rdf"] = ":/icons/files/files-text.png";
	tmpTypes["log"] = ":/icons/files/files-text.png";

	tmpTypes["avi"] = ":/icons/files/files-media.png";
	tmpTypes["mpg"] = ":/icons/files/files-media.png";
	tmpTypes["mpeg"] = ":/icons/files/files-media.png";
	tmpTypes["mov"] = ":/icons/files/files-media.png";
	tmpTypes["ogg"] = ":/icons/files/files-media.png";
	tmpTypes["ts"] = ":/icons/files/files-media.png";

	tmpTypes["mp3"] = ":/icons/files/files-audio.png";
	tmpTypes["m4u"] = ":/icons/files/files-audio.png";
	tmpTypes["m4a"] = ":/icons/files/files-audio.png";
	tmpTypes["m4r"] = ":/icons/files/files-audio.png";
	tmpTypes["m4r"] = ":/icons/files/files-audio.png";

	tmpTypes["app"] = ":/icons/files/files-executable.png";
	tmpTypes["exe"] = ":/icons/files/files-executable.png";
	tmpTypes["bin"] = ":/icons/files/files-executable.png";
	tmpTypes["run"] = ":/icons/files/files-executable.png";
	tmpTypes["jar"] = ":/icons/files/files-executable.png";
	
	QString tmpSuffix = this->fileName().section(".",-1,-1);
	switch(this->type) {
		case Wired::Directory:
			return QIcon(":/icons/files/files-folder.png"); break;
			
		case Wired::DropBox:
			return QIcon(":/icons/files/files-dropbox.png"); break;
			
		case Wired::Uploads:
			return QIcon(":/icons/files/files-uploads.png"); break;

		case Wired::RegularFile:
		default:
			if(tmpTypes.contains(tmpSuffix)) {
				return QIcon(tmpTypes[tmpSuffix]);
			} else {
				return QIcon(":/icons/files/files-regular.png");
			}
			break;
	}
	
	return QIcon();
}


