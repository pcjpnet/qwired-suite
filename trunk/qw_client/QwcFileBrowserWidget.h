#ifndef QWCFILEBROWSERWIDGET_H
#define QWCFILEBROWSERWIDGET_H

#include "ui_QwcFileBrowserWidget.h"

#include "QwcTransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcUserInfo.h"

#include <QPointer>
#include <QSortFilterProxyModel>

/*! \class QwcFileBrowserWidget
    \author Bastian Bense <bastibense@gmail.com>
    \date 2009-03-06

    This provides a widget for browsing files on a remote server. It emits signal for certain actions
    such as download requests, get info requests and so on.
*/

class QwcSocket;

class QwcFileBrowserWidget :
        public QWidget,
        public Ui_QwcFileBrowserWidget
{
    Q_OBJECT

public:
    QwcFileBrowserWidget(QWidget *parent = 0);

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

    QString remotePath() const;
    void setRemotePath(const QString &path);

    /*! Information about the current directory/folder. */
    QwcFileInfo currentFolderInfo;
    /*! Information about the currently selected file/folder in the list (for get info) */
    QwcFileInfo currentFileInfo;
    /*! A copy of the user info object for the current session. */
    QwcUserInfo userInfo;

    void resetForListing();
    void setFileInformation(QwcFileInfo file);
    void setUserInformation(QwcUserInfo info);

private slots:
    void on_btnBack_clicked();
    void on_btnInfo_clicked();
    void on_btnNewFolder_clicked();
    void on_btnRefresh_clicked();
    void on_btnDelete_clicked();
    void on_btnDownload_clicked();
    void on_btnPreview_clicked();
    void on_btnUpload_clicked();
    void on_findFilter_returnPressed();
    void on_fList_itemSelectionChanged();
    void on_fList_itemDoubleClicked(QTreeWidgetItem *item, int column);

    // Info page
    void on_btnInfoCancel_clicked();
    void on_btnInfoApply_clicked();


private:
    /*! The currently used socket to send data to the server. */
    QwcSocket *m_socket;
    /*! This member is true if the browser is expecting more list items. */
    bool m_waitingForListItems;
    /*! This contains the amount of free space on the remote server. */
    qlonglong m_freeRemoteSpace;
    /*! This contains the total amount of data for all listed items. */
    qlonglong m_totalUsedSpace;
    /*! The path to the current directory on the remote server. */
    QString m_remotePath;

public slots:
    void handleFilesListItem(QwcFileInfo item);
    void handleFilesListDone(QString path, qlonglong freeSpace);
    void handleSearchResultListDone();

};

#endif
