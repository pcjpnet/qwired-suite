#ifndef QWCNEWSWIDGET_H
#define QWCNEWSWIDGET_H

#include "QwcUserInfo.h"
#include "ui_QwcNewsWidget.h"

#include <QtGui/QWidget>
#include <QtCore/QPointer>


class QwcSocket;

class QwcNewsWidget :
        public QWidget,
        protected Ui_QwcNewsWidget
{
    Q_OBJECT

public:
    QwcNewsWidget(QWidget *parent=0);
    ~QwcNewsWidget();

    void setSocket(QwcSocket *socket);
    QwcSocket* socket();

    void setupFromUser(const QwcUserInfo user);

protected:
    QwcSocket *m_socket;

    /*! Stores the current number of news articles. */
    bool m_emoticonsEnabled;
    int m_newsCounter;

    void updateNewsStats();
    void updateNewsCss();

    bool eventFilter(QObject *what, QEvent *event);


public slots:
    void addNewsItem(const QString &nickname, QDateTime time, const QString &text,
                     bool insertAtTop = false);

    void newsDone();

private slots:
    void on_fBtnRefresh_clicked();
    void on_fBtnPost_clicked();
    void on_fBtnDelete_clicked();
    void on_btnComposeCancel_clicked();
    void on_btnComposePost_clicked();
    void on_newsView_linkClicked(const QUrl &url);
    void reloadPreferences();

};

#endif
