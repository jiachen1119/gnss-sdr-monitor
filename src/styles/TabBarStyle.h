//
// Created by tang on 24-3-8.
//

#ifndef GNSS_SDR_MONITOR_TABBARSTYLE_H
#define GNSS_SDR_MONITOR_TABBARSTYLE_H


#pragma once

#include <QProxyStyle>

class TabBarStyle : public QProxyStyle
{
    Q_OBJECT

public:
    explicit TabBarStyle(Qt::Orientation orientation = Qt::Vertical);
    ~TabBarStyle() override;

    void drawControl(QStyle::ControlElement element, const QStyleOption *option,
        QPainter *painter, const QWidget *widget = nullptr) const override;

    void drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal,
        bool enabled, const QString &text, QPalette::ColorRole textRole = QPalette::NoRole) const override;

    // 控制CE_TabBarTabLabel的尺寸
    QSize sizeFromContents(QStyle::ContentsType type, const QStyleOption *option,
        const QSize &contentsSize, const QWidget *widget = nullptr) const override;

private:
    Qt::Orientation orientation_;	// 文本方向
};


#endif  // GNSS_SDR_MONITOR_TABBARSTYLE_H
