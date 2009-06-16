#ifndef QWCCONNECTIONMAINWINDOW_H
#define QWCCONNECTIONMAINWINDOW_H

#include <QPointer>
#include <QLabel>
#include <QMainWindow>

#include "ui_QwcConnectionMainWindow.h"
#include "QwcAboutWidget.h"

class QwcConnectionMainWindow : public QMainWindow, public Ui_QwcConnectionMainWindow
{

Q_OBJECT

public:
    QPointer<QwcAboutWidget> pQwcAboutWidget;
    QwcConnectionMainWindow(QWidget *parent=0);

    /*! This member holds a pointer to a QLabel which contains the pixmap for the server banner
        in the toolbar. */
    QPointer<QLabel> bannerToolbarWidget;
    /*! This member holds a pointer to a QWidget which is responsible for spacing the banner, so
        that it appears at the right-hand side of the connection window toolbar. */
    QPointer<QWidget> bannerToolbarWidgetSpacer;
	
public slots:
    void toggleVisible();
	
private slots:
    void on_actionAbout_triggered(bool);
};

#endif
