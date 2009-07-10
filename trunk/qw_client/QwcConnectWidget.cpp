#include "QwcConnectWidget.h"
#include "QwFile.h"

#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

/*! \class QwcConnectWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief This widget provides the login prompt when connecting manually to a server.
    It asks the user for a server name, login name and password; and provides a connect button to
    establish the connection to the server.
*/

QwcConnectWidget::QwcConnectWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    fContainer->setCurrentIndex(0);
    loadBookmarks();
    pReconnectTimerId = -1;

    updateTrackerMenu();

    connect(fAddress, SIGNAL(returnPressed()),
            this, SLOT(on_btnConnect_clicked()));
    connect(fLogin, SIGNAL(returnPressed()),
            this, SLOT(on_btnConnect_clicked()));
    connect(fPassword, SIGNAL(returnPressed()),
            this, SLOT(on_btnConnect_clicked()));

    // Notification manager
    QwcSingleton *singleton = &WSINGLETON::Instance();
    connect(singleton, SIGNAL(prefsChanged()),
            this, SLOT(loadBookmarks()) );
    connect(singleton, SIGNAL(prefsChanged()),
            this, SLOT(updateTrackerMenu()) );

    // Tracker Socket
    trackerSocket = new QwTrackerClientSocket(this);
    trackerSocket->autoCommand = Qw::TrackerClientAutoCommandSERVERS;
    connect(trackerSocket, SIGNAL(receivedServers(QList<QwServerInfo>&)),
            this, SLOT(handleTrackerServers(QList<QwServerInfo>&)));
}


/*! The "Trackers..." button has been clicked.
*/
void QwcConnectWidget::on_btnConnectTrackers_clicked()
{
    fContainer->setCurrentWidget(pageTrackers);
}


void QwcConnectWidget::on_btnTrackerManual_clicked()
{
    fContainer->setCurrentWidget(pageConnect);
}


void QwcConnectWidget::on_btnTrackerRefresh_clicked()
{
    if (trackerList->currentIndex() == -1) { return; }
    trackerSocket->connectToTracker(trackerList->itemData(trackerList->currentIndex()).toString());

}


/*! Handle the returned list of tracker server entries.
*/
void QwcConnectWidget::handleTrackerServers(QList<QwServerInfo> &servers)
{
    trackerServerList->clear();
    QListIterator<QwServerInfo> i(servers);
    while (i.hasNext()) {
        QwServerInfo item = i.next();
        QTreeWidgetItem *newItem = new QTreeWidgetItem(trackerServerList);
        newItem->setIcon(0, QIcon(":icons/icon_favourite.png"));
        newItem->setText(0, item.name);
        newItem->setText(1, QString::number(item.userCount));
        newItem->setText(4, tr("%1 (%2 files)")
                         .arg(QwFile::humanReadableSize(item.filesSize))
                         .arg(item.filesCount));
        newItem->setText(2, item.canGuests ? tr("Yes") : tr("No"));
        newItem->setText(3, item.canDownload ? tr("Yes") : tr("No"));
        newItem->setText(6, tr("%1/s").arg(QwFile::humanReadableSize(item.bandwidth)));
        newItem->setText(7, item.category);
        newItem->setText(5, item.description);
        newItem->setText(8, item.url);
    }

    for (int column = 0; column < 8; column++) {
        trackerServerList->resizeColumnToContents(column);
    }

}


/*! Update the list of trackers from the system settings.
*/
void QwcConnectWidget::updateTrackerMenu()
{
    trackerList->clear();
    QSettings settings;
    int arraySize = settings.beginReadArray("trackers");
    for (int i = 0; i < arraySize; i++) {
        settings.setArrayIndex(i);
        trackerList->addItem(settings.value("name").toString(),
                             settings.value("address"));
    }
    settings.endArray();
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
                                                    QString("%1 (%2)")
                                                        .arg(fAddress->text())
                                                        .arg(fLogin->text().isEmpty() ? tr("guest") : fLogin->text()),
                                                    &ok);
        if (!ok) { return; }

        // Check if the name is empty
        if (bookmarkName.trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Please enter a name for the bookmark."),
                                 tr("You can not create a bookmark without a name. Entering a short "
                                    "and specific name for a bookmark helps to identify it later."));
            return;
        }

        // Check if the bookmark already exists
        QSettings settings;
        int bookmarkCount = settings.beginReadArray("bookmarks");
        for (int i = 0; i < bookmarkCount; i++) {
            settings.setArrayIndex(i);
            if (settings.value("name") == bookmarkName) {
                QMessageBox::warning(this, tr("A bookmark with the same name already exists."),
                                     tr("You can not create a bookmark with the name of an existing "
                                        "bookmark. Please choose another name for this bookmark."));
                settings.endArray();
                return;
            }
        }
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

    // Load the bookmark data and connect
    QStringList tmpList = action->data().value<QStringList>();
    fAddress->setText(tmpList.at(0));
    fLogin->setText(tmpList.at(1));
    fPassword->setText(tmpList.at(2));
    btnConnect->click();
}


/*! Update the bookmarks popup-menu.
*/
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
