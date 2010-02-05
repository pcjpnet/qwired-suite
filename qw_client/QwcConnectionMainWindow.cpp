#include "QwcConnectionMainWindow.h"
#include "QwcAboutWidget.h"
#include "QwcSocket.h"

#include <QtCore/QUrl>
#include <QtCore/QSettings>

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

    if (qApp->style()->objectName().startsWith("macintosh")) {
        setUnifiedTitleAndToolBarOnMac(true);
    }

    setWindowIcon(QIcon(":/icons/qwired_logo_32.png"));
    m_socket = NULL;

    // Restore the window geometry
    QSettings settings;
    if (settings.contains("window_states/session")) {
        restoreGeometry(settings.value("window_states/session").toByteArray());
    }
}


void QwcConnectionMainWindow::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }
    m_socket = socket;
    if (!socket) { return; }

    connect(m_socket, SIGNAL(newsPosted(QString,QDateTime,QString)),
            this, SLOT(handleSocketNewsPosted(QString,QDateTime,QString)));
    connect(m_socket, SIGNAL(receivedUserPrivileges()),
            this, SLOT(handleSocketReceivedPrivileges()));
    connect(m_socket, SIGNAL(onServerLoginSuccessful()),
            this, SLOT(handleSocketLoginSuccessful()));
    connect(m_socket, SIGNAL(connectionLost()),
            this, SLOT(handleSocketDisconnected()));
    connect(m_socket, SIGNAL(privateMessage(QwcUserInfo,QString)),
            this, SLOT(handleSocketPrivateMessage()));
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
    Q_UNUSED(nickname);
    Q_UNUSED(time);
    Q_UNUSED(post);

    // Update the toolbar icon if the news are not currently visible
    QPixmap newIcon(":/icons/32x32/internet-news-reader.png");
    QPainter iconPainter(&newIcon);
    iconPainter.drawImage(16, 16, QImage(":/icons/16x16/emblem-important.png"));
    actionNews->setIcon(newIcon);
}


void QwcConnectionMainWindow::handleSocketReceivedPrivileges()
{
    Qws::Privileges privs = m_socket->sessionUser().privileges();
    actionAccounts->setEnabled(privs & Qws::PrivilegeCreateAccounts
                               || privs & Qws::PrivilegeDeleteAccounts
                               || privs & Qws::PrivilegeEditAccounts);
    actionFiles->setEnabled(privs & Qws::PrivilegeAlterFiles
                            || privs & Qws::PrivilegeCreateFolders
                            || privs & Qws::PrivilegeDeleteFiles
                            || privs & Qws::PrivilegeDownload
                            || privs & Qws::PrivilegeUpload
                            || privs & Qws::PrivilegeUploadAnywhere
                            || privs & Qws::PrivilegeViewDropboxes);
    actionBroadcast->setEnabled(privs & Qws::PrivilegeSendBroadcast);
    actionNewsPost->setEnabled(privs & Qws::PrivilegePostNews);
}


void QwcConnectionMainWindow::handleSocketConnected()
{
    actionDisconnect->setEnabled(true);
    actionReconnect->setEnabled(true);
}


void QwcConnectionMainWindow::handleSocketDisconnected()
{
    actionDisconnect->setEnabled(false);

    actionServerInfo->setEnabled(false);
    actionNews->setEnabled(false);
    actionMessages->setEnabled(false);
    actionChat->setEnabled(false);

    actionAccounts->setEnabled(false);
    actionFiles->setEnabled(false);
    actionBroadcast->setEnabled(false);
    actionNewsPost->setEnabled(false);
}

void QwcConnectionMainWindow::handleSocketLoginSuccessful()
{
    actionServerInfo->setEnabled(true);
    actionNews->setEnabled(true);
    actionMessages->setEnabled(true);
    actionChat->setEnabled(true);
    setWindowTitle(tr("%1 - Qwired").arg(m_socket->serverInformation().name));
}


void QwcConnectionMainWindow::handleSocketPrivateMessage()
{
    // Update the toolbar icon if the news are not currently visible
    QPixmap newIcon(":/icons/32x32/mail-message-new.png");
    QPainter iconPainter(&newIcon);
    iconPainter.drawImage(16, 16, QImage(":/icons/16x16/emblem-important.png"));
    actionMessages->setIcon(newIcon);
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

void QwcConnectionMainWindow::on_actionChat_triggered()
{ emit actionTriggered(TriggeredActionChat); }

void QwcConnectionMainWindow::on_actionDisconnect_triggered()
{ emit actionTriggered(TriggeredActionDisconnect); }

void QwcConnectionMainWindow::on_actionReconnect_triggered()
{ emit actionTriggered(TriggeredActionReconnect); }

void QwcConnectionMainWindow::on_actionServerInfo_triggered()
{ emit actionTriggered(TriggeredActionServerInformation); }

void QwcConnectionMainWindow::on_actionNews_triggered()
{
    actionNews->setIcon(QIcon(":/icons/32x32/internet-news-reader.png"));
    emit actionTriggered(TriggeredActionNews);
}

void QwcConnectionMainWindow::on_actionMessages_triggered()
{
    actionMessages->setIcon(QIcon(":/icons/32x32/mail-message-new.png"));
    emit actionTriggered(TriggeredActionMessages);
}

void QwcConnectionMainWindow::on_actionFiles_triggered()
{ emit actionTriggered(TriggeredActionFiles); }

void QwcConnectionMainWindow::on_actionTransfers_triggered()
{ emit actionTriggered(TriggeredActionTransfers); }

void QwcConnectionMainWindow::on_actionAccounts_triggered()
{ emit actionTriggered(TriggeredActionAccounts); }

void QwcConnectionMainWindow::on_actionBroadcast_triggered()
{ emit actionTriggered(TriggeredActionBroadcast); }

void QwcConnectionMainWindow::on_actionPreferences_triggered()
{ emit actionTriggered(TriggeredActionPreferences); }
