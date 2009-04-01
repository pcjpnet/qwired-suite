#ifndef SERVERMONITORCONTROLLER_H
#define SERVERMONITORCONTROLLER_H

#include "ServerMonitorWindow.h"

#include <QObject>
#include <QPointer>
#include "QwmConsoleSocket.h"


class ServerMonitorController : public QObject
{
    Q_OBJECT

private:
    QPointer<QwmConsoleSocket> socket;
    QPointer<ServerMonitorWindow> monitorWindow;

    QString humanReadableSize(qlonglong theBytes);

private slots:
    void handleCommandSTAT(QHash<QString,QString> parameters);
    void handleLogMessage(const QString logMessage);

public:
    ServerMonitorController(QObject *parent=0);

public slots:
    void startMonitor();

};

#endif // SERVERMONITORCONTROLLER_H
