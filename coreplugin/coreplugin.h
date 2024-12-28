#pragma once

#include "iplugin.h"
#include "pluginsystem_global.h"
#include "mainwindow.h"
#include "commonmode.h"

class CorePlugin : public IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deptrum.core" FILE "core.json")

public:
    CorePlugin();
    ~CorePlugin() override;

    bool initialize(const QStringList & arguments, QString * errorString) override;
    void extensionsInitialized() override;
    bool delayedInitialize() override;
    ShutdownFlag aboutToShutdown() override;

    MainWindow *m_mainWindow = nullptr;
    CommonMode *m_commonMode = nullptr;
};
