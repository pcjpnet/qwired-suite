#include "QwcConnectionMainWindow.h"


/*! \class QwcConnectionMainWindow
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This is a QMainWindow derivate that provides the default connection window and toolbars. Its
    CentralWidget will be set to be a \a QwsChat instance by default, providing a standard user list.
*/

QwcConnectionMainWindow::QwcConnectionMainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(QIcon(":/icons/qwired_logo_32.png"));
}


void QwcConnectionMainWindow::toggleVisible()
{
    if(this->isMinimized()) {
        this->showNormal();
    } else {
        if(this->isVisible()) this->hide();
        else this->show();
    }
}


void QwcConnectionMainWindow::on_actionAbout_triggered(bool)
{
    if(!pQwcAboutWidget)
        pQwcAboutWidget = new QwcAboutWidget();
    pQwcAboutWidget->show();
}
