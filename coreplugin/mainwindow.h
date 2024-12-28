#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include "modemanager.h"
#include "fancytabwidget.h"
#include "navigationwidget.h"
#include <unordered_map>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

    void extensionsInitialized();
    void aboutToShutdown();

    IContext *currentContextObject() const;
    IContext *contextObject(QWidget *widget) const;
    void addContextObject(IContext *context);
    void removeContextObject(IContext *context);
    void updateContextObject(const QList<IContext *> &context);
    void updateContext();

private:
    FancyTabWidget *m_modeStack = nullptr;
    NavigationWidget *m_leftNavigationWidget = nullptr;
    NavigationWidget *m_rightNavigationWidget = nullptr;
    QToolButton *m_toggleLeftSideBarButton = nullptr;
    QToolButton *m_toggleRightSideBarButton = nullptr;

    QList<IContext *> m_activeContext;
    std::unordered_map<QWidget *, IContext *> m_contextWidgets;
};

#endif // MAINWINDOW_H
