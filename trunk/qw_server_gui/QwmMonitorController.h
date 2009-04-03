#ifndef QWMMONITORCONTROLLER_H
#define QWMMONITORCONTROLLER_H

#include "QwmMonitorWindow.h"
#include "QwmConsoleSocket.h"

#include <QObject>
#include <QPointer>
#include <QProcess>


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
    void handleCommandTRANSFERS(QList<QwTransferInfo> transfers);
    void handleCommandUSERS(QList<QwUser> users);
    void handleLogMessage(const QString logMessage);

    // Daemon handling
    void handleDaemonStarted();
    void handleDaemonFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleDaemonError(QProcess::ProcessError error);
    void handleDaemonReadyReadStdout();

public:
    QwmMonitorController(QObject *parent=0);

public slots:
    void startMonitor();
//    void connectToConsole();
    void startDaemonProcess();
    void stopDaemonProcess();

};

#endif // QWMMONITORCONTROLLER_H
