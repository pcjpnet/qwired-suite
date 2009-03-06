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

 
#ifndef QwcFileSearchWidget_H
#define QwcFileSearchWidget_H

#include "ui_QwcFileSearchWidget.h"
#include "QwcFileInfo.h"

/**
	@author Bastian Bense <bastibense@gmail.com>
 */
class QwcFileSearchWidget : public QWidget, private Ui::QwcFileSearchWidget
{
Q_OBJECT
public:
    QwcFileSearchWidget(QWidget *parent = 0);
    ~QwcFileSearchWidget();

signals:
	void search(QString theSearch);
	void downloadFile(QString theFile);
	void revealFile(QString thePath);
	
private slots:
	void on_fBtnDownload_clicked(bool);
	void on_fSearch_returnPressed();
	void on_fResults_currentRowChanged(int row);
	void on_fBtnReveal_clicked(bool);
	
public slots:
    void updateResults(QList<QwcFileInfo> theResults);
};

#endif
