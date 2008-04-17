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
	sequence = 0;
	//QWTransaction::pSequence++;
// 	sequence = QWTransaction::pSequence;
	
}

QWTransaction::QWTransaction(const quint32 t) {
	rawLength = 0;
	type = t;
	error = 0;
	ttl = 16;
	flags = 0;
	sequence = getNextSequence();
// 	sequence = QWTransaction::pSequence;
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
	stream >> sequence;
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
	
// 	QDataStream headerStream(&buffer, QIODevice::WriteOnly);
	stream << (quint32)0;
	stream << type;
	stream << sequence;
	stream << error;
	stream << ttl;
	stream << flags;
// 	qDebug() << "Len after headers:"<<buffer.length();
	
	// add objects
	QHashIterator<QByteArray,QByteArray> i(objects);
	while(i.hasNext()) { i.next();
		stream << (quint8)i.key().length();
		stream.writeRawData( i.key().data(), i.key().length() );
		stream << i.value();
		
	}
	rawLength = buffer.length();
// 	qDebug() << "Len after data:"<<buffer.length();

// 	qDebug() << "Data before:"<<buffer.toHex();
	stream.device()->seek(0);
	stream << rawLength;
// 	qDebug() << "Len after seek:"<<buffer.length();
// 	qDebug() << "Data after:"<<buffer.toHex();


// 	qDebug() << "Total length:"<<rawLength;
	
// 	buffer = buffer.prepend(header);

// 	qDebug() << "buffer is now"<<buffer.length();
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

/**
 * Create a new transaction with proper flags set and return it.
 */
QWTransaction QWTransaction::toResponse() const {
	QWTransaction t;
	t.type = type;
	t.ttl = ttl;
	t.flags = Qwired::FlagResponse;
	t.sequence = sequence;
	return t;
}

int QWTransaction::getNextSequence() {
	static quint32 tmpSeq=0;
	return ++tmpSeq;
}

bool QWTransaction::hasObject(const QByteArray key) const {
	return objects.contains(key);
}

void QWTransaction::addObject(const QByteArray key, const QString data) {
	objects[key] = data.toUtf8();
}

void QWTransaction::addObject(const QByteArray key, const int data) {
	objects[key] = QByteArray::number(data);
}

int QWTransaction::getObjectInt(const QByteArray key) const {
	return getObject(key).toInt();
}




