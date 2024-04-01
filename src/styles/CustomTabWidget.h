//
// Created by tang on 24-3-30.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMTABWIDGET_H
#define GNSS_SDR_MONITOR_CUSTOMTABWIDGET_H

#include "QTabWidget"

class CustomTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit CustomTabWidget(QWidget *parent = nullptr);
    ~CustomTabWidget() override = default;
};


#endif  // GNSS_SDR_MONITOR_CUSTOMTABWIDGET_H
