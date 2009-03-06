#ifndef QWCSINGLETON_H
#define QWCSINGLETON_H

#include <QObject>
#include <QPointer>
#include <QColor>
#include <QSettings>
#include <QMenu>
#include <QSystemTrayIcon>

class QwcSession;

class QwcSingleton : public QObject
{
	
Q_OBJECT

public:
    QwcSingleton();
    ~QwcSingleton();
    void addSession(QwcSession *session);
	void createTrayIcon();
    QList<QwcSession*> pSessions;
	QPointer<QMenu> pTrayMenu;
	QPointer<QSystemTrayIcon> pTrayIcon;

	// Global functions
	static QString systemMonospaceFont();
	static QColor colorFromPrefs(QString theKey, QColor theDefault);

private slots:
	void sessionDestroyed(QObject *obj);
	void showTrayMenu(QSystemTrayIcon::ActivationReason);
	
signals:
        void prefsChanged();

public slots:
	void notifyPrefsChanged();
	void cleanUp();
    void makeNewConnection(QString address);
	
};

#endif
