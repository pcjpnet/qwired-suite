#include "QwcGlobals.h"
#include "QwcNewsWidget.h"
#include "QwcMessageStyle.h"
#include "QwFile.h"
#include "QwcSocket.h"

#include <QMessageBox>
#include <QDebug>
#include <QTextCursor>
#include <QTextBlock>

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebElement>

#include <QDesktopServices>

/*! \class QwcNewsWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06
    \brief This is the news browser widget which allows interaction with the news system of the
           Wired protocol.
*/

QwcNewsWidget::QwcNewsWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUi(this);
    m_newsCounter = 0;

    m_socket = NULL;

    // Notification manager
    QwcSingleton *settings = &WSINGLETON::Instance();
    connect(settings, SIGNAL(applicationSettingsChanged()),
            this, SLOT(reloadPreferences()));

    pageWidget->setCurrentIndex(1); // jump to the waiting panel

    newsView->installEventFilter(this);
    newsView->page()->mainFrame()->
            setHtml(QString("<html><body>"
                            "<style type=\"text/css\">"
                            ".news_item { } "
                            ".news_header { font-weight: bold; display: block; padding-bottom: 8px; } "
                            ".news_body { display: block; padding-bottom: 12px; } "
                            "</style>"
                            "<style id=\"css_block\" type=\"text/css\"></style>"
                            "<span id=\"news_items\"></span>"
                            "</body></html>"));
    newsView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    reloadPreferences();
}


QwcNewsWidget::~QwcNewsWidget()
{
}


void QwcNewsWidget::setSocket(QwcSocket *socket)
{
    if (m_socket) { disconnect(m_socket, 0, this, 0); }
    m_socket = socket;
    if (!socket) { return; }

    connect(m_socket, SIGNAL(newsListingItem(QString,QDateTime,QString)),
            this, SLOT(addNewsItem(QString, QDateTime, QString)));
    connect(m_socket, SIGNAL(newsPosted(QString, QDateTime, QString)),
            this, SLOT(addNewsItemAtTop(QString, QDateTime, QString)));
    connect(m_socket, SIGNAL(newsListingDone()),
            this, SLOT(newsDone()));
    connect(m_socket, SIGNAL(receivedUserPrivileges()),
            this, SLOT(handleSocketPrivileges()));

    handleSocketPrivileges();
    m_socket->getNews();
}

QwcSocket* QwcNewsWidget::socket()
{ return m_socket; }


/*! The preferences were changed and we should reload the news.
*/
void QwcNewsWidget::reloadPreferences()
{
    QSettings settings;
    m_emoticonsEnabled = settings.value("interface/chat/emoticons", true).toBool();
    updateNewsCss();
    pageWidget->setCurrentIndex(1);
}


void QwcNewsWidget::addNewsItem(const QString &nickname, QDateTime time, const QString &text,
                                bool insertAtTop)
{
    QString messageText = text.trimmed();
    messageText = Qt::escape(messageText);
    messageText = QwcMessageStyle::replaceEmoticons(messageText, m_emoticonsEnabled);
    messageText.replace("\n", "<br>");

    QString newItem = QString("<div class=\"news_item\">"
                              "<span class=\"news_header\">From %1 (%2):</span>"
                              "<span class=\"news_body\">%3</span></div>")
                      .arg(Qt::escape(nickname))
                      .arg(Qt::escape(time.toString()))
                      .arg(messageText);

    QWebElement mainElement = newsView->page()->mainFrame()->findFirstElement("#news_items");
    if (insertAtTop) {
        mainElement.prependInside(newItem);
    } else {
        mainElement.appendInside(newItem);
    }

    m_newsCounter++;
    fNewsStatus->setText(tr("%1 news article(s)").arg(m_newsCounter));
}




void QwcNewsWidget::on_fBtnRefresh_clicked()
{
    QWebElement newsItemsElement = newsView->page()->mainFrame()->findFirstElement("#news_items");
    newsItemsElement.setPlainText(QString());

    m_newsCounter = 0;
    m_socket->getNews();
    pageWidget->setCurrentIndex(1);
}


/*! Show the composer/post window.
*/
void QwcNewsWidget::on_fBtnPost_clicked()
{
    pageWidget->setCurrentIndex(2);
    composeMessage->setFocus();
}


void QwcNewsWidget::on_fBtnDelete_clicked()
{
    if (QMessageBox::question(this, tr("Clear news"),
                              tr("Are you sure you want to clear all news items?\n\nThis cannot be undone."),
                              QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes) {
        m_newsCounter = 0;
        m_socket->clearNews();
    }
}


void QwcNewsWidget::on_btnComposeCancel_clicked()
{
    if (!composeMessage->toPlainText().isEmpty()) {
        if (!QMessageBox::question(this, QString(),
                tr("Are you sure you want to lose all changes to the current news article?"),
                QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Cancel) == QMessageBox::Cancel) {
            return;
        }
    }
    composeMessage->setPlainText("");
    pageWidget->setCurrentIndex(0);
}


/*! Send a new article to the server.
*/
void QwcNewsWidget::on_btnComposePost_clicked()
{
    if (composeMessage->toPlainText().isEmpty()) { return; }
    m_socket->postNews(composeMessage->toPlainText().trimmed());
    pageWidget->setCurrentIndex(0);
    composeMessage->setPlainText("");
}


void QwcNewsWidget::on_newsView_linkClicked(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}


/*! All news items have been received.
*/
void QwcNewsWidget::newsDone()
{
    pageWidget->setCurrentIndex(0);
}


void QwcNewsWidget::handleSocketPrivileges()
{
    Q_ASSERT(m_socket);
    fBtnPost->setEnabled(m_socket->sessionUser.privileges() & Qws::PrivilegePostNews);
    fBtnDelete->setEnabled(m_socket->sessionUser.privileges() & Qws::PrivilegeClearNews);
}




/*! Clear the contents of the news view and set an empty HTML page with styles.
*/
void QwcNewsWidget::updateNewsCss()
{
    QSettings settings;
    QFont newsFont;
    newsFont.fromString(settings.value("interface/news/font", QFont().toString()).toString());

    composeMessage->setFont(newsFont);

    QWebElement styleElement = newsView->page()->mainFrame()->findFirstElement("#css_block");
    styleElement.setPlainText(QString("body { %1 }").arg(QwcMessageStyle::cssFromFont(newsFont)));
}


bool QwcNewsWidget::eventFilter(QObject *what, QEvent *event)
{
    // Filter out the context menu events so that we don't have to worry about the user messing up
    // QWebView.
    if (what == newsView) {
        if (event->type() == QEvent::ContextMenu) {
            return true;
        }
    }
    return QWidget::eventFilter(what, event);
}
