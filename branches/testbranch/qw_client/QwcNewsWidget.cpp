#include "QwcGlobals.h"
#include "QwcNewsWidget.h"
#include "QwFile.h"

#include <QMessageBox>
#include <QDebug>
#include <QTextCursor>
#include <QTextBlock>

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
    connect(tmpS, SIGNAL(prefsChanged()),
            this, SLOT(reloadPreferences()));


    pageWidget->setCurrentIndex(1); // jump to the waiting panel
}


QwcNewsWidget::~QwcNewsWidget()
{
}


/*! The preferences were changed and we should reload the news.
*/
void QwcNewsWidget::reloadPreferences()
{
//    fNews->setPlainText("");
    initPrefs();

    fNews->clear();
    newsCounter = 0;
    updateNewsStats();
    pageWidget->setCurrentIndex(1);
    emit requestedRefresh();
}


void QwcNewsWidget::initPrefs()
{
    QPalette inputPalette = fNews->palette();
    inputPalette.setColor(QPalette::Base, QwcSingleton::colorFromPrefs("interface/news/back/color", Qt::white));
    fNews->setPalette(inputPalette);
    composeMessage->setPalette(inputPalette);

    pColorText =  QwcSingleton::colorFromPrefs("interface/news/text/color", Qt::black);
    pColorTitle = QwcSingleton::colorFromPrefs("interface/news/titles/color", Qt::gray);

    // Set the default font for the news reader
    QSettings settings;
    newsFont = QFont(settings.value("interface/news/font", QwcSingleton::systemMonospaceFont()).toString().section(",",0,0),
                   settings.value("interface/news/font", QwcSingleton::systemMonospaceFont()).toString().section(",",1,1).toInt());

    composeMessage->document()->setDefaultFont(newsFont);
//    fNews->document()->setDefaultFont(newsFont);
}


/*! Update the news statistics bar in the bottom of the widget.
*/
void QwcNewsWidget::updateNewsStats()
{
    fNewsStatus->setText(tr("%1 news article(s)")
                         .arg(newsCounter));
    fBtnDelete->setEnabled(newsCounter > 0);
}


void QwcNewsWidget::addNewsItem(QString theNick, QDateTime &time, QString thePost, bool insertAtTop)
{
    QListWidgetItem *itemHeader = new QListWidgetItem;
    QFont headerFont(newsFont);
    headerFont.setBold(true);
    itemHeader->setFont(headerFont);
    itemHeader->setBackground(Qt::lightGray);
    itemHeader->setTextColor(pColorTitle);
    itemHeader->setText(tr("From %1 (%2)").arg(theNick).arg(time.toString()));

    QListWidgetItem *item = new QListWidgetItem;
    item->setFont(newsFont);
    item->setText(thePost);
    item->setTextColor(pColorText);

    if (insertAtTop) {
        fNews->insertItem(0, itemHeader);
        fNews->insertItem(1, item);
    } else {
        fNews->addItem(itemHeader);
        fNews->addItem(item);
    }

    newsCounter++;
    updateNewsStats();


    /*

    QTextBlock firstBlock = fNews->document()->firstBlock();
    QTextCursor cursor(firstBlock);
//    cursor.movePosition(QTextCursor::Start);
//    cursor.movePosition(QTextCursor::PreviousBlock);
    //cursor.setPosition(-1);
    cursor.insertBlock();
cursor.insertText(thePost);
return;


    QTextCharFormat charFormat;
    charFormat.setFont(newsFont);
    charFormat.setFontUnderline(true);

    cursor.insertText(tr("From %1 (%2):\n").arg(theNick).arg(time.toString()),
                      charFormat);

    charFormat.setFontUnderline(false);
    cursor.insertText(thePost, charFormat);

    cursor.insertBlock();
    return;


//    fNews->setFo(newsFont);
    fNews->moveCursor(QTextCursor::Start);

    fNews->setFontWeight(QFont::Bold);
    fNews->setTextColor(pColorTitle);
//    fNews->insertPlainText(tr("From %1 (%2):\n").arg(theNick).arg(time.toString()));
    fNews->setFontWeight(QFont::Normal);
    fNews->setTextColor(pColorText);
    fNews->insertPlainText(tr("%1\n\n").arg(thePost));

    */

}




void QwcNewsWidget::on_fBtnRefresh_clicked(bool checked)
{
    Q_UNUSED(checked);
    fNews->clear();
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
        QDateTime currentDate = QDateTime::currentDateTime();
        addNewsItem(tr("[sent to server]"), currentDate, composeMessage->toPlainText(), true);
    }
    pageWidget->setCurrentIndex(0);
    composeMessage->setPlainText("");
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
