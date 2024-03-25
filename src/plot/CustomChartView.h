//
// Created by tang on 24-3-19.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H
#define GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H

#include <QtCharts>
#include <QTimer>
#include "Variance.h"
#include <boost/circular_buffer.hpp>

class CustomChartView : public QtCharts::QChartView
{
    Q_OBJECT
public:
    explicit CustomChartView(QWidget *parent, bool isPoint);
    void setTitle(const QString& title);
    void setAxisTitle(const QString& x_title,const QString& y_title);
    void updateChart(const QModelIndex &index);
    void updateCN0Chart(const QModelIndex &index);
    void updateChart_noIndex(const boost::circular_buffer<QPointF>& buffer);

private:
    std::unique_ptr<QtCharts::QChart> chart_;
    std::unique_ptr<QLineSeries> lineSeries_;
    std::unique_ptr<QScatterSeries> scatterSeries_;

    bool isPoint_ = false;
};


#endif  // GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H
