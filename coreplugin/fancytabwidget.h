#ifndef FANCYTABWIDGET_H
#define FANCYTABWIDGET_H

#include <QStackedLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class FancyTabWidget : public QWidget
{
    Q_OBJECT

public:
    FancyTabWidget(QWidget *parent = nullptr);
//    int currentIndex() const;
//    void setCurrentIndex(int index);

    QWidget *m_tabBar;
    QVBoxLayout *m_tabLayout;
    QStackedLayout *m_modesStack;

    void setTabEnabled(int index, bool enable);
    void insertTab(int index, QWidget *tab, const QIcon &icon, const QString &label, bool hasMenu);
};

#endif // FANCYTABWIDGET_H
