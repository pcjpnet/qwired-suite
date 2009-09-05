#ifndef QWCPLUGININTERFACE_H
#define QWCPLUGININTERFACE_H

#include <QtPlugin>
#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtGui/QAction>

class QwcPluginInterface
{
public:
    /*! The destructor of the plugin. */
    virtual ~QwcPluginInterface() { }


    /*! \brief Return information about the plugin during initialization.
        The function should return a QHash with the following keys:
          - \e PLUGIN_NAME - The human readable name of the plugin (e.g. "My Plugin").
          - \e PLUGIN_VERSION - The version number of the plugin (e.g. "1.0.0").
          - \e PLUGIN_DESCRIPTION - A short description of the plugin.
          - \e PLUGIN_AUTHOR - The name of the author of the plugin.
          - \e PLUGIN_AUTHOR_WEB - The URL to the author's website.
          - \e PLUGIN_AUTHOR_EMAIL - The eMail-address of the author of the plugin.
    */
    virtual QHash<QString,QString> pluginInformation() const = 0;


    /*! This function must be re-implemented by the plugin and is called right after the plugin has
        been loaded. */
    virtual void initializePlugin() = 0;

    /*! \brief Get a list of plugin-relevant toolbar items for the main connection widget.
        Return a list of pointers to QAction objects that should be added to the connection widget.
        Returned actions should be /e disabled when they are returned here, and be enabled as soon
        a connection has been established.
    */
//    virtual QList<QAction*> getSessionWidgetToolbarActions() { return QList<QAction*>(); }

};

Q_DECLARE_INTERFACE(QwcPluginInterface, "de.neo.qwired.PluginInterface/1.0");

#endif


