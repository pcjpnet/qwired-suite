/*! \class Singleton
    \author Bastian Bense <bastibense@gmail.com>
    \brief A simple singleton template class for use in applications.
*/

#ifndef SINGLETON_H
#define SINGLETON_H

#include <QObject>

template <class T>
class Singleton
{
public:
    static T& Instance()
    {
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
