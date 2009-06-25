#ifndef QWCFILEBROWSERWIDGET_H
#define QWCFILEBROWSERWIDGET_H

#include "ui_QwcFileBrowserWidget.h"

#include "QwcTransferInfo.h"
#include "QwcFileInfo.h"

#include <QPointer>
#include <QSortFilterProxyModel>

class QwcSession;

/*! \class QwcFileBrowserWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This provides a widget for browsing files on a remote server. It emits signal for certain actions
    such as download requests, get info requests and so on.
*/

class QwcFileBrowserWidget : public QWidget, private Ui_QwcFileBrowserWidget
{
    Q_OBJECT

public:
    QwcFileBrowserWidget(QWidget *parent = 0);

    /*! The path to the current directory on the remote server. */
    QString remotePath;

    void resetForListing();
    void setFileInformation(QwcFileInfo file);

//    void dragEnterEvent(QDragEnterEvent *event);
//    void dropEvent(QDropEvent *event);

signals:
    /*! This signal is emitted when the user wants to refresh the contents of the current directory. */
    void requestedRefresh(QString path);
    /*! This signal is emitted when the user reqested information about a path. */
    void requestedInformation(QString path);
    /*! This signal is emitted when a path needs to be deleted. */
    void requestedDelete(QString path);
    /*! This signal is emitted when a new folder should be created. */
    void requestedNewFolder(QString path);

private slots:
    void on_btnBack_clicked();
    void on_btnInfo_clicked();
    void on_btnNewFolder_clicked();
    void on_btnRefresh_clicked();
    void on_btnDelete_clicked();
    void on_fList_itemSelectionChanged();
    void on_fList_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void fileTransferDone(QwcTransferInfo);

    // Info page
    void on_btnInfoCancel_clicked();


private:
    /*! This member is true if the browser is expecting more list items. */
    bool waitingForListItems;
    /*! This contains the amount of free space on the remote server. */
    qlonglong freeRemoteSpace;
    /*! This contains the total amount of data for all listed items. */
    qlonglong totalUsedSpace;

    void downloadFile(QString path);

public slots:
    void handleFilesListItem(QwcFileInfo item);
    void handleFilesListDone(QString path, qlonglong freeSpace);

    void on_fBtnDownload_clicked(bool checked);

    void on_fBtnUpload_clicked(bool checked);
    void on_fFilter_textEdited(QString);


};

#endif
