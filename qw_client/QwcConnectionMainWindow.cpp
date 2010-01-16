#include "QwcConnectionMainWindow.h"
#include "QwcAboutWidget.h"
#include "QwcSocket.h"

#include <QtCore/QUrl>

#include <QtGui/QDesktopServices>
#include <QtGui/QPainter>


/*! \class QwcConnectionMainWindow
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This is a QMainWindow derivate that provides the default connection window and toolbars. Its
    CentralWidget will be set to be a \a QwsChat instance by default, providing a standard user list.
*/

QwcConnectionMainWindow::QwcConnectionMainWindow(QWidget *parent) :
        QMainWindow(parent)
{
    setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);
    setWindowIcon(QIcon(":/icons/qwired_logo_32.png"));
    m_socket = NULL;
}


void QwcConnectionMainWindow::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }
    m_socket = socket;
    if (!socket) { return; }
    connect(m_socket, SIGNAL(newsPosted(QString,QDateTime,QString)),
            this, SLOT(handleSocketNewsPosted(QString,QDateTime,QString)));
}

QwcSocket* QwcConnectionMainWindow::socket()
{ return m_socket; }

/*! This is called from the try menu on Windows/Linux systems to hide and show the connection
    window properly.
*/
void QwcConnectionMainWindow::toggleVisible()
{
    if (this->isMinimized()) {
        this->showNormal();
    } else {
        if (this->isVisible()) {
            this->hide();
        } else {
            this->show();
        }
    }
}


void QwcConnectionMainWindow::handleSocketNewsPosted(QString nickname, QDateTime time, QString post)
{
    // Update the toolbar icon if the news are not currently visible
    QPixmap newIcon(":/icons/32x32/internet-news-reader.png");
    QPainter iconPainter(&newIcon);
    iconPainter.drawImage(16, 16, QImage(":/icons/16x16/emblem-important.png"));
    actionNews->setIcon(newIcon);
}


/*! This displays the about dialog to the user and is usually called from the "About" menu-item.
*/
void QwcConnectionMainWindow::on_actionAbout_triggered(bool)
{
    if (!pQwcAboutWidget) {
        pQwcAboutWidget = new QwcAboutWidget();
    }
    pQwcAboutWidget->show();
}

void QwcConnectionMainWindow::on_actionDonate_triggered()
{
    QDesktopServices::openUrl(QUrl("http://code.google.com/p/qwired-suite/wiki/SupportTheProject"));
}

void QwcConnectionMainWindow::on_actionNewConnection_triggered()
{ emit actionTriggered(TriggeredActionNewConnection); }

void QwcConnectionMainWindow::on_actionDisconnect_triggered()
{ emit actionTriggered(TriggeredActionDisconnect); }

void QwcConnectionMainWindow::on_actionReconnect_triggered()
{ emit actionTriggered(TriggeredActionReconnect); }

void QwcConnectionMainWindow::on_actionServerInfo_triggered()
{ emit actionTriggered(TriggeredActionServerInformation); }

void QwcConnectionMainWindow::on_actionServerNews_triggered()
{
    actionNews->setIcon(QIcon(":/icons/32x32/internet-news-reader.png"));
    emit actionTriggered(TriggeredActionNews);
}

void QwcConnectionMainWindow::on_actionServerMessages_triggered()
{ emit actionTriggered(TriggeredActionMessages); }

void QwcConnectionMainWindow::on_actionServerFiles_triggered()
{ emit actionTriggered(TriggeredActionFiles); }

void QwcConnectionMainWindow::on_actionServerTransfers_triggered()
{ emit actionTriggered(TriggeredActionTransfers); }

void QwcConnectionMainWindow::on_actionServerAccounts_triggered()
{ emit actionTriggered(TriggeredActionAccounts); }

void QwcConnectionMainWindow::on_actionServerBroadcast_triggered()
{ emit actionTriggered(TriggeredActionBroadcast); }

void QwcConnectionMainWindow::on_actionServerPreferences_triggered()
{ emit actionTriggered(TriggeredActionPreferences); }
