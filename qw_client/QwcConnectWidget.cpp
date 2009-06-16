#include "QwcConnectWidget.h"

#include <QDebug>
#include <QInputDialog>

/*! \class QwcConnectWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief This widget provides the login prompt when connecting manually to a server.
    It asks the user for a server name, login name and password; and provides a connect button to
    establish the connection to the server.
*/

QwcConnectWidget::QwcConnectWidget(QWidget *parent) : QWidget(parent)
{
    Q_UNUSED(parent);
    setupUi(this);
    fContainer->setCurrentIndex(0);
    loadBookmarks();
    pReconnectTimerId = -1;

    connect(fAddress, SIGNAL(returnPressed()),
            this, SLOT(on_btnConnect_clicked()));
    connect(fLogin, SIGNAL(returnPressed()),
            this, SLOT(on_btnConnect_clicked()));
    connect(fPassword, SIGNAL(returnPressed()),
            this, SLOT(on_btnConnect_clicked()));

    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect( tmpS, SIGNAL(prefsChanged()), this, SLOT(loadBookmarks()) );
}


void QwcConnectWidget::on_btnConnect_clicked()
{
    fContainer->setCurrentIndex(1);
    fStatus->setText(tr("Connecting..."));
    emit userFinished(fAddress->text(), fLogin->text(), fPassword->text());
}


void QwcConnectWidget::on_fBtnCancel_clicked()
{
    emit onConnectAborted();
    resetForm();
}


void QwcConnectWidget::bookmarkSelected(QAction *action)
{
    if (!action) { return; }
    if (action->text() == tr("Add Bookmark...")) {
        bool ok;
        QString bookmarkName = QInputDialog::getText(this,
                                                    tr("Create New Bookmark"),
                                                    tr("Please enter a name for the new bookmark:"),
                                                    QLineEdit::Normal,
                                                    fAddress->text(),
                                                    &ok);
        if (!ok) { return; }

        // Actually add the new bookmark
        QSettings settings;
        // Read the count of existing entries
        int bookmarkCount = settings.beginReadArray("bookmarks");
        settings.endArray();
        // Write the new entry
        settings.beginWriteArray("bookmarks");
        settings.setArrayIndex(bookmarkCount);
        settings.setValue("name", bookmarkName);
        settings.setValue("address", fAddress->text());
        settings.setValue("login", fLogin->text());
        settings.setValue("password", fPassword->text());
        settings.endArray();

        loadBookmarks();
        return;
    }
    QStringList tmpList = action->data().value<QStringList>();
    fAddress->setText(tmpList.at(0));
    fLogin->setText(tmpList.at(1));
    fPassword->setText(tmpList.at(2));
    btnConnect->click();
}


void QwcConnectWidget::loadBookmarks()
{
    QSettings settings;

    QMenu *menu = fBtnFavourites->menu();
    if(!menu) {
        menu = new QMenu(fBtnFavourites);
        connect(menu, SIGNAL(triggered(QAction*)),
                this, SLOT(bookmarkSelected(QAction*)));
        fBtnFavourites->setMenu(menu);
    }

    menu->clear();

    int bookmarkCount = settings.beginReadArray("bookmarks");
    for (int i = 0; i < bookmarkCount; i++) {
        settings.setArrayIndex(i);
        QStringList tmpData = QStringList()
                << settings.value("address").toString()
                << settings.value("login").toString()
                << settings.value("password").toString();
        QAction *action = new QAction(menu);
        action->setText(settings.value("name").toString());
        action->setIcon(QIcon(":/icons/icn_trackerserver.png"));
        action->setData(tmpData);
        menu->addAction(action);
    }
    settings.endArray();

    // Add the 'special' action that allows to add a bookmark automatically
    menu->addSeparator();
    QAction *addAction = new QAction(menu);
    addAction->setText(tr("Add Bookmark..."));
    addAction->setIcon(QIcon(":/icons/16x16/list-add.png"));
    menu->addAction(addAction);
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
