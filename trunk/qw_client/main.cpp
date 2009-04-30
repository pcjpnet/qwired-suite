#include <QApplication>
#include <QTranslator>

#include "QwcSession.h"
#include "QwcGlobals.h"
#include <QNetworkProxy>

int main (int argc, char *argv[])
{

    QApplication app(argc, argv);

    // Set the plugins directory correctly.
    // This has to be done for bundles in OS X if they are packed using the deployqt tool.
//#ifdef Q_WS_MAC
//    qDebug() << "Running in mac mode.";
//    QDir dir(QApplication::applicationDirPath());
//    dir.cdUp();
//    dir.cd("Plugins");
//    QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
//#endif

    QCoreApplication::setOrganizationName("NeoSoftware");
    QCoreApplication::setOrganizationDomain("neo.de");
    QCoreApplication::setApplicationName("Qwired");

    QSettings tmpPrefs;

    // Translation loader
    QTranslator translator;
    QString tmpLocaleShort = QLocale::system().name().section ( "_",0,0 );
    QStringList tmpLocaleList;
    if (tmpPrefs.contains("general/language") && tmpPrefs.value("general/language").toString()!="_auto_" ) {
        tmpLocaleList.append(QString(":lang_%1.qm").arg(tmpPrefs.value("general/language").toString()));
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
    if (tmpPrefs.value("proxy/type").toInt() != 0) {
        QNetworkProxy proxy((QNetworkProxy::ProxyType)tmpPrefs.value("proxy/type").toInt());
        proxy.setHostName(tmpPrefs.value("proxy/host").toString());
        proxy.setPort(tmpPrefs.value("proxy/port").toInt());
        proxy.setUser(tmpPrefs.value("proxy/username").toString());
        proxy.setPassword(tmpPrefs.value("proxy/password").toString());
        QNetworkProxy::setApplicationProxy(proxy);
        qDebug() << "Configured network proxy.";
    }

    // Create the inital connection
    QwcSession *tmpConn = new QwcSession();

    // Singleton
    QObject::connect(&app, SIGNAL(aboutToQuit()), &WSINGLETON::Instance(), SLOT(cleanUp()));
    WSINGLETON::Instance().addSession(tmpConn);
    WSINGLETON::Instance().createTrayIcon();

    return app.exec();
}

