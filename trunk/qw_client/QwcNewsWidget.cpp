#include "QwcGlobals.h"
#include "QwcNewsWidget.h"
#include "QwFile.h"

#include <QMessageBox>
#include <QDebug>
#include <QTextCursor>
#include <QTextBlock>

#include <QtWebKit>
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
    initPrefs();

    newsCounter = 0;

    // Notification manager
    QwcSingleton *tmpS = &WSINGLETON::Instance();
    connect(tmpS, SIGNAL(applicationSettingsChanged()),
            this, SLOT(reloadPreferences()));


    pageWidget->setCurrentIndex(1); // jump to the waiting panel


    newsView->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    resetNewsView();


}


QwcNewsWidget::~QwcNewsWidget()
{
}


/*! The preferences were changed and we should reload the news.
*/
void QwcNewsWidget::reloadPreferences()
{
    initPrefs();
    resetNewsView();
    newsCounter = 0;
    updateNewsStats();
    pageWidget->setCurrentIndex(1);
    emit requestedRefresh();
}


void QwcNewsWidget::initPrefs()
{
    pColorText =  QwcSingleton::colorFromPrefs("interface/news/text/color", Qt::black);
    pColorTitle = QwcSingleton::colorFromPrefs("interface/news/titles/color", Qt::gray);

    // Set the default font for the news reader
    QSettings settings;
    newsFont = QFont(settings.value("interface/news/font", QwcSingleton::systemMonospaceFont()).toString().section(",",0,0),
                   settings.value("interface/news/font", QwcSingleton::systemMonospaceFont()).toString().section(",",1,1).toInt());

    composeMessage->document()->setDefaultFont(newsFont);
}


/*! Update the news statistics bar in the bottom of the widget.
*/
void QwcNewsWidget::updateNewsStats()
{
    fNewsStatus->setText(tr("%1 news article(s)")
                         .arg(newsCounter));
    fBtnDelete->setEnabled(newsCounter > 0);
}


void QwcNewsWidget::addNewsItem(QString theNick, QDateTime time, QString thePost, bool insertAtTop)
{

    
    QRegExp urlRegex("([a-z0-9]+://[a-zA-Z0-9._-]+(:[0-9]*)?([a-z0-9/\\?&=_\\.\\-%]*))",
                     Qt::CaseInsensitive);
    QString messageText = thePost.trimmed();
    messageText = Qt::convertFromPlainText(messageText, Qt::WhiteSpaceNormal);
    messageText.replace(urlRegex, "<a href=\"\\1\">\\1</a>");


    QString newItem = QString("<span class=\"news_item\">"
                              "<span class=\"news_header\">From %1 (%2):</span>"
                              "<span class=\"news_body\">%3</span>"
                              "</span>")
                      .arg(Qt::escape(theNick))
                      .arg(Qt::escape(time.toString()))
                      .arg(messageText);


    QWebElement mainElement = newsView->page()->mainFrame()->documentElement().findFirst("body");

    if (insertAtTop) {
        mainElement.prependInside(newItem);
    } else {
        mainElement.appendInside(newItem);
    }

    newsCounter++;
    updateNewsStats();
}




void QwcNewsWidget::on_fBtnRefresh_clicked(bool checked)
{
    Q_UNUSED(checked);
    resetNewsView();
    newsCounter = 0;
    emit requestedRefresh();
    pageWidget->setCurrentIndex(1);
}


/*! Show the composer/post window.
*/
void QwcNewsWidget::on_fBtnPost_clicked()
{
    pageWidget->setCurrentIndex(2);
    composeMessage->setFocus();
}


void QwcNewsWidget::on_fBtnDelete_clicked(bool)
{
    if (QMessageBox::question(this, tr("Clear news"),
                              tr("Are you sure you want to clear all news items?\n\nThis cannot be undone."),
                              QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes) {
        newsCounter = 0;
        emit userPurgedNews();
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
    if (!composeMessage->toPlainText().isEmpty()) {
        emit doPostNews(composeMessage->toPlainText());
//        QDateTime currentDate = QDateTime::currentDateTime();
//        addNewsItem(tr("[sent to server]"), currentDate, composeMessage->toPlainText(), true);
    }
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


/*! Enable/Disable GUI elements based on the user privileges.
*/
void QwcNewsWidget::setupFromUser(const QwcUserInfo user)
{
    fBtnPost->setVisible(user.privPostNews);
    fBtnDelete->setVisible(user.privClearNews);
}


/*! Clear the contents of the news view and set an empty HTML page with styles.
*/
void QwcNewsWidget::resetNewsView()
{
    newsView->page()->mainFrame()->
            setHtml(QString("<html><head>"
                    "<style rel=stylesheet ref=stylesheet>"
                    "body { font-size: %1px; background-color: %5; }"
                    ".news_item { display: block; font-family: \"%2\"; padding-bottom: 12px; }"
                    ".news_header { color: %3; display: block; }"
                    ".news_body { color: %4; }"
                    "</style>"
                    "</head><body></body></html>")
                    .arg(newsFont.pointSize())
                    .arg(newsFont.family())
                    .arg(pColorTitle.name())
                    .arg(pColorText.name())
                    .arg(QwcSingleton::colorFromPrefs("interface/news/back/color", Qt::white).name()));
}
