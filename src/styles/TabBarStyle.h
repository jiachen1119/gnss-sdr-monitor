//
// Created by tang on 24-3-8.
//

#ifndef GNSS_SDR_MONITOR_TABBARSTYLE_H
#define GNSS_SDR_MONITOR_TABBARSTYLE_H


#pragma once

#include <QPainter>
#include <QProxyStyle>
#include <QStyleOptionTab>

class TabBarStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
        const QSize &size, const QWidget *widget) const override;

    void drawControl(ControlElement element, const QStyleOption *option,
        QPainter *painter, const QWidget *widget) const override;
};

#endif  // GNSS_SDR_MONITOR_TABBARSTYLE_H
