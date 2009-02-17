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

 
#ifndef DIALOGNEWSPOST_H
#define DIALOGNEWSPOST_H

#include <QDialog>
#include "ui_WidgetNewsPost.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */

class WidgetNewsPost
	: public QWidget, private Ui_WidgetNewsPost
{
	Q_OBJECT

	public:

		WidgetNewsPost(QWidget *parent = 0 )
		{
			Q_UNUSED(parent)
			setAttribute(Qt::WA_DeleteOnClose);
			setupUi(this);
                        connect(fBtnPost,SIGNAL(released()),this,SLOT(sendButtonClicked()));
                        connect(fBtnCancel,SIGNAL(released()),this,SLOT(cancelButtonClicked()));
		};

		
		~WidgetNewsPost()
		{ };

		
		QString getPostText()
		{
			return fNews->toPlainText();
		};

        private slots:
                void sendButtonClicked()
                {
                    emit accepted();
                }

                void cancelButtonClicked()
                {
                    emit rejected();
                }

        signals:
                void accepted();
                void rejected();

};


#endif
