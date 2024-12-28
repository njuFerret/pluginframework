#include "coreplugin.h"
#include "modemanager.h"

CorePlugin::CorePlugin()
{

}

CorePlugin::~CorePlugin()
{
    delete m_commonMode;
    delete m_mainWindow;
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    m_mainWindow = new MainWindow;
    m_mainWindow->setWindowState(m_mainWindow->windowState() & ~Qt::WindowMinimized);
    m_mainWindow->raise();
    m_mainWindow->activateWindow();

    m_commonMode = new CommonMode;
    qModeManager->activateMode(m_commonMode->m_id);

    return true;
}

void CorePlugin::extensionsInitialized()
{
    m_mainWindow->extensionsInitialized();
}

bool CorePlugin::delayedInitialize()
{
    return true;
}

IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    m_mainWindow->aboutToShutdown();
    return SynchronousShutdown;
}
