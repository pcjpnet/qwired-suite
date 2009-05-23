#include "QwcFileSearchWidget.h"
#include <QSettings>
#include <QDir>
#include <QMessageBox>
#include <QPushButton>

QwcFileSearchWidget::QwcFileSearchWidget(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

}


QwcFileSearchWidget::~QwcFileSearchWidget()
{
}

void QwcFileSearchWidget::updateResults(QList< QwcFileInfo > theResults) {
    QListIterator<QwcFileInfo> i(theResults);
    while(i.hasNext()) {
        QwcFileInfo tmpRes = i.next();
        QListWidgetItem *item = new QListWidgetItem(fResults);
        item->setText(tmpRes.fileName());
        item->setIcon( QIcon(":/icons/icn_file_18.png") );
        item->setData(Qt::UserRole, tmpRes.path);
        item->setData(Qt::UserRole+1, tmpRes.type);
    }

}

void QwcFileSearchWidget::on_fSearch_returnPressed() {
    QString tmpSearch = fSearch->text();
    if(!tmpSearch.isEmpty()) {
        fResults->clear();
        emit search(tmpSearch);
    }
}

void QwcFileSearchWidget::on_fBtnDownload_clicked(bool)
{
    QList<QListWidgetItem*> items = fResults->selectedItems();
    QListIterator<QListWidgetItem*> i(items);
    while(i.hasNext()) {
        QListWidgetItem *item = i.next();
        if(item->data(Qt::UserRole+1).toInt()==0) {
            // Only files! :)
            QString tmpPath = item->data(Qt::UserRole).toString();

            QSettings settings;
            QString tmpName = tmpPath.section("/",-1,-1);

            QDir tmpDownloadFolder( settings.value("files/download_dir", QDir::homePath()).toString() );
            QFile tmpDir( tmpDownloadFolder.absoluteFilePath(tmpName) );
            if(tmpDir.exists()) {
                QMessageBox messageBox(this);
                messageBox.setWindowTitle(tr("File Exists"));
                messageBox.setIcon(QMessageBox::Warning);
                messageBox.setText(tr("The file you are trying to download already exists in your download directory. Overwrite it?") );
                QAbstractButton *rejectButton = messageBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
                Q_UNUSED(rejectButton)
                        QAbstractButton *deleteButton = messageBox.addButton(tr("Overwrite"), QMessageBox::AcceptRole);
                messageBox.exec();
                if (messageBox.clickedButton()!=deleteButton)
                    return;
            }
            emit downloadFile( item->data(Qt::UserRole).toString() );

        }



    }
}

void QwcFileSearchWidget::on_fResults_currentRowChanged(int row) {
    fBtnDownload->setEnabled( row!=-1);
    fBtnReveal->setEnabled( row!=-1);
}

void QwcFileSearchWidget::on_fBtnReveal_clicked(bool ) {
    QList<QListWidgetItem*> items = fResults->selectedItems();
    if(items.count()) {
        QListWidgetItem *item = items[0];
        emit revealFile( item->data(Qt::UserRole).toString() );
    }
}



