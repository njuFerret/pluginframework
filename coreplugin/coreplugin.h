#pragma once

#include "commonmode.h"
#include "iplugin.h"
#include "mainwindow.h"
#include "pluginsystem_global.h"

class CorePlugin : public IPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "com.deptrum.core" FILE "core.json")

public:
  CorePlugin();
  ~CorePlugin() override;

  bool initialize(const QStringList &arguments, QString *errorString) override;
  void extensionsInitialized() override;
  bool delayedInitialize() override;
  ShutdownFlag aboutToShutdown() override;

  QScopedPointer<MainWindow> m_mainWindow;
  QScopedPointer<CommonMode> m_commonMode;
};
