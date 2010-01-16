#ifndef QWCNEWSWIDGET_H
#define QWCNEWSWIDGET_H

#include "QwcUserInfo.h"
#include "ui_QwcNewsWidget.h"

#include <QtGui/QWidget>
#include <QtCore/QPointer>


class QwcNewsWidget :
        public QWidget,
        private Ui_QwcNewsWidget
{
    Q_OBJECT

public:
    QwcNewsWidget(QWidget *parent=0);
    ~QwcNewsWidget();

    void setupFromUser(const QwcUserInfo user);

private:
    /*! Stores the current number of news articles. */
    bool m_emoticonsEnabled;
    int m_newsCounter;

    void updateNewsStats();
    void updateNewsCss();

protected:
    bool eventFilter(QObject *what, QEvent *event);

signals:
    void requestedRefresh();
    void doPostNews(QString text);
    /*! This signal is emitted when the user clicked the 'purge news' button and all news should
        be purged from the server database. */
    void userPurgedNews();


public slots:
    void addNewsItem(const QString &nickname, QDateTime time, const QString &text,
                     bool insertAtTop = false);

    void newsDone();

private slots:
    void on_fBtnRefresh_clicked(bool checked);
    void on_fBtnPost_clicked();
    void on_fBtnDelete_clicked(bool checked);
    void on_btnComposeCancel_clicked();
    void on_btnComposePost_clicked();
    void on_newsView_linkClicked(const QUrl &url);
    void reloadPreferences();

};

#endif
