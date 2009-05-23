#ifndef QWCFILESEARCHWIDGET_H
#define QWCFILESEARCHWIDGET_H

#include "ui_QwcFileSearchWidget.h"
#include "QwcFileInfo.h"

class QwcFileSearchWidget : public QWidget, private Ui::QwcFileSearchWidget
{
    Q_OBJECT

public:
    QwcFileSearchWidget(QWidget *parent = 0);
    ~QwcFileSearchWidget();

signals:
    void search(QString theSearch);
    void downloadFile(QString theFile);
    void revealFile(QString thePath);

private slots:
    void on_fBtnDownload_clicked(bool);
    void on_fSearch_returnPressed();
    void on_fResults_currentRowChanged(int row);
    void on_fBtnReveal_clicked(bool);

public slots:
    void updateResults(QList<QwcFileInfo> theResults);
};

#endif
