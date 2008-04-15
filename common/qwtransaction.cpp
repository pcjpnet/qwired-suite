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

#include "qwtransaction.h"

QWTransaction::QWTransaction() {
	rawLength = 0;
	type = 0;
	error = 0;
	ttl = 16;
	flags = 0;
}

QWTransaction::~QWTransaction() {
}

/**
 * Parse a transaction from the serialized protocol stream.
 * @param data The data astream containing the flat transaction.
 * @return True if successful.
 */
bool QWTransaction::parseFromData(const QByteArray data) {
	QDataStream stream(data);
	
	stream >> rawLength;
	stream >> type;
	stream >> error;
	stream >> ttl;
	stream >> flags;
	
	// Check length
	if(rawLength != data.length()) {
		qDebug() << "QWTransaction: Ignoring transaction with wrong size.";
		return false;
	}

	// Parse objects
	while(1) {
		QByteArray key_name;
		QByteArray key_value;
		quint8 keyLen;

		stream >> keyLen;
		key_name.resize(keyLen);
		stream.readRawData(key_name.data(), keyLen);
		stream >> key_value;
		objects[key_name] = key_value;
		//qDebug() << "QWTransaction: Found: "<<key_name<<"data:"<<key_value.length()<<"value:"<<key_value;
		if(stream.atEnd())
			return true;
	}

	return true;
	
}

/**
 * Serialize the transaction object for transmission.
 * @return The serialized transaction object.
 */
QByteArray QWTransaction::toData() {
	QByteArray buffer;
	
	QDataStream stream(&buffer, QIODevice::WriteOnly);
	QHashIterator<QByteArray,QByteArray> i(objects);
	while(i.hasNext()) { i.next();
		stream << (quint8)i.key().length();
		stream.writeRawData( i.key().data(), i.key().length());
		stream << i.value();
		
	}
	rawLength = buffer.length()+15;
	
	QByteArray header;
	QDataStream headerStream(&header, QIODevice::WriteOnly);
	headerStream << rawLength;
	headerStream << type;
	headerStream << error;
	headerStream << ttl;
	headerStream << flags;

	qDebug() << "Total length:"<<rawLength;
	
	buffer.prepend(header);
	return buffer;
	
}

/**
 * Add an object to the transaction.
 * @param key The ASCII formatted key.
 * @param data The raw data.
 */
void QWTransaction::addObject(const QByteArray key, const QByteArray data) {
	objects[key] = data;
	//qDebug() << "QWTransaction: Adding"<<key;
}

/**
 * Find an object in the transaction.
 * @param key The key of the object.
 * @return The raw data of the object.
 */
QByteArray QWTransaction::getObject(const QByteArray key) const {
	if(objects.contains(key)) {
		return objects[key];
	} else {
		qDebug() << "QWTransaction: Warning: Unknown key: "<<key;
	}
}

/**
 * Find an object in the transaction and convert it to utf-8.
 * @param key The key of the object
 * @return The UTF-8 version of the data.
 */
QString QWTransaction::getObjectString(const QByteArray key) const {
	return QString::fromUtf8( getObject(key) );
}



