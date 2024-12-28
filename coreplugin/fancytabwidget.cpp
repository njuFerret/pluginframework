#include "fancytabwidget.h"

FancyTabWidget::FancyTabWidget(QWidget *parent)
    : QWidget(parent)
{
    m_tabBar = new QWidget(this);
    m_tabLayout = new QVBoxLayout;

    //m_tabLayout->setMargin(0);
    m_tabLayout->setContentsMargins(0,0,0,0);
    m_tabBar->setLayout(m_tabLayout);
}

//int FancyTabWidget::currentIndex() const
//{
//    return m_tabBar->currentIndex();
//}

//void FancyTabWidget::setCurrentIndex(int index)
//{
//    m_tabBar->setCurrentIndex(index);
//}

void FancyTabWidget::insertTab(int index, QWidget *tab, const QIcon &icon, const QString &label, bool hasMenu)
{
    m_modesStack->insertWidget(index, tab);
//    m_tabBar->insertTab(index, icon, label, hasMenu);
    m_tabLayout->addWidget(new QPushButton(icon, label));
}

void FancyTabWidget::setTabEnabled(int index, bool enable)
{
//    m_tabBar->setTabEnabled(index, enable);
    m_tabLayout->itemAt(index)->widget()->setEnabled(enable);
}
