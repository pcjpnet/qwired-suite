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
	enum Errors { ErrorOK=0, ErrorLoginFailed=100, ErrorPermissionDenied, ErrorObjectNotExists,
		ErrorObjectExists, ErrorObjectProtected, ErrorChecksumMismatch, ErrorAdminAborted };

	enum Flags { FlagResponse=1, FlagListingComplete=2 };

	enum Transactions {
		T_HandshakeRequest=1000, T_LoginRequest, T_SetUserInfo, T_PingRequest=1010, T_ServerBannerRequest, T_MotdRequest,
  		T_UserListRequest=1020, T_UserInfoRequest
	};
}

/**
	@author Bastian Bense <bb@bense.de>
*/
class QWTransaction {
private:
	static int getNextSequence();

public:
	
    QWTransaction();
	QWTransaction(const quint32 t);
    ~QWTransaction();
	bool parseFromData(const QByteArray data);
	
	bool hasObject(const QByteArray key) const;
	
	void addObject(const QByteArray key, const QByteArray data);
	void addObject(const QByteArray key, const QString data);
	void addObject(const QByteArray key, const int data);

	void setFlagListingComplete();
	bool hasFlagListingComplete();
	
	QByteArray getObject(const QByteArray key) const;
	QString getObjectString(const QByteArray key) const;
	int getObjectInt(const QByteArray key) const;
	
	QWTransaction toResponse() const;

	QByteArray toData() const;
	QHash<QByteArray,QByteArray> objects;
	quint32 rawLength;
	quint16 type;
	quint32 sequence;
	quint8 error;
	quint8 ttl;
	quint16 flags;

	static quint32 pSequence;

	
};



#endif
