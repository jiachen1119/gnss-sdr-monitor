//
// Created by tang on 24-3-30.
//

#include "CustomTabWidget.h"
#include "TabBarStyle.h"
#include "Variance.h"

CustomTabWidget::CustomTabWidget(QWidget *parent) : QTabWidget(parent)
{
    // 左侧侧边栏设置
    this->setTabPosition(QTabWidget::West);
    this->setDocumentMode(true);

    // 自定义qstyle
    this->tabBar()->setStyle(new TabBarStyle);

    // 只有开启WA_Hover才可以探查到是否有鼠标悬浮
    this->tabBar()->setAttribute(Qt::WA_Hover, true);
    this->tabBar()->setAttribute(Qt::WA_StyledBackground, true);

    // 设置背景色和边界距离
    this->tabBar()->setStyleSheet("background-color: #414141;");
    //    this->widget(0)->setStyleSheet("background-color: #ececec;");
    this->tabBar()->setContentsMargins(0,0,0,0);

    alert_widget_ = new AlertWidget(nullptr);

    // 添加tab
    this->addTab(new QWidget,QStringLiteral("Channel"));
    this->addTab(new QWidget,QStringLiteral("Solution"));
    this->addTab(alert_widget_,QStringLiteral("Alert"));
    this->addTab(new QWidget,QStringLiteral("Settings"));
    this->addTab(new QWidget,QStringLiteral("About"));

    // 设置起始页
    this->setCurrentIndex(0);

    // 设置Icon
    auto icon_channel_push = QIcon(":/images/dashboard.svg");
    auto icon_channel_normal = QIcon(":/images/dashboard2.svg");

    auto iconSolution_push = QIcon(":/images/map.svg");
    auto iconSolution_normal = QIcon(":/images/map2.svg");

    auto iconAlert_push = QIcon(":/images/alert.svg");
    auto iconAlert_normal = QIcon(":/images/alert2.svg");

    auto iconSettings_push = QIcon(":/images/settings.svg");
    auto iconSettings_normal = QIcon(":/images/settings2.svg");

    auto iconAbout_push = QIcon(":/images/about.svg");
    auto iconAbout_normal = QIcon(":/images/about2.svg");

    std::vector<QIcon> icon_normal {icon_channel_normal, iconSolution_normal,
        iconAlert_normal, iconSettings_normal,iconAbout_normal};
    std::vector<QIcon> icon_push {icon_channel_push, iconSolution_push,
        iconAlert_push, iconSettings_push, iconAbout_push};

    this->tabBar()->setTabIcon(TAB_CHANNEL, icon_push.at(TAB_CHANNEL));
    this->tabBar()->setTabIcon(TAB_SOLUTION, icon_normal.at(TAB_SOLUTION));
    this->tabBar()->setTabIcon(TAB_ALERT, icon_normal.at(TAB_ALERT));
    this->tabBar()->setTabIcon(TAB_SETTINGS, icon_normal.at(TAB_SETTINGS));
    this->tabBar()->setTabIcon(TAB_ABOUT, icon_normal.at(TAB_ABOUT));

    // 设置Icon切换
    connect(this,&QTabWidget::currentChanged, this,
        [=](int index){
            this->tabBar()->setTabIcon(TAB_CHANNEL, icon_normal.at(TAB_CHANNEL));
            this->tabBar()->setTabIcon(TAB_SOLUTION, icon_normal.at(TAB_SOLUTION));
            this->tabBar()->setTabIcon(TAB_ALERT, icon_normal.at(TAB_ALERT));
            this->tabBar()->setTabIcon(TAB_SETTINGS, icon_normal.at(TAB_SETTINGS));
            this->tabBar()->setTabIcon(TAB_ABOUT, icon_normal.at(TAB_ABOUT));

            this->tabBar()->setTabIcon(index,icon_push.at(index));
        });

}
