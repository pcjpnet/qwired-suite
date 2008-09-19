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
#include <QPainter>
#include <QColorDialog>

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class WidgetColorButton
	: public QToolButton
{
	
Q_OBJECT
		
public:
	
    WidgetColorButton(QWidget *parent = 0)
	{
		Q_UNUSED(parent)
		connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
		updateIcon();
	};

	WidgetColorButton(QColor theColor, QWidget *parent = 0)
	{
		Q_UNUSED(parent)
		connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
		pColor = theColor;
		updateIcon();
	};

	~WidgetColorButton()
	{ };

	QColor selectedColor()
	{
		return pColor;
	};

	
private:
	
	QColor pColor;
	
	void updateIcon()
	{
		QPainter p;
		QPixmap pm(18,18);
		p.begin(&pm);
		p.fillRect(0,0,18,18,pColor);
		p.end();
		this->setIcon(pm);
	};

	
private slots:
	void requestColor()
	{
		QColor tmpC = QColorDialog::getColor(pColor);
		pColor = tmpC;
		updateIcon();
		emit colorSelected(pColor);
	};	

	
public slots:
	
	void setColor(QColor theColor)
	{
		pColor = theColor;
		updateIcon();
	};

	
signals:
	void colorSelected(QColor);

};

#endif
