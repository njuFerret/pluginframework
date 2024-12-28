#include "modemanager.h"
#include "algorithm.h"
#include <QDebug>
#include <mainwindow.h>

Q_GLOBAL_STATIC(ModeManager, modeManager)

ModeManager *ModeManager::instance()
{
    return modeManager();
}

void ModeManager::init(MainWindow *mainWindow, FancyTabWidget *modeStack)
{
    m_mainWindow = mainWindow;
    m_modeStack = modeStack;
}

void ModeManager::extensionsInitialized()
{
    m_startingUp = false;

//    Utils::sort(m_modes, &IContext::priority);
    std::reverse(m_modes.begin(), m_modes.end());

    for (IContext *mode : m_modes)
    {
        appendMode(mode);
    }

    if (!m_pendingFirstActiveMode.isEmpty())
    {
        activateMode(m_pendingFirstActiveMode);
    }
}

int ModeManager::indexOf(QString id)
{
    for (int i = 0; i < m_modes.count(); ++i)
    {
        if (m_modes.at(i)->m_id == id)
        {
            return i;
        }
    }
    qDebug() << "Warning, no such mode:" << id;
    return -1;
}

void ModeManager::activateMode(QString id)
{
//    if (m_startingUp)
//    {
//        m_pendingFirstActiveMode = id;
//    }
//    else
//    {
//        const int currentIndex = m_modeStack->currentIndex();
//        const int newIndex = indexOf(id);
//        if (newIndex != currentIndex && newIndex >= 0)
//        {
//            m_modeStack->setCurrentIndex(newIndex);
//        }
//    }
}

void ModeManager::appendMode(IContext *mode)
{
    const int index = m_modes.count();

    m_mainWindow->addContextObject(mode);

    m_modeStack->insertTab(index, mode->widget(), mode->m_icon, mode->m_displayName, mode->m_menu != nullptr);
    m_modeStack->setTabEnabled(index, mode->m_isEnabled);

//    QObject::connect(mode, &IContext::enabledStateChanged, [this, mode] { enabledStateChanged(mode); });
}

void ModeManager::enabledStateChanged(IContext *mode)
{
//    int index = m_modes.indexOf(mode);
//    m_modeStack->setTabEnabled(index, mode->m_isEnabled);

//    // Make sure we leave any disabled mode to prevent possible crashes:
//    if (mode->m_id == qModeManager->currentModeId() && !mode->isEnabled())
//    {
//        // This assumes that there is always at least one enabled mode.
//        for (int i = 0; i < d->m_modes.count(); ++i)
//        {
//            if (m_modes.at(i) != mode && m_modes.at(i)->isEnabled())
//            {
//                ModeManager::activateMode(m_modes.at(i)->id());
//                break;
//            }
//        }
//    }
}
