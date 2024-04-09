//
// Created by tang on 24-4-9.
//

#ifndef GNSS_SDR_MONITOR_ALERTWIDGET_H
#define GNSS_SDR_MONITOR_ALERTWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include "CustomChartView.h"
#include <QTextBrowser>
#include <boost/circular_buffer.hpp>

class AlertWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlertWidget(QWidget *parent = nullptr);

private:
    QGridLayout *layout_;
    CustomChartView *al_level_;
    CustomChartView *position_corrected_;
    QTextBrowser *log_;
};


#endif  // GNSS_SDR_MONITOR_ALERTWIDGET_H
