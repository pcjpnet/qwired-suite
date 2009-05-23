#ifndef QWCFILEBROWSERWIDGET_H
#define QWCFILEBROWSERWIDGET_H

#include "ui_QwcFileBrowserWidget.h"

#include "QwcFilelistModel.h"
#include "QwcTransferInfo.h"

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

    void initWithConnection(QwcSession *theSession);
    void setPath(QString thePath);
    QPointer<QwcFilelistModel> pModel;
    QPointer<QSortFilterProxyModel> pFilterProxy;
    QPointer<QwcSession> pSession;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void fileTransferDone(QwcTransferInfo);


private:
    void downloadFile(QString path);

public slots:
    void on_fBtnDownload_clicked(bool checked);

    void doUpdateBrowserStats(QString thePath, qlonglong theFree);
    void on_fList_doubleClicked(const QModelIndex &index);
    void on_fList_clicked(const QModelIndex &index);
    void on_fBtnBack_clicked(bool checked);

    void on_fBtnUpload_clicked(bool checked);
    void on_fBtnDelete_clicked(bool checked);
    void on_fBtnNewFolder_clicked(bool checked);
    void on_fBtnInfo_clicked(bool checked);
    void on_fFilter_textEdited(QString);
};

#endif
