#include "QwcSession.h"
#include "QwcGlobals.h"

#include <QtCore/QTranslator>
#include <QtCore/QPluginLoader>
#include <QtNetwork/QNetworkProxy>
#include <QtGui/QApplication>

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Neo Software");
    QCoreApplication::setOrganizationDomain("neo.de");
    QCoreApplication::setApplicationName("Qwired");

    QSettings settings;

#ifdef Q_WS_MAC
    if (!app.style()->objectName().startsWith("macintosh")) {
        app.setStyleSheet("* {font-size: 11px;} QWidget { background-color: #DDD; }");
    }
#endif

    // Basic first-time initialization
    if (settings.allKeys().isEmpty()) {
        // The main bookmark
        settings.beginWriteArray("bookmarks", 1);
        settings.setArrayIndex(0);
        settings.setValue("name", "Official Qwired Server");
        settings.setValue("address", "hl.neo.de");
        settings.endArray();
    }

    // Translation loader
    QTranslator translator;
    QString tmpLocaleShort = QLocale::system().name().section ( "_",0,0 );
    QStringList tmpLocaleList;
    if (settings.contains("general/language") && settings.value("general/language").toString()!="_auto_" ) {
        tmpLocaleList.append(QString(":lang_%1.qm").arg(settings.value("general/language").toString()));
    } else {
        tmpLocaleList.append(QLocale::system().name()); // long locale, from file
        tmpLocaleList.append(tmpLocaleShort); // short locale, from file
        tmpLocaleList.append(QString(":%1.qm").arg(QLocale::system().name())); // long locale, from res
        tmpLocaleList.append(QString(":lang_%1.qm").arg(tmpLocaleShort) );
    }
    QStringListIterator i(tmpLocaleList);
    while (i.hasNext()) {
        QString tmpName = i.next();
        if(translator.load(tmpName)) {
            app.installTranslator(&translator);
            break;
        } else {
            qDebug() << "Trying to load locale:"<<tmpName<<": Failed (trying next)";
        }
    }


    // Configure the proxying, if needed
    if (settings.value("proxy/type").toInt() != 0) {
        QNetworkProxy proxy((QNetworkProxy::ProxyType)settings.value("proxy/type").toInt());
        proxy.setHostName(settings.value("proxy/host").toString());
        proxy.setPort(settings.value("proxy/port").toInt());
        proxy.setUser(settings.value("proxy/username").toString());
        proxy.setPassword(settings.value("proxy/password").toString());
        QNetworkProxy::setApplicationProxy(proxy);
    }



    // Singleton
    QwcSingleton *singleton = &WSINGLETON::Instance();
    QObject::connect(&app, SIGNAL(aboutToQuit()),
                     singleton, SLOT(cleanUp()));
    singleton->createTrayIcon();

    // Create the settings directory
    QDir settingDir(QwcSingleton::systemSettingsDirectory());
    if (!settingDir.exists()) { settingDir.mkpath("."); }


    singleton->createInitialSessions();

    return app.exec();
}

