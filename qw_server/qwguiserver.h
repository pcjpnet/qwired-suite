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
#ifndef QWGUISERVER_H
#define QWGUISERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPointer>

/**
	@author Bastian Bense <bb@bense.de>
*/
class QWGuiServer : public QObject
{
Q_OBJECT

private:
	QPointer<QTcpSocket> tcpSocket;
	QTcpServer *tcpServer;
	QByteArray tcpBuffer;

public:
    QWGuiServer(QObject *parent = 0);
    ~QWGuiServer();

private slots:
	void acceptConnection();
	void clearConnection();
	void readData();
	
public slots:
	void startServer();

};

#endif
