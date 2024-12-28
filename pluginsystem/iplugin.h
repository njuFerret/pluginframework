#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "pluginsystem_global.h"
#include <QObject>

class PluginSpec;
class IPluginPrivate;

class PLUGINSYSTEM_EXPORT IPlugin : public QObject
{
    Q_OBJECT

public:
    enum ShutdownFlag { SynchronousShutdown, AsynchronousShutdown };
    IPlugin() {}
    ~IPlugin() {}
    virtual bool initialize(const QStringList &arguments, QString *errorString) = 0;
    virtual void extensionsInitialized() {}
    virtual bool delayedInitialize() { return false; }
    virtual ShutdownFlag aboutToShutdown() { return SynchronousShutdown; }

    PluginSpec *pluginSpec;

signals:
    void asynchronousShutdownFinished();
};

#endif // IPLUGIN_H
