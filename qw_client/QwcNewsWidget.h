#ifndef QWCNEWSWIDGET_H
#define QWCNEWSWIDGET_H

#include <QWidget>
#include <QPointer>
#include "ui_QwcNewsWidget.h"
#include "QwcNewsPostWidget.h"

class QwcNewsWidget : public QWidget, private Ui_QwcNewsWidget
{
    Q_OBJECT

private:
    QColor pColorTitle;
    QColor pColorText;
    int newsCounter;

public:
    QwcNewsWidget(QWidget *parent=0);
    ~QwcNewsWidget();
    QPointer<QwcNewsPostWidget> pWinPost;
    int newsCount();

signals:
    void doRefreshNews();
    void doPostNews(QString);
    void onDeleteNews();

public slots:
    void addNewsItem(QString theNick, QString theTime, QString thePost);
    void addFreshNewsItem(QString theNick, QString theTime, QString thePost);
    void newsDone();
    void doSendNews();
    void dontSendNews();
    void setDisabledPostButton(bool b);
    void setDisabledClearButton(bool b);
    void clearTextArea();

private slots:
    void on_fBtnRefresh_clicked(bool checked);
    void on_fBtnPost_clicked(bool checked);
    void on_fBtnDelete_clicked(bool checked);
    void reloadPreferences();
    void initPrefs();

};

#endif
