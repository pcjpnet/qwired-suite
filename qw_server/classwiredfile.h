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

 
#ifndef CLASSWIREDFILE_H
#define CLASSWIREDFILE_H

#include <QtCore>

namespace Wired {
	enum FileType { RegularFile, Directory, Uploads, DropBox };
}

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class ClassWiredFile {
public:
    ClassWiredFile(QList<QByteArray> theParams);
    ClassWiredFile();
    ~ClassWiredFile();
    
	static QString humanReadableSize(float theBytes);
    void setFromStat(QList<QByteArray> theParams);
	
    
    // Default parameters
    QString path;
    int type;
    qlonglong size;
    QDateTime created;
    QDateTime modified;
    
    // STAT parameters
    QString checksum;
    QString comment;
    
    QString fileName() const;
	QIcon fileIcon() const;

};

Q_DECLARE_METATYPE(ClassWiredFile)

#endif
