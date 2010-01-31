#ifndef QWCFILETRANSFERWIDGET_H
#define QWCFILETRANSFERWIDGET_H

#include "ui_QwcFiletransferWidget.h"
#include "QwcTransferInfo.h"

#include <QWidget>


class QwcFiletransferWidget :
        public QWidget,
        protected Ui_QwcFiletransferWidget
{
    Q_OBJECT

public:
    QwcFiletransferWidget(QWidget *parent = 0);
    ~QwcFiletransferWidget();


private slots:
    void on_btnStop_clicked();
    void on_btnResume_clicked();
    void on_btnReveal_clicked();
    void handleTransferListSelectionChanged(const QItemSelection &previous, const QItemSelection &current);

};

#endif
