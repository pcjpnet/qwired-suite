#ifndef QWCFILEBROWSERWIDGET_H
#define QWCFILEBROWSERWIDGET_H

#include "ui_QwcFileBrowserWidget.h"

#include "QwcTransferInfo.h"
#include "QwcFileInfo.h"
#include "QwcUserInfo.h"

#include <QtCore/QPointer>
#include <QtGui/QSortFilterProxyModel>

class QStandardItemModel;
class QwcSocket;
class QwcTransfer;

class QwcFileBrowserWidget :
        public QWidget,
        protected Ui_QwcFileBrowserWidget
{
    Q_OBJECT

public:
    QwcFileBrowserWidget(QWidget *parent = 0);

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

    QString remotePath() const;
    void setRemotePath(const QString &path);

    /*! Information about the current directory/folder. */
    QwcFileInfo m_currentFolderInfo;
    /*! Information about the currently selected file/folder in the list (for get info) */
    QwcFileInfo m_currentFileInfo;

    void resetForListing();
    void setFileInformation(const QwFile &file);


private slots:
    void on_btnBack_clicked();
    void on_btnInfo_clicked();
    void on_btnNewFolder_clicked();
    void on_btnRefresh_clicked();
    void on_btnDelete_clicked();
    void on_btnDownload_clicked();
    void on_btnUpload_clicked();
    void on_findFilter_returnPressed();
    void on_fList_doubleClicked(const QModelIndex &index);
    void handleListSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void handleTransferListSelectionChanged(const QItemSelection &selected,
                                            const QItemSelection &deselected);
    void on_pauseTransfer_clicked();
    void on_resumeTransfer_clicked();
    void on_cancelTransfer_clicked();

    void handleSocketTransferCreated(QwcTransfer *transfer);
    void handleSocketTransferRemoved(QwcTransfer *transfer);
    void handleSocketTransferChanged(QwcTransfer *transfer);

    // Info page
    void on_btnInfoCancel_clicked();
    void on_btnInfoApply_clicked();

    void handlePrivilegesChanged();


protected:
    /*! The model for the file list view. */
    QStandardItemModel *m_fileListModel;
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
    /*! The model for the file transfer view. */
    QStandardItemModel *m_fileTransferModel;

public slots:
    void handleFilesListItem(QwcFileInfo item);
    void handleFilesListDone(const QString &path, qint64 freeSpace);
    void handleSearchResultListDone();

};

#endif
