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


#ifndef CLASSWIREDEVENTFILTER_H
#define CLASSWIREDEVENTFILTER_H

#include <QObject>
#include "wired/wiredsocket.h"

class ClassWiredEventFilter : public QObject
{
    Q_OBJECT

public:
   ClassWiredEventFilter()
   {
       qDebug("Event filter created");
   }

   ~ClassWiredEventFilter()
   {
       qDebug("Event filter destroyed");
   }

   WiredSocket *pWiredSocket;

   void setSocket(WiredSocket *s);

   bool disconnectionPrompt(QEvent *event = 0);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

#endif
