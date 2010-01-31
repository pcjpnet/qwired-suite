 
#include "QwcUserInfoWidget.h"
#include "QwTransferInfo.h"
#include <QProgressBar>

QwcUserInfoWidget::QwcUserInfoWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    pUserID = 0;
}


QwcUserInfoWidget::~QwcUserInfoWidget()
{
}

void QwcUserInfoWidget::setUser(QwcUserInfo theUser)
{
    fStatus->setText( theUser.userStatus );
    fNick->setText( theUser.userNickname );
    fLogin->setText( theUser.name );
    fID->setText( QString::number(theUser.pUserID) );
    fAddress->setText( theUser.userIpAddress );
    fHost->setText( theUser.userHostName );
    fVersion->setText( theUser.pClientVersion );
    fCipher->setText( theUser.pCipherName+QString(" (%1 bits)").arg(theUser.pCipherBits) );
    fLoginTime->setText( theUser.pLoginTime.toString() );
    fIdleTime->setText( theUser.pIdleTime.toString() );

    fIcon->setPixmap(QPixmap::fromImage(theUser.userImage()));

    setWindowTitle(tr("User Information: %1").arg(theUser.userNickname));
    pUserID = theUser.pUserID;
    loadTransfers(theUser);

}

void QwcUserInfoWidget::loadTransfers(const QwcUserInfo &theUser)
{
    transfersList->clear();

    foreach (QwTransferInfo transfer, theUser.transfers()) {
        QTreeWidgetItem *transferItem = new QTreeWidgetItem(transfersList);
        transferItem->setText(0, transfer.file.fileName());
        transferItem->setText(1, tr("%1 of %2 at %3/s")
                              .arg(QwFile::humanReadableSize(transfer.bytesTransferred))
                              .arg(QwFile::humanReadableSize(transfer.file.size()))
                              .arg(QwFile::humanReadableSize(transfer.currentTransferSpeed)));

        switch (transfer.type) {
        case Qw::TransferTypeDownload:
        case Qw::TransferTypeFolderDownload:
            transferItem->setIcon(0, QIcon(":/icons/16x16/go-down.png"));
            break;
        case Qw::TransferTypeUpload:
        case Qw::TransferTypeFolderUpload:
            transferItem->setIcon(0, QIcon(":/icons/16x16/go-up.png"));
            break;
        }

    }

//	// theType: 0=Downloads, 1=Uploads
//	QByteArray tmpData;
//	QGroupBox *tmpGroupBox;
//	if( theType==0 ) {
//		// Downloads
//		tmpData = theUser.pDownloads;
//		tmpGroupBox = fGroupDownloads;
//	} else {
//		// Uploads
//		tmpData = theUser.pUploads;
//		tmpGroupBox = fGroupUploads;
//	}
//
//	QVBoxLayout *tmpLayDown = new QVBoxLayout(fGroupDownloads);
//	if( tmpData.isEmpty() ) {
//		tmpGroupBox->setVisible(false);
//	} else {
//		// List Transfers
//		QList<QByteArray> tmpList = tmpData.split(0x1D);
//		QListIterator<QByteArray> i(tmpList);
//		while( i.hasNext() ) {
//			QList<QByteArray> tmpFields = i.next().split(0x1E);
//			QString tmpPath = tmpFields.value(0);
//			int tmpDone = tmpFields.value(1).toInt();
//			int tmpSize = tmpFields.value(2).toInt();
//			float tmpSpeed = tmpFields.value(3).toFloat();
//
//			QFont font;
//			font.setPixelSize(10);
//			font.setBold(false);
//
//			// Name
//			QLabel *tmpLab = new QLabel( tmpPath, tmpGroupBox);
//			tmpLab->setFont(font);
//			tmpLayDown->addWidget(tmpLab);
//
//			// Progress
//			QProgressBar *tmpStat = new QProgressBar(tmpGroupBox);
//			tmpStat->setFixedHeight(10);
//			tmpStat->setTextVisible(false);
//			tmpStat->setMaximum(tmpSize/1024/1024);
//			tmpStat->setValue(tmpDone/1024/1024);
//			tmpLayDown->addWidget(tmpStat);
//
//			// More Info
//			QString tmpInfo = tr("%1 MB of %2 MB at %3 KiB/sec")
//					.arg( (float)tmpDone/1024/1024 )
//					.arg( (float)tmpSize/1024/1024 )
//					.arg( tmpSpeed/1024 );
//
//			QLabel *tmpMore = new QLabel( tmpInfo, tmpGroupBox);
//			tmpMore->setFont(font);
//			tmpLayDown->addWidget(tmpMore);
//
//			if( i.hasNext() )
//				tmpLayDown->addSpacing(8);
//
//			qDebug() <<"Path:"<<tmpPath <<"Done:"<< tmpDone <<"Size:"<<tmpSize <<"Speed:"<<tmpSpeed;
//		}
//	}
//	tmpLayDown->setSpacing(0);
//	tmpLayDown->setMargin(6);
//	tmpGroupBox->setLayout(tmpLayDown);
}


