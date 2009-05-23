#ifndef QWCCONNECTIONMAINWINDOW_H
#define QWCCONNECTIONMAINWINDOW_H

#include <QPointer>
#include <QMainWindow>

#include "ui_QwcConnectionMainWindow.h"
#include "QwcAboutWidget.h"

class QwcConnectionMainWindow : public QMainWindow, public Ui_QwcConnectionMainWindow
{

Q_OBJECT

public:
    QPointer<QwcAboutWidget> pQwcAboutWidget;
    QwcConnectionMainWindow(QWidget *parent=0);
	
public slots:
    void toggleVisible();
	
private slots:
    void on_actionAbout_triggered(bool);
};

#endif
