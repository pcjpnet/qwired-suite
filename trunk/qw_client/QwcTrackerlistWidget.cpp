#include "QwcTrackerlistWidget.h"

#include <QSettings>
#include <QMessageBox>

QwcTrackerlistWidget::QwcTrackerlistWidget(QWidget *parent) : QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setupUi(this);
    pSocket = new QwcSocket(this);
	
        connect( pSocket, SIGNAL(trackerServersReceived(QList<QwcTrackerServerInfo>)),
                         this, SLOT(trackerServersReceived(QList<QwcTrackerServerInfo>)) );

	connect( pSocket, SIGNAL(onSocketError(QAbstractSocket::SocketError)),
			 this, SLOT(handleSocketError(QAbstractSocket::SocketError)) );
	
	fProgress->setVisible(false);

	pModel = new QStandardItemModel(fList);
	pSortModel = new QSortFilterProxyModel(fList);
	pSortModel->setSourceModel(pModel);
	pSortModel->setSortRole(Qt::UserRole+1);
	pSortModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	pSortModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	pSortModel->setFilterKeyColumn(-1);
	connect(fFilter, SIGNAL(textEdited(QString)), pSortModel, SLOT(setFilterWildcard(QString)));
	fList->setModel(pSortModel);
	QStringList tmpHeaders;
	tmpHeaders << tr("Name") << tr("Users") << tr("Bandwidth") << tr("Guest");
	tmpHeaders << tr("Download") << tr("Files") << tr("Size") << tr("Description") << tr("Address");
	pModel->setHorizontalHeaderLabels(tmpHeaders);
	
	connect(fList, SIGNAL(doubleClicked ( const QModelIndex &)), this, SLOT(doubleclickedListItem(QModelIndex)));
        connect(fServers, SIGNAL(currentIndexChanged(int)), this, SLOT(on_fBtnRefresh_clicked()));

	updateTrackerList();
	if(fServers->count()) {
		fServers->setCurrentIndex(0);
		on_fBtnRefresh_clicked();
	}

	
}


QwcTrackerlistWidget::~QwcTrackerlistWidget()
{ }

void QwcTrackerlistWidget::updateTrackerList() {
	QSettings s;
	int tmpCnt = s.beginReadArray("trackers");
	fServers->clear();
	for(int i=0; i<tmpCnt; i++) {
		s.setArrayIndex(i);
		fServers->addItem(s.value("name").toString(), s.value("address"));
	}
	s.endArray();
}

void QwcTrackerlistWidget::on_fBtnRefresh_clicked() {
// 	fList->clear();
	fProgress->setVisible(true);
        fProgress->setText(tr("Refreshing list..."));
	//fList->setEnabled(false);
	fFilter->setEnabled(false);
	fBtnRefresh->setEnabled(false);
	QString address = fServers->itemData( fServers->currentIndex() ).toString();
	pSocket->connectToTracker( address);
}

void QwcTrackerlistWidget::trackerServersReceived(QList< QwcTrackerServerInfo > theList) {
	fList->setEnabled(true);
	fFilter->setEnabled(true);
	fBtnRefresh->setEnabled(true);
	pModel->setRowCount(theList.count());
	
	for(int i=0; i<theList.count(); i++) {
                fProgress->setText(QString(tr("%1 servers registered with tracker")).arg(int(i+1)));
                QwcTrackerServerInfo ts = theList.value(i);
		QStandardItem *item;

		item = new QStandardItem(ts.name);
		item->setData(ts.name);
		item->setIcon(QIcon(":/icons/icn_trackerserver.png"));
		pModel->setItem(i, 0, item );
		
		item = new QStandardItem(QString::number(ts.userCount));
		item->setData(ts.userCount);
		pModel->setItem(i, 1, item );

        item = new QStandardItem(ts.bandwidth ? QwcFileInfo::humanReadableSize(ts.bandwidth).append("it") : tr("n/a"));
		item->setData(ts.bandwidth);
		pModel->setItem(i, 2, item);

		item = new QStandardItem(ts.guestAllowed ? tr("Yes") : tr("No"));
		item->setData(ts.guestAllowed);
		pModel->setItem(i, 3, item );

		item = new QStandardItem(ts.downloadAllowed ? tr("Yes") : tr("No"));
		item->setData(ts.downloadAllowed);
		pModel->setItem(i, 4, item );

		item = new QStandardItem(ts.fileCount ? QString::number(ts.fileCount) : tr("n/a"));
		item->setData(ts.fileCount);
		pModel->setItem(i, 5, item );

        item = new QStandardItem(ts.fileTotalSize ? QwcFileInfo::humanReadableSize(ts.fileTotalSize) : tr("n/a"));
		item->setData(ts.fileTotalSize);
		pModel->setItem(i, 6, item);

		item = new QStandardItem(ts.description);
		item->setData(ts.description);
		pModel->setItem(i, 7, item );

		item = new QStandardItem(ts.address);
		item->setData(ts.address);
		pModel->setItem(i, 8, item );

	}

        fList->resizeColumnToContents(0);
        fList->resizeColumnToContents(1);
        fList->resizeColumnToContents(2);
        fList->resizeColumnToContents(3);
        fList->resizeColumnToContents(4);
        fList->resizeColumnToContents(5);
        fList->resizeColumnToContents(6);
        fList->resizeColumnToContents(7);
        fList->resizeColumnToContents(8);

}

void QwcTrackerlistWidget::handleSocketError(QAbstractSocket::SocketError error) {
	QMessageBox::critical(this, tr("Connection Error"), tr("A connection error occured while trying to connect to the tracker.\nReason: %1").arg(error));
	fBtnRefresh->setEnabled(true);
	fProgress->setVisible(false);
}

void QwcTrackerlistWidget::doubleclickedListItem(QModelIndex index )
{
    int row = index.row();
    QString address = index.model()->index(row,8).data().toString();
    emit(newConnectionRequested(address.mid(8,address.length()-9)));
}




