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

 
#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>

/**
A simple Singleton template, used for basic Singleton needs. :)

	@author Bastian Bense <bastibense@gmail.com>
*/
template <class T>
class Singleton
{
public:
	static T& Instance() {
		static T _instance; // create static instance of our class
		return _instance;   // return it
	}
	
private:
	Singleton();	// hide constructor
	~Singleton();	// hide destructor
	Singleton(Singleton const&); // hide copy constructor
	Singleton& operator=(Singleton const&); // hide assign op
};
#endif
