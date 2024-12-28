#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include <QObject>
#include "fancytabwidget.h"
#include "icontext.h"

class MainWindow;

#define qModeManager (ModeManager::instance())

class ModeManager : public QObject
{
    Q_OBJECT

public:
    static ModeManager* instance();
    void init(MainWindow *mainWindow, FancyTabWidget *modeStack);
    void extensionsInitialized();

    int indexOf(QString id);
    void activateMode(QString id);
    void enabledStateChanged(IContext *mode);
    void appendMode(IContext *mode);

    MainWindow *m_mainWindow;
    FancyTabWidget *m_modeStack;

    QVector<IContext*> m_modes;
    bool m_startingUp = true;
    QString m_pendingFirstActiveMode;

};

#endif // MODEMANAGER_H
