#include "QwcConnectWidget.h"

QwcConnectWidget::QwcConnectWidget(QWidget *parent) : QWidget(parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    fContainer->setCurrentIndex(0);
    loadBookmarks();
    pReconnectTimerId = -1;

    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(loadBookmarks()) );
}


void QwcConnectWidget::on_btnConnect_clicked()
{
    fContainer->setCurrentIndex(1);
    fStatus->setText(tr("Connecting..."));
    emit onConnnectReady(fAddress->text(), fLogin->text(), fPassword->text());
}


void QwcConnectWidget::on_fBtnCancel_clicked()
{
    emit onConnectAborted();
    resetForm();
}


void QwcConnectWidget::bookmarkSelected(QAction *action)
{
    QStringList tmpList = action->data().value<QStringList>();
    fAddress->setText(tmpList.at(0));
    fLogin->setText(tmpList.at(1));
    fPassword->setText(tmpList.at(2));
    btnConnect->click();
}


void QwcConnectWidget::loadBookmarks()
{
    QSettings conf;
    QMenu *menu = fBtnFavourites->menu();
    if(!menu) {
        menu = new QMenu(fBtnFavourites);
        connect( menu, SIGNAL(triggered(QAction*)), this, SLOT(bookmarkSelected(QAction*)) );
        fBtnFavourites->setMenu(menu);
    }
    menu->clear();
    int tmpCnt = conf.beginReadArray("bookmarks");
    for(int i=0; i<tmpCnt; ++i) {
        conf.setArrayIndex(i);
        QStringList tmpData;
        tmpData << conf.value("address").toString() << conf.value("login").toString() << conf.value("password").toString();
        QAction *action = new QAction(menu);
        action->setText( conf.value("name").toString() );
        action->setIcon(QIcon(":/icons/icn_trackerserver.png"));
        action->setData(tmpData);
        menu->addAction(action);
    }
    conf.endArray();
}


void QwcConnectWidget::timerEvent(QTimerEvent *)
{
    killTimer(pReconnectTimerId);
    pReconnectTimerId = -1;
    on_btnConnect_clicked();
}


void QwcConnectWidget::stopReconnecting()
{
    fContainer->setCurrentIndex(0);
    if(pReconnectTimerId>-1)
        this->killTimer(pReconnectTimerId);

}


void QwcConnectWidget::startReconnecting()
{
    fContainer->setCurrentIndex(1);
    fStatus->setText(tr("Waiting for reconnect..."));
    pReconnectTimerId = this->startTimer(5*1000);
    fProgress->setMaximum(0);
}


void QwcConnectWidget::resetForm()
{
    fContainer->setCurrentIndex(0);
}


void QwcConnectWidget::setProgressBar(int value, int max)
{
    fProgress->setMaximum(max);
    fProgress->setValue(value);
}


void QwcConnectWidget::setStatus(const QString &status)
{
    fStatus->setText(status);
}
