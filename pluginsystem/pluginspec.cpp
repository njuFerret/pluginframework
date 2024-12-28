#include "pluginspec.h"
#include "algorithm.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QRegularExpression>

uint qHash(const PluginDependency &value) { return qHash(value.name); }

// Returns false if the file does not represent a Qt Creator plugin.
bool PluginSpec::read(const QString &fileName) {
  qDebug() << "\nReading meta data of" << fileName;

  state = PluginSpec::Invalid;
  hasError = false;
  dependencies.clear();
  metaData = QJsonObject();

  QFileInfo fileInfo(fileName);
  location = fileInfo.absolutePath();
  filePath = fileInfo.absoluteFilePath();

  loader.setFileName(filePath);
  if (loader.fileName().isEmpty()) {
    qDebug() << "Cannot open file";
    return false;
  }

  if (!readMetaData(loader.metaData())) {
    return false;
  }

  state = PluginSpec::Read;
  return true;
}

void readMultiLineString(const QJsonValue &value, QString out) {
  if (value.isString()) {
    out = value.toString();
  } else if (value.isArray()) {
    QStringList lines;
    for (const QJsonValue &v : value.toArray()) {
      if (!v.isString()) {
        return;
      }
      lines.append(v.toString());
    }
    out = lines.join(QLatin1Char('\n'));
  } else {
    return;
  }
  return;
}

bool PluginSpec::readMetaData(const QJsonObject &pluginMetaData) {
  qDebug() << "MetaData:" << pluginMetaData;

  QJsonValue value;
  value = pluginMetaData.value(QLatin1String("IID"));
  qDebug() << "IID:" << value.toString();

  value = pluginMetaData.value(QLatin1String("MetaData"));
  if (!value.isObject()) {
    hasError = true;
    qDebug("Plugin meta data not found");
    return true;
  }
  metaData = value.toObject();

  name = metaData.value("Name").toString();
  version = metaData.value("Version").toString();
  compatVersion = metaData.value("CompatVersion").toString();

  required = metaData.value(QLatin1String("Required")).toBool(false);
  qDebug() << "required =" << required;

  hiddenByDefault = metaData.value(QLatin1String("HiddenByDefault")).toBool(false);
  qDebug() << "hiddenByDefault =" << hiddenByDefault;

  experimental = metaData.value(QLatin1String("Experimental")).toBool(false);
  qDebug() << "experimental =" << experimental;

  enabledByDefault = !metaData.value(QLatin1String("DisabledByDefault")).toBool(false);
  qDebug() << "enabledByDefault =" << enabledByDefault;

  if (experimental) {
    enabledByDefault = false;
  }
  enabledBySettings = enabledByDefault;

  vendor = metaData.value(QLatin1String("Vendor")).toString();
  copyright = metaData.value(QLatin1String("Copyright")).toString();
  readMultiLineString(metaData.value("Description"), description);
  url = metaData.value(QLatin1String("Url")).toString();
  category = metaData.value(QLatin1String("Category")).toString();
  readMultiLineString(metaData.value("License"), license);

  for (const QJsonValue &v : metaData.value("Dependencies").toArray()) {
    QJsonObject dependencyObject = v.toObject();
    PluginDependency dep;
    dep.name = dependencyObject.value("Name").toString();
    dep.version = dependencyObject.value("Version").toString();
    dep.type = PluginDependency::Required;
    QString typeValue = dependencyObject.value("Type").toString();
    if (typeValue.toLower() == "required") {
      dep.type = PluginDependency::Required;
    } else if (typeValue.toLower() == "optional") {
      dep.type = PluginDependency::Optional;
    }
    dependencies.append(dep);
  }
  for (const QJsonValue &v : metaData.value("Arguments").toArray()) {
    QJsonObject argumentObject = v.toObject();
    PluginArgumentDescription arg;
    arg.name = argumentObject.value("Name").toString();
    arg.description = argumentObject.value("Description").toString();
    arg.parameter = argumentObject.value("Parameter").toString();
    argumentDescriptions.append(arg);
  }
  return true;
}

int PluginSpec::versionCompare(QString version1, QString version2) {
  QRegularExpression pattern =
      QRegularExpression(QLatin1String("([0-9]+)(?:[.]([0-9]+))?(?:[.]([0-9]+))?(?:_([0-9]+))?"));

  QRegularExpressionMatch match1 = pattern.match(version1);
  QRegularExpressionMatch match2 = pattern.match(version2);

  if (!match1.hasMatch()) {
    return 0;
  }
  if (!match1.hasMatch()) {
    return 0;
  }

  qDebug() << "match1:" << match1 << ", match2:" << match2;
  int number1;
  int number2;
  for (int i = 0; i < 4; ++i) {
    number1 = match1.captured(i + 1).toInt();
    number2 = match2.captured(i + 1).toInt();
    if (number1 < number2) {
      return -1;
    }
    if (number1 > number2) {
      return 1;
    }
  }
  return 0;
}

bool PluginSpec::provides(QString pluginName, QString pluginVersion) {
  if (QString::compare(pluginName, name, Qt::CaseInsensitive) != 0) {
    return false;
  }
  return (versionCompare(version, pluginVersion) >= 0) && (versionCompare(compatVersion, pluginVersion) <= 0);
}

bool PluginSpec::resolveDependencies(const QList<PluginSpec *> &specs) {
  if (hasError) {
    return false;
  }
  if (state == PluginSpec::Resolved) {
    state = PluginSpec::Read;        // Go back, so we just re-resolve the dependencies.
  }
  if (state != PluginSpec::Read) {
    qDebug() << "Resolving dependencies failed because state != Read";
    hasError = true;
    return false;
  }

  QHash<PluginDependency, PluginSpec *> resolvedDependencies;
  for (const PluginDependency &dependency : std::as_const(dependencies)) {
    PluginSpec *const found = Utils::findOrDefault(specs, [&dependency](PluginSpec *spec) {
      qDebug() << "spec name:" << spec->name << "dependency name:" << dependency.name;
      return spec->provides(dependency.name, dependency.version);
    });
    if (!found) {
      if (dependency.type == PluginDependency::Required) {
        hasError = true;
        qDebug() << QString("Could not resolve dependency '%1(%2)'").arg(dependency.name).arg(dependency.version);
      }
      continue;
    }
    resolvedDependencies.insert(dependency, found);
  }
  if (hasError) {
    return false;
  }

  dependencySpecs = resolvedDependencies;

  state = PluginSpec::Resolved;

  return true;
}

// Returns whether the plugin is loaded at startup. isEnabledBySettings()
bool PluginSpec::isEffectivelyEnabled() const {
  if (forceEnabled || enabledIndirectly) {
    return true;
  }
  if (forceEnabled) {
    return false;
  }
  return enabledBySettings;
}

// returns the plugins that it actually indirectly enabled
QList<PluginSpec *> PluginSpec::enableDependenciesIndirectly() {
  if (!isEffectivelyEnabled())        // plugin not enabled, nothing to do
  {
    return {};
  }
  QList<PluginSpec *> enabled;
  for (auto it = dependencySpecs.cbegin(), end = dependencySpecs.cend(); it != end; ++it) {
    if (it.key().type != PluginDependency::Required) {
      continue;
    }
    PluginSpec *dependencySpec = it.value();
    if (!dependencySpec->isEffectivelyEnabled()) {
      dependencySpec->enabledIndirectly = true;
      enabled << dependencySpec;
    }
  }
  return enabled;
}

bool PluginSpec::loadLibrary() {
  qDebug() << "====PluginSpec::loadLibrary====";

  if (hasError) {
    return false;
  }
  if (state != PluginSpec::Resolved) {
    if (state == PluginSpec::Loaded) {
      return true;
    }
    qDebug() << "Loading the library failed because state != Resolved";
    hasError = true;
    return false;
  }
  if (!loader.load()) {
    hasError = true;
    qDebug() << QDir::toNativeSeparators(filePath) + QString::fromLatin1(": ") + loader.errorString();
    return false;
  }
  auto *pluginObject = qobject_cast<IPlugin *>(loader.instance());
  if (!pluginObject) {
    hasError = true;
    qDebug() << "Plugin is not valid (does not derive from IPlugin)";
    loader.unload();
    return false;
  }
  state = PluginSpec::Loaded;
  plugin = pluginObject;
  plugin->pluginSpec = this;
  return true;
}

bool PluginSpec::initializePlugin() {
  qDebug() << "====PluginSpec::initializePlugin====";

  if (hasError) {
    return false;
  }
  if (state != PluginSpec::Loaded) {
    if (state == PluginSpec::Initialized) {
      return true;
    }
    qDebug() << "Initializing the plugin failed because state != Loaded";
    hasError = true;
    return false;
  }
  if (!plugin) {
    qDebug() << "Internal error: have no plugin instance to initialize";
    hasError = true;
    return false;
  }

  QString err;
  if (!plugin->initialize(arguments, &err)) {
    qDebug() << QString("Plugin initialization failed: %1").arg(err);
    hasError = true;
    return false;
  }
  state = PluginSpec::Initialized;
  return true;
}

bool PluginSpec::initializeExtensions() {
  qDebug() << "====PluginSpec::initializeExtensions====";

  if (hasError) {
    return false;
  }
  if (state != PluginSpec::Initialized) {
    if (state == PluginSpec::Running) {
      return true;
    }
    qDebug() << "Cannot perform extensionsInitialized because state != Initialized";
    hasError = true;
    return false;
  }
  if (!plugin) {
    qDebug() << "Internal error: have no plugin instance to perform extensionsInitialized";
    hasError = true;
    return false;
  }
  plugin->extensionsInitialized();
  state = PluginSpec::Running;
  return true;
}

bool PluginSpec::delayedInitialize() {
  if (hasError) {
    return false;
  }
  if (state != PluginSpec::Running) {
    return false;
  }
  if (!plugin) {
    qDebug() << "Internal error: have no plugin instance to perform delayedInitialize";
    hasError = true;
    return false;
  }
  return plugin->delayedInitialize();
}

IPlugin::ShutdownFlag PluginSpec::stop() {
  if (!plugin) {
    return IPlugin::SynchronousShutdown;
  }
  state = PluginSpec::Stopped;
  return plugin->aboutToShutdown();
}

void PluginSpec::kill() {
  if (!plugin) {
    return;
  }
  delete plugin;
  plugin = nullptr;
  state = PluginSpec::Deleted;
}
