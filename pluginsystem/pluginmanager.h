#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "pluginspec.h"
#include "pluginsystem_global.h"
#include <QElapsedTimer>
#include <QEventLoop>
#include <QObject>
#include <QReadWriteLock>
#include <QScopedPointer>
#include <QSet>
#include <QSettings>
#include <QStringList>
#include <QTextStream>
#include <QTimer>
#include <queue>

#define qPluginManager (PluginManager::instance())

class PluginSpec;

class PLUGINSYSTEM_EXPORT PluginManager : public QObject {
  Q_OBJECT

public:
  static PluginManager *instance();
  ~PluginManager() override;

  template <typename T>
  static T *getObject() {
    if (!instance()) {
      return nullptr;
    }
    QReadLocker lock(&(instance()->m_lock));
    // const QList<QObject *> all = instance()->allObjects;
    for (QObject *obj : instance()->allObjects) {
      if (T *result = qobject_cast<T *>(obj)) {
        return result;
      }
    }
    return nullptr;
  }
  template <typename T, typename Predicate>
  static T *getObject(Predicate predicate) {
    if (!instance()) {
      return nullptr;
    }
    QReadLocker lock(&(instance()->m_lock));
    // const QList<QObject *> all = instance()->allObjects;
    for (QObject *obj : instance()->allObjects) {
      if (T *result = qobject_cast<T *>(obj)) {
        if (predicate(result)) {
          return result;
        }
      }
    }
    return nullptr;
  }

  void addObject(QObject *obj);
  void removeObject(QObject *obj);
  QObject *getObjectByName(const QString &name);

  void setSettings(QString name);
  void setPluginPath(const QString &path);

  void loadPlugins();

  QSettings *settings = nullptr;

private:
  QList<QObject *> allObjects;
  const QList<PluginSpec *> loadQueue();
  bool loadQueue(PluginSpec *spec, QList<PluginSpec *> &queue, QList<PluginSpec *> &circularityCheckQueue);

  QTimer *delayedInitializeTimer = nullptr;
  std::queue<PluginSpec *> delayedInitializeQueue;

  QList<PluginSpec *> pluginSpecs;
  QString pluginPath;

  QStringList disabledPlugins;
  QStringList forceEnabledPlugins;

  QSet<PluginSpec *> asynchronousPlugins;         // plugins that have requested async shutdown
  QEventLoop *shutdownEventLoop = nullptr;        // used for async shutdown

  mutable QReadWriteLock m_lock;

  void asyncShutdownFinished();

  void loadPlugin(PluginSpec *spec, PluginSpec::State destState);

signals:
  void pluginsChanged();
  void initializationDone();

private slots:
  void nextDelayedInitialize();

public slots:
  void shutdown();
};

#endif        // PLUGINMANAGER_H
