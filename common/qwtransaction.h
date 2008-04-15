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
#ifndef QWTRANSACTION_H
#define QWTRANSACTION_H

#include <QtCore>

namespace Qwired {
	enum Errors { ErrorOK=0, ErrorLoginFailed=100, ErrorBanned, ErrorPermissionDenied, ErrorObjectNotExists,
		ErrorObjectExists, ErrorObjectProtected, ErrorChecksumMismatch, ErrorAdminAborted };
}

/**
	@author Bastian Bense <bb@bense.de>
*/
class QWTransaction {
public:
    QWTransaction();
    ~QWTransaction();
	bool parseFromData(const QByteArray data);
	QByteArray toData();
	QHash<QByteArray,QByteArray> objects;
	quint32 rawLength;
	quint32 type;
	quint32 error;
	quint8  ttl;
	quint16 flags;
};



#endif
