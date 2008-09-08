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
#include "WidgetFileInfo.h"

WidgetFileInfo::WidgetFileInfo(QWidget *parent)
 : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
}

WidgetFileInfo::~WidgetFileInfo()
{ }

void WidgetFileInfo::loadFromFile(const ClassWiredFile theFile) {
	pFile = theFile;
	fName->setText(theFile.fileName());
	fIcon->setPixmap(theFile.fileIcon().pixmap(16,16));
	fSize->setText(QString("%1 (%2 bytes)").arg(ClassWiredFile::humanReadableSize(theFile.size)).arg(theFile.size));
	fPath->setText(theFile.path);
	fModified->setText(theFile.modified.toString());
	fCreated->setText(theFile.created.toString());
	fChecksum->setText(theFile.checksum);
	fComments->setText(theFile.comment);
}

