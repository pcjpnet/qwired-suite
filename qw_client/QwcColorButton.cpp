#include "QwcColorButton.h"

QwcColorButton::QwcColorButton(QWidget *parent) : QToolButton(parent)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
    updateIcon();
}


QwcColorButton::QwcColorButton(QColor theColor, QWidget *parent) : QToolButton(parent)
{
    Q_UNUSED(parent);
    connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
    pColor = theColor;
    updateIcon();
}


QColor QwcColorButton::selectedColor()
{
    return pColor;
}


void QwcColorButton::updateIcon()
{
    QPainter p;
    QPixmap pm(18,18);
    p.begin(&pm);
    p.fillRect(0,0,18,18,pColor);
    p.end();
    this->setIcon(pm);
}


void QwcColorButton::requestColor()
{
    QColor tmpC = QColorDialog::getColor(pColor);
    pColor = tmpC;
    updateIcon();
    emit colorSelected(pColor);
}


void QwcColorButton::setColor(QColor theColor)
{
    pColor = theColor;
    updateIcon();
}
