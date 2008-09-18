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

 
#ifndef WIDGETCOLORBUTTON_H
#define WIDGETCOLORBUTTON_H

#include <QToolButton>

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetColorButton : public QToolButton
{
Q_OBJECT
public:
    WidgetColorButton(QWidget *parent = 0);
	WidgetColorButton(QColor theColor, QWidget *parent = 0);
    ~WidgetColorButton();
	QColor selectedColor();

private:
	QColor pColor;
	void updateIcon();

private slots:
	void requestColor();	
	
public slots:
	void setColor(QColor);

signals:
	void colorSelected(QColor);

};

#endif
