#include "QwcAboutWidget.h"

#include <QDesktopServices>
#include <QUrl>

/*! \class QwcAboutWidget
    \author Bastian Bense <bb@bense.de>
    \date 2009-03-06

    This is the about window/widget of the Qwired application and displays some information about
    its version, authors and other things.
*/

QwcAboutWidget::QwcAboutWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

void QwcAboutWidget::on_fBtnDonate_clicked()
{
    QDesktopServices::openUrl(QUrl("http://qwired.neo.de/donate/"));
}

void QwcAboutWidget::on_fBtnGoToWebsite_clicked()
{
    QDesktopServices::openUrl(QUrl("http://qwired.neo.de/"));
}
