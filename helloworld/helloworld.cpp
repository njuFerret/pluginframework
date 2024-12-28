#include "helloworld.h"
#include <QWidget>

HelloWorldPlugin::HelloWorldPlugin() {}

HelloWorldPlugin::~HelloWorldPlugin() {
  // delete w;
}

bool HelloWorldPlugin::initialize(const QStringList &arguments, QString *errorString) {
  w.reset(new QWidget);
  w->setWindowTitle("尼玛");
  w->show();
  return true;
}

void HelloWorldPlugin::extensionsInitialized() {}

void HelloWorldPlugin::shutdown() {}
