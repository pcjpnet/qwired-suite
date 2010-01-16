#ifndef QWCSINGLETON_H
#define QWCSINGLETON_H

#include "QwcPluginInterface.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QSettings>
#include <QtGui/QColor>
#include <QtGui/QMenu>
#include <QtGui/QSystemTrayIcon>

class QwcSession;

class QwcSingleton :
        public QObject
{
    Q_OBJECT

public:
    QwcSingleton(QObject *parent = 0);
    ~QwcSingleton();
    void addSession(QwcSession *session);
    void createTrayIcon();

    /*! A list of all active/open sessions in this application. */
    QList<QwcSession*> sessions;
    QPointer<QMenu> pTrayMenu;
    QPointer<QSystemTrayIcon> pTrayIcon;

    QStringList messageStyles() const;


    // Global functions
    static QString systemMonospaceFont();
    static QColor colorFromPrefs(QString theKey, QColor theDefault);
    static QString systemSettingsDirectory();

private slots:
    void sessionDestroyed(QObject *obj);
    void showTrayMenu(QSystemTrayIcon::ActivationReason);

signals:
    /*! This signal is emitted whenever the preferences/settings have changed, and parts of the
        application needs to reload them. */
    void applicationSettingsChanged();

public slots:
    void createInitialSessions();
    void emitApplicationSettingsChanged();
    void cleanUp();

};

#endif
