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

 
#ifndef QWSFILE_H
#define QWSFILE_H

#include <QtCore>

/*
#include <QFileInfo>
#include <QList>
#include <QByteArray>
#include <QDateTime>
#include <QString>
*/

namespace Qws {
    /*! A file item within the protocol can have different types. */
    enum FileType { FileTypeRegular, FileTypeFolder, FileTypeUploadsFolder, FileTypeDropBox };
}

class QwsFile
{

public:
    QwsFile(QList<QByteArray> theParams);
    QwsFile();
    ~QwsFile();
    
    bool isWithinLocalRoot();
    bool updateLocalPath(bool quickCheck=false);
    void updateLocalChecksum();

    /*! The relative location of the file - usually this is appended to the \a localFilesRoot path.
        Example: "/photos/myphoto.jpg" */
    QString path;
    qlonglong size;
    QDateTime created;
    QDateTime modified;
    QString checksum;
    QString comment;

    /*! The local root of the files tree. (normally the Files directory of the server)
        Example: "/home/username/qwired_server/files" */
    QString localFilesRoot;
    QString localAbsolutePath;

    /*! The type of the file. See \a Qws::FileType for more information. */
    Qws::FileType type;

    QString fileName() const;


    ////////////////////////////////
    // Transfer Related Properties
    ////////////////////////////////

    /*! This is used during \a TRANSFER messages. */
    QString transferHash;

    static QString humanReadableSize(float theBytes);
    void setFromStat(QList<QByteArray> theParams);

};

Q_DECLARE_METATYPE(QwsFile)

#endif
