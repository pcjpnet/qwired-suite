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
    QProcess daemonProcess;

private slots:
    void handleCommandCompleted(QString command);
    void handleCommandSTAT(QHash<QString,QString> parameters);
    void handleCommandTRANSFERS(QList<QwTransferInfo> transfers);
    void handleCommandUSERS(QList<QwUser> users);
    void handleCommandVERSION(QString version);
    void handleCommandCONFIG_READ(QString configName, QByteArray configValue);
    void handleLogMessage(const QString logMessage);

    void handleSelectedNewBanner(QImage);

    // Daemon handling
    void handleDaemonStarted();
    void handleDaemonFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleDaemonError(QProcess::ProcessError error);
    void handleDaemonReadyReadStdout();

    // Status
    void handle_btnRebuildIndex_clicked();
    void handleUpdatedTrackerList(QStringList trackers);



public:
    QwmMonitorController(QObject *parent=0);
    ~QwmMonitorController();

public slots:
    void startMonitor();
//    void connectToConsole();
    void startDaemonProcess();
    void stopDaemonProcess();

};

#endif // QWMMONITORCONTROLLER_H
