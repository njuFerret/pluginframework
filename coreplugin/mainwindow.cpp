#include "mainwindow.h"
#include "modemanager.h"

MainWindow::MainWindow()
{
    m_modeStack = new FancyTabWidget(this);
    qModeManager->init(this, m_modeStack);
//    m_leftNavigationWidget = new NavigationWidget(m_toggleLeftSideBarAction, Left);
//    m_rightNavigationWidget = new NavigationWidget(m_toggleRightSideBarAction, Side::Right);

    setCentralWidget(m_modeStack);
}

MainWindow::~MainWindow()
{
    delete m_leftNavigationWidget;
    delete m_rightNavigationWidget;
    m_leftNavigationWidget = nullptr;
    m_rightNavigationWidget = nullptr;
}

void MainWindow::extensionsInitialized()
{
    qModeManager->extensionsInitialized();
}

void MainWindow::aboutToShutdown()
{
    m_activeContext.clear();
    hide();
}

IContext *MainWindow::contextObject(QWidget *widget) const
{
    const auto it = m_contextWidgets.find(widget);
    return it == m_contextWidgets.end() ? nullptr : it->second;
}

IContext *MainWindow::currentContextObject() const
{
    return m_activeContext.isEmpty() ? nullptr : m_activeContext.first();
}

void MainWindow::addContextObject(IContext *context)
{
    if (!context)
    {
        return;
    }
    QWidget *widget = context->widget();
    if (m_contextWidgets.find(widget) != m_contextWidgets.end())
    {
        return;
    }

    m_contextWidgets.insert(std::make_pair(widget, context));
    connect(context, &QObject::destroyed, this, [this, context] { removeContextObject(context); });
}

void MainWindow::removeContextObject(IContext *context)
{
    if (!context)
    {
        return;
    }

    disconnect(context, &QObject::destroyed, this, nullptr);

    const auto it = std::find_if(m_contextWidgets.cbegin(), m_contextWidgets.cend(),
                                 [context](const std::pair<QWidget *, IContext *> &v) { return v.second == context; });

    if (it == m_contextWidgets.cend())
    {
        return;
    }

    m_contextWidgets.erase(it);
    if (m_activeContext.removeAll(context) > 0)
    {
        updateContextObject(m_activeContext);
    }
}

void MainWindow::updateContext()
{
//    Context contexts = m_highPrioAdditionalContexts;

//    foreach (IContext *context, m_activeContext)
//    {
//        contexts.add(context->context());
//    }

//    contexts.add(m_lowPrioAdditionalContexts);

//    Context uniquecontexts;
//    for (const QString &id : qAsConst(contexts)) {
//        if (!uniquecontexts.contains(id))
//            uniquecontexts.add(id);
//    }

//    ActionManager::setContext(uniquecontexts);
//    emit m_coreImpl->contextChanged(uniquecontexts);
}

void MainWindow::updateContextObject(const QList<IContext *> &context)
{
//    emit m_coreImpl->contextAboutToChange(context);
    m_activeContext = context;
    updateContext();
}
