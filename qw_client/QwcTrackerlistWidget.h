#ifndef QWCTRACKERLISTWIDGET_H
#define QWCTRACKERLISTWIDGET_H

#include <QWidget>
#include <QPointer>

#include "ui_QwcTrackerlistWidget.h"
#include "QwcSocket.h"
#include "QwcTrackerInfo.h"

/**
	@author Bastian Bense <bb@bense.de>
*/
class QwcTrackerlistWidget : public QWidget, public Ui_QwcTrackerlistWidget
{
Q_OBJECT
public:
    QwcTrackerlistWidget(QWidget *parent = 0);
    ~QwcTrackerlistWidget();
    QPointer<QwcSocket> pSocket;

private:
	QPointer<QStandardItemModel> pModel;
	QPointer<QSortFilterProxyModel> pSortModel;
	void updateTrackerList();

signals:
        void newConnectionRequested(QString address);

private slots:
	
	void on_fBtnRefresh_clicked();
	void trackerServersReceived(QList<ClassTrackerServer> theList);
	void handleSocketError(QAbstractSocket::SocketError);
	void doubleclickedListItem(QModelIndex index );

};

#endif
