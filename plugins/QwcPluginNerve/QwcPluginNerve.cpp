#include "QwcPluginNerve.h"


#include "QwcGlobals.h"

#include <QtDebug>

QwcPluginNerve::QwcPluginNerve()
{

}


QHash<QString,QString> QwcPluginNerve::pluginInformation() const
{
    QHash<QString,QString> info;
    info["PLUGIN_NAME"] = "Nerve Plugin";
    info["PLUGIN_VERSION"] = "1.0.0";
    info["PLUGIN_DESCRIPTION"] = "Customer management, wiki and ticket system.";
    info["PLUGIN_AUTHOR"] = "Bastian Bense";
    info["PLUGIN_AUTHOR_WEB"] = "http://bense.de/centralpark/";
    info["PLUGIN_AUTHOR_EMAIL"] = "bb@bense.de";
    return info;
}



//QList<QAction*> QwcPluginNerve::getSessionWidgetToolbarActions()
//{
//    QAction *action = new QAction(this);
//    action->setText(tr("Nerve"));
//    action->setIcon(QIcon(":/resources/nerve_toolbar_icon.png"));
//    action->setEnabled(false);
//    return QList<QAction*>() << action;
//}


void QwcPluginNerve::initializePlugin()
{
    qDebug() << this << "Initialized plugin!!!!";
    QwcSingleton *singleton = &WSINGLETON::Instance();
//    qDebug() << "FONT" << singleton->systemMonospaceFont();


}

Q_EXPORT_PLUGIN2(QwcPluginNerve, QwcPluginNerve);
