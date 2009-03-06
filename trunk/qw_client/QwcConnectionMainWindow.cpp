#include "QwcConnectionMainWindow.h"


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
