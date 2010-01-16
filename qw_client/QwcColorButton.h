#ifndef QWCCOLORBUTTON_H
#define QWCCOLORBUTTON_H


#include <QToolButton>
#include <QPainter>
#include <QColorDialog>


class QwcColorButton :
        public QToolButton
{
    Q_OBJECT
		
public:
    QwcColorButton(QWidget *parent = 0);
    QwcColorButton(QColor theColor, QWidget *parent = 0);
    QColor selectedColor();
	
private:
    QColor pColor;
    void updateIcon();

private slots:
    void requestColor();
	
public slots:
    void setColor(QColor theColor);

signals:
    void colorSelected(QColor);

};

#endif
