#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include "iplugin.h"
#include "pluginsystem_global.h"
#include <QObject>
#include <QPluginLoader>

struct PluginDependency {
  enum Type { Required, Optional };
  PluginDependency() : type(Required) {}
  QString name;
  QString version;
  Type type;
  bool operator==(const PluginDependency &other) const {
    return name == other.name && version == other.version && type == other.type;
  }
  QString toString() const {
    if (type == PluginDependency::Optional) {
      return name + " (" + version + QString(", optional") + ")";
    } else if (type == PluginDependency::Required) {
      return name + " (" + version + QString() + ")";
    }
    return QString();
  }
};

uint qHash(const PluginDependency &value);

class IPlugin;

struct PluginArgumentDescription {
  QString name;
  QString parameter;
  QString description;
};

class PLUGINSYSTEM_EXPORT PluginSpec : public QObject {
  Q_OBJECT

public:
  enum State { Invalid, Read, Resolved, Loaded, Initialized, Running, Stopped, Deleted };

private:
  PluginSpec() {}

  bool read(const QString &fileName);
  bool provides(QString pluginName, QString pluginVersion);
  bool resolveDependencies(const QList<PluginSpec *> &specs);
  QList<PluginSpec *> enableDependenciesIndirectly();
  bool loadLibrary();
  bool initializePlugin();
  bool initializeExtensions();
  bool delayedInitialize();
  IPlugin::ShutdownFlag stop();
  void kill();

  QPluginLoader loader;

  QJsonObject metaData;

  bool required = false;
  bool hiddenByDefault = false;
  bool experimental = false;
  bool enabledByDefault = true;

  QString name;
  QString version;
  QString compatVersion;
  QString vendor;
  QString copyright;
  QString license;
  QString description;
  QString url;
  QString category;
  QString location;

  QList<PluginDependency> dependencies;
  QHash<PluginDependency, PluginSpec *> dependencySpecs;
  QList<PluginArgumentDescription> argumentDescriptions;

  bool isEffectivelyEnabled() const;
  bool enabledBySettings = true;
  bool enabledIndirectly = false;
  bool forceEnabled = false;
  bool forceDisabled = false;

  QString filePath;
  QStringList arguments;

  State state{PluginSpec::Invalid};

  bool hasError{false};

  bool readMetaData(const QJsonObject &pluginMetaData);
  int versionCompare(QString version1, QString version2);

  friend class PluginManager;
  IPlugin *plugin = nullptr;
};

#endif        // PLUGINSPEC_H
