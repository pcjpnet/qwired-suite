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

 
#ifndef WIDGETSERVERINFORMATION_H
#define WIDGETSERVERINFORMATION_H

#include <QWidget>
#include <QtNetwork>
#include <QSslSocket>
#include <QSslCipher>
#include "ui_QwcServerInfoWidget.h"
#include "QwcSocket.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class QwcServerInfoWidget : public QWidget, public Ui_QwcServerInfoWidget
{
	
Q_OBJECT

public:
	
    QwcServerInfoWidget(QWidget *parent = 0);
    ~QwcServerInfoWidget();
    void loadInfo(QwcSocket *);

};

#endif
