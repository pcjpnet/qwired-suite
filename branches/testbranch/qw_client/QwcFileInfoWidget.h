#ifndef WIDGETFILEINFO_H
#define WIDGETFILEINFO_H

#include <QWidget>
#include "ui_QwcFileInfoWidget.h"
#include "QwcFileInfo.h"

class QwcFileInfoWidget : public QWidget, public Ui_QwcFileInfoWidget
{
	Q_OBJECT
		
public:
    QwcFileInfo pFile;
	
    QwcFileInfoWidget(QWidget *parent = 0)
	{
		Q_UNUSED(parent)
		setAttribute(Qt::WA_DeleteOnClose);
		setupUi(this);
	};
	
    void loadFromFile(const QwcFileInfo theFile)
	{
		pFile = theFile;
		fName->setText(theFile.fileName());
		fIcon->setPixmap(theFile.fileIcon().pixmap(16,16));
        fSize->setText(QString("%1 (%2 bytes)").arg(QwcFileInfo::humanReadableSize(theFile.size)).arg(theFile.size));
		fPath->setText(theFile.path);
		fModified->setText(theFile.modified.toString());
		fCreated->setText(theFile.created.toString());
		fChecksum->setText(theFile.checksum);
		fComments->setText(theFile.comment);
	};
	
};

#endif
