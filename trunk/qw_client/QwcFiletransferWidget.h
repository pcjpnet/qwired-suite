#ifndef QWCFILETRANSFERWIDGET_H
#define QWCFILETRANSFERWIDGET_H

#include "ui_QwcFiletransferWidget.h"
#include "QwcTransferInfo.h"
#include "QwcFiletransferModel.h"

#include <QWidget>


class QwcFiletransferWidget :
        public QWidget,
        protected Ui_QwcFiletransferWidget
{

    Q_OBJECT

public:
    QwcFiletransferWidget(QWidget *parent = 0);
    ~QwcFiletransferWidget();

    /*! Returns a pointer to the currently used data model. */
    QwcFiletransferModel* transferModel() { return dataModel; };

private:
    /*! Contains a pointer to the current data model. */
    QwcFiletransferModel *dataModel;

signals:
    /*! The user decided to pause/stop a transfer. */
    void transferStopped(const QwcTransferInfo &transfer);
    /*! The user decided to resume a transfer. */
    void transferResumed(const QwcTransferInfo &transfer);

private slots:
    void on_btnStop_clicked();
    void on_btnResume_clicked();
    void on_btnReveal_clicked();
    void handleTransferListSelectionChanged(const QItemSelection &previous, const QItemSelection &current);

};

#endif
