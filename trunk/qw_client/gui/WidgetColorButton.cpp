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

 
#include "../gui/WidgetColorButton.h"
#include <QColorDialog>
#include <QPainter>


WidgetColorButton::WidgetColorButton(QWidget *parent)
 : QToolButton(parent)
{
	connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
	updateIcon();
}

WidgetColorButton::WidgetColorButton(QColor theColor, QWidget *parent)
	
{
	Q_UNUSED(parent)
	connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
	pColor = theColor;
	updateIcon();
}


WidgetColorButton::~WidgetColorButton()
{ }

QColor WidgetColorButton::selectedColor() {
	return pColor;
}

void WidgetColorButton::setColor(QColor theColor) {
	pColor = theColor;
	updateIcon();
}

void WidgetColorButton::requestColor() {
	QColor tmpC = QColorDialog::getColor(pColor);
	pColor = tmpC;
	updateIcon();
	emit colorSelected(pColor);
}

void WidgetColorButton::updateIcon() {
	QPainter p;
	QPixmap pm(18,18);
	p.begin(&pm);
// 	p.setBrush(pColor);
	p.fillRect(0,0,18,18,pColor);
	p.end();
	this->setIcon(pm);
}




