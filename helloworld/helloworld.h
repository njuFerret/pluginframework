#pragma once

#include "iplugin.h"
#include "pluginsystem_global.h"
#include <QObject>

class QWidget;

class HelloWorldPlugin final : public IPlugin {
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "hello.world" FILE "helloworld.json")

public:
  HelloWorldPlugin();
  ~HelloWorldPlugin() override;

  bool initialize(const QStringList &arguments, QString *errorString) final;
  void extensionsInitialized() override;
  void shutdown();

private:
  QScopedPointer<QWidget> w;
};
