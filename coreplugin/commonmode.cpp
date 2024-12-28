#include "commonmode.h"
#include "modemanager.h"

CommonMode::CommonMode(QObject *parent) : IContext(parent)
{
    qModeManager->m_modes.append(this);
}

CommonMode::~CommonMode()
{
    const int index = qModeManager->m_modes.indexOf(this);
    qModeManager->m_modes.remove(index);
    if (qModeManager->m_startingUp)
    {
        return;
    }

//    qModeManager->m_mainWindow->removeContextObject(this);
}
