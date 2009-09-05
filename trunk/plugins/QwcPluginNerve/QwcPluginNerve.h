#ifndef QWCPLUGINNERVE_H
#define QWCPLUGINNERVE_H

#include "QwcPluginInterface.h"


#include <QtCore/QObject>

class QwcPluginNerve : public QObject, public QwcPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QwcPluginInterface)

public:
    QwcPluginNerve();

    QHash<QString,QString> pluginInformation() const;

    void initializePlugin();

};



#endif
