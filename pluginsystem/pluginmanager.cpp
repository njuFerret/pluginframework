#include "pluginmanager.h"
#include "algorithm.h"
#include "pluginspec.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>

Q_GLOBAL_STATIC(PluginManager, pluginManager)

PluginManager *PluginManager::instance() { return pluginManager(); }

PluginManager::~PluginManager() { qDeleteAll(pluginSpecs); }

void PluginManager::addObject(QObject *obj) {
  QWriteLocker lock(&m_lock);
  if (obj == nullptr) {
    qWarning() << "PluginManager::addObject(): trying to add null object";
    return;
  }
  if (allObjects.contains(obj)) {
    qWarning() << "PluginManager::addObject(): trying to add duplicate object";
    return;
  }

  allObjects.append(obj);
}

void PluginManager::removeObject(QObject *obj) {
  if (obj == nullptr) {
    qWarning() << "PluginManager::removeObject(): trying to remove null object";
    return;
  }

  if (!allObjects.contains(obj)) {
    qWarning() << "PluginManager::removeObject(): object not in list:" << obj << obj->objectName();
    return;
  }

  QWriteLocker lock(&m_lock);
  allObjects.removeAll(obj);
}

QObject *PluginManager::getObjectByName(const QString &name) {
  QReadLocker lock(&m_lock);
  return Utils::findOrDefault(allObjects, [&name](const QObject *obj) { return obj->objectName() == name; });
}

void PluginManager::setSettings(QString name) { settings = new QSettings(name, QSettings::IniFormat, this); }

const QStringList pluginFiles(const QString &pluginPath) {
  QStringList searchPaths;
  searchPaths << pluginPath;
  QStringList pluginFileNames;
  while (!searchPaths.isEmpty()) {
    const QDir dir(searchPaths.takeFirst());
    const QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
    const QStringList absoluteFilePaths = Utils::transform(files, &QFileInfo::absoluteFilePath);
    pluginFileNames +=
        Utils::filtered(absoluteFilePaths, [](const QString &path) { return QLibrary::isLibrary(path); });
    const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    searchPaths += Utils::transform(dirs, &QFileInfo::absoluteFilePath);
  }
  for (QString name : pluginFileNames) {
    qDebug() << "pluginFile" << name;
  }

  return pluginFileNames;
}

void PluginManager::setPluginPath(const QString &path) {
  // readSettings
  pluginPath = path;
  if (settings) {
    disabledPlugins = settings->value(QLatin1String("Plugins/Ignored")).toStringList();
    forceEnabledPlugins = settings->value(QLatin1String("Plugins/ForceEnabled")).toStringList();
  }

  // readPluginPaths
  qDeleteAll(pluginSpecs);
  pluginSpecs.clear();

  for (const QString &pluginFile : pluginFiles(pluginPath)) {
    auto *spec = new PluginSpec;
    if (!spec->read(pluginFile)) {
      delete spec;
      continue;
    }

    if (forceEnabledPlugins.contains(spec->name)) {
      spec->enabledBySettings = true;
    }
    if (disabledPlugins.contains(spec->name)) {
      spec->enabledBySettings = false;
    }

    pluginSpecs.append(spec);
  }

  // resolveDependencies
  for (PluginSpec *spec : std::as_const(pluginSpecs)) {
    spec->resolveDependencies(pluginSpecs);
  }

  // enableDependenciesIndirectly
  for (PluginSpec *spec : std::as_const(pluginSpecs)) {
    spec->enabledIndirectly = false;
  }
  // cannot use reverse loadQueue here, because test dependencies can introduce circles
  QList<PluginSpec *> queue = Utils::filtered(pluginSpecs, &PluginSpec::isEffectivelyEnabled);
  while (!queue.isEmpty()) {
    PluginSpec *spec = queue.takeFirst();
    queue += spec->enableDependenciesIndirectly();
  }

  // ensure deterministic plugin load order by sorting
  Utils::sort(pluginSpecs, &PluginSpec::name);
}

bool PluginManager::loadQueue(PluginSpec *spec, QList<PluginSpec *> &queue,
                              QList<PluginSpec *> &circularityCheckQueue) {
  if (queue.contains(spec)) {
    return true;
  }
  // check for circular dependencies
  if (circularityCheckQueue.contains(spec)) {
    spec->hasError = true;
    qDebug() << "Circular dependency detected:\n";

    int index = circularityCheckQueue.indexOf(spec);
    for (int i = index; i < circularityCheckQueue.size(); ++i) {
      qDebug() << QString("%1 (%2) depends on\n")
                      .arg(circularityCheckQueue.at(i)->name)
                      .arg(circularityCheckQueue.at(i)->version);
    }
    qDebug() << QString("%1 (%2)").arg(spec->name).arg(spec->version);
    return false;
  }
  circularityCheckQueue.append(spec);
  // check if we have the dependencies
  if (spec->state == PluginSpec::Invalid || spec->state == PluginSpec::Read) {
    queue.append(spec);
    return false;
  }

  // add dependencies
  const QHash<PluginDependency, PluginSpec *> deps = spec->dependencySpecs;
  for (auto it = deps.cbegin(), end = deps.cend(); it != end; ++it) {
    PluginSpec *depSpec = it.value();
    if (!loadQueue(depSpec, queue, circularityCheckQueue)) {
      spec->hasError = true;
      qDebug() << QString("Cannot load plugin because dependency failed to load: %1 (%2)\n")
                      .arg(depSpec->name)
                      .arg(depSpec->version);
      return false;
    }
  }
  // add self
  queue.append(spec);
  return true;
}

const QList<PluginSpec *> PluginManager::loadQueue() {
  QList<PluginSpec *> queue;
  for (PluginSpec *spec : std::as_const(pluginSpecs)) {
    QList<PluginSpec *> circularityCheckQueue;
    loadQueue(spec, queue, circularityCheckQueue);
  }
  return queue;
}

void PluginManager::loadPlugins() {
  const QList<PluginSpec *> queue = loadQueue();

  qDebug() << "====loadQueue====";
  for (PluginSpec *spec : queue) {
    loadPlugin(spec, PluginSpec::Loaded);
  }
  for (PluginSpec *spec : queue) {
    loadPlugin(spec, PluginSpec::Initialized);
  }
  Utils::reverseForeach(queue, [this](PluginSpec *spec) {
    loadPlugin(spec, PluginSpec::Running);
    if (spec->state == PluginSpec::Running) {
      delayedInitializeQueue.push(spec);
    } else {
      spec->kill();
    }
  });
  delayedInitializeTimer = new QTimer;
  delayedInitializeTimer->setInterval(20);
  delayedInitializeTimer->setSingleShot(true);
  connect(delayedInitializeTimer, &QTimer::timeout, this, &PluginManager::nextDelayedInitialize);
  delayedInitializeTimer->start();
}

void PluginManager::asyncShutdownFinished() {
  auto *plugin = qobject_cast<IPlugin *>(sender());
  Q_ASSERT(plugin);
  asynchronousPlugins.remove(plugin->pluginSpec);
  if (asynchronousPlugins.isEmpty()) {
    shutdownEventLoop->exit();
  }
}

void PluginManager::nextDelayedInitialize() {
  while (!delayedInitializeQueue.empty()) {
    PluginSpec *spec = delayedInitializeQueue.front();
    delayedInitializeQueue.pop();
    bool delay = spec->delayedInitialize();
    if (delay) {
      break;        // do next delayedInitialize after a delay
    }
  }
  if (delayedInitializeQueue.empty()) {
    delete delayedInitializeTimer;
    delayedInitializeTimer = nullptr;
  } else {
    delayedInitializeTimer->start();
  }
}

void PluginManager::loadPlugin(PluginSpec *spec, PluginSpec::State destState) {
  qDebug() << "PluginManager::loadPlugin" << spec->state << destState;
  if (spec->hasError || spec->state != destState - 1) {
    return;
  }

  qDebug() << "isEffect" << spec->isEffectivelyEnabled();

  // don't load disabled plugins.
  if (!spec->isEffectivelyEnabled() && destState == PluginSpec::Loaded) {
    return;
  }

  qDebug() << "destStateA" << destState;
  switch (destState) {
  case PluginSpec::Running: spec->initializeExtensions(); return;
  case PluginSpec::Deleted: spec->kill(); return;
  default: break;
  }

  // check if dependencies have loaded without error
  const QHash<PluginDependency, PluginSpec *> deps = spec->dependencySpecs;
  for (auto it = deps.cbegin(), end = deps.cend(); it != end; ++it) {
    if (it.key().type != PluginDependency::Required) {
      continue;
    }
    PluginSpec *depSpec = it.value();
    if (depSpec->state != destState) {
      spec->hasError = true;
      qDebug() << QString("Cannot load plugin because dependency failed to load: %1(%2)\n")
                      .arg(depSpec->name)
                      .arg(depSpec->version);
      return;
    }
  }

  qDebug() << "destStateB" << destState;
  switch (destState) {
  case PluginSpec::Loaded: {
    spec->loadLibrary();
    break;
  }
  case PluginSpec::Initialized: spec->initializePlugin(); break;
  case PluginSpec::Stopped:
    if (spec->stop() == IPlugin::AsynchronousShutdown) {
      asynchronousPlugins << spec;
      connect(spec->plugin, &IPlugin::asynchronousShutdownFinished, this, &PluginManager::asyncShutdownFinished);
    }
    break;
  default: break;
  }
}

void PluginManager::shutdown() {
  if (delayedInitializeTimer && delayedInitializeTimer->isActive()) {
    delayedInitializeTimer->stop();
    delete delayedInitializeTimer;
    delayedInitializeTimer = nullptr;
  }
  const QList<PluginSpec *> queue = loadQueue();
  for (PluginSpec *spec : queue) {
    loadPlugin(spec, PluginSpec::Stopped);
  }

  if (!asynchronousPlugins.isEmpty()) {
    shutdownEventLoop = new QEventLoop;
    shutdownEventLoop->exec();
  }

  Utils::reverseForeach(loadQueue(), [this](PluginSpec *spec) { loadPlugin(spec, PluginSpec::Deleted); });

  if (!allObjects.isEmpty()) {
    qDebug() << "There are" << allObjects.size() << "objects left in the plugin manager pool.";
    qDebug() << "The following objects left in the plugin manager pool:" << allObjects;
  }
}
