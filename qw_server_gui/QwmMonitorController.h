#ifndef QWMMONITORCONTROLLER_H
#define QWMMONITORCONTROLLER_H

#include "QwmMonitorWindow.h"

#include <QObject>
#include <QPointer>
#include <QProcess>

#include "QwmConsoleSocket.h"


class QwmMonitorController : public QObject
{
    Q_OBJECT

private:
    QPointer<QwmConsoleSocket> socket;
    QPointer<QwmMonitorWindow> monitorWindow;
    QString humanReadableSize(qlonglong theBytes);
    QProcess daemonProcess;

private slots:
    void handleCommandSTAT(QHash<QString,QString> parameters);
    void handleLogMessage(const QString logMessage);

public:
    QwmMonitorController(QObject *parent=0);

public slots:
    void startMonitor();
    void startDaemonProcess();

};

#endif // QWMMONITORCONTROLLER_H
