#include "QwcColorButton.h"


/*! \class QwcColorButton
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This is a QToolButton derivate that provides a color selection button with color preview in
    the middle of the button itself.
*/

QwcColorButton::QwcColorButton(QWidget *parent) : QToolButton(parent)
{
    connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
    updateIcon();
}


/*! Initialize the button with the color \a color.
*/
QwcColorButton::QwcColorButton(QColor color, QWidget *parent) : QToolButton(parent)
{
    Q_UNUSED(parent);
    connect(this, SIGNAL(clicked(bool)), this, SLOT(requestColor()) );
    this->pColor = color;
    updateIcon();
}


/*! Returns the currently selected color.
*/
QColor QwcColorButton::selectedColor()
{
    return pColor;
}


/*! Updates the button's icon by generating a new one from the selected color.
*/
void QwcColorButton::updateIcon()
{
    QPainter p;
    QPixmap pm(18,18);
    p.begin(&pm);
    p.fillRect(0,0,18,18,pColor);
    p.end();
    this->setIcon(pm);
}


/*! Shows a color selection dialog and promts the user to select a new color.
*/
void QwcColorButton::requestColor()
{
    QColor tmpC = QColorDialog::getColor(pColor);
    pColor = tmpC;
    updateIcon();
    emit colorSelected(pColor);
}


/*! Set the current color of the button and update the icon. Do not emit a \a colorSelected() signal.
*/
void QwcColorButton::setColor(QColor theColor)
{
    pColor = theColor;
    updateIcon();
}
