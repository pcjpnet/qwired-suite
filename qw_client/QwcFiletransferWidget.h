#ifndef QWCFILETRANSFERWIDGET_H
#define QWCFILETRANSFERWIDGET_H

#include "ui_QwcFiletransferWidget.h"
#include "QwcTransferInfo.h"
#include <QWidget>


class QwcFiletransferWidget : public QWidget, public Ui_QwcFiletransferWidget
{

    Q_OBJECT

public:
    QwcFiletransferWidget(QWidget *parent = 0);
    ~QwcFiletransferWidget();
    void init();

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
