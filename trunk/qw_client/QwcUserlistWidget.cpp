#include "QwcUserlistWidget.h"

#include <QMouseEvent>
#include <QtDebug>
#include <QApplication>

QwcUserlistWidget::QwcUserlistWidget(QWidget *parent) : QListView(parent)
{
}


QwcUserlistWidget::~QwcUserlistWidget()
{
}

void QwcUserlistWidget::mouseMoveEvent(QMouseEvent * event)
{
	Q_UNUSED(event)
			
	// Handle Mouse Drag+Drop
	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;
	QImage tmpVar =  this->model()->data(this->currentIndex(), Qt::DecorationRole).value<QImage>();
	if(!tmpVar.isNull())
		drag->setPixmap( QPixmap::fromImage(tmpVar) );
	mimeData->setData("application/x-userid", QByteArray("Hello world"));
	drag->setMimeData(mimeData);
	Qt::DropAction dropAction = drag->start();
	Q_UNUSED(dropAction)
}

void QwcUserlistWidget::dragEnterEvent(QDragEnterEvent * event)
{
	Q_UNUSED(event)
			
	// Handle drag+drop hovering
	const QMimeData *md = event->mimeData();
	if( md->hasFormat("application/x-userid") ) {
		qDebug() << md->data("application/x-userid");
		event->acceptProposedAction();
		
	}
}

void QwcUserlistWidget::dropEvent(QDropEvent * event)
{
	Q_UNUSED(event)
	// Handle drag+drop
	//qDebug() << "Dropped";
}
