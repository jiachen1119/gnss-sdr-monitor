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
    CustomChartView(QWidget *parent, bool isPoint, int num_series);

    void setTitle(const QString& title);
    void setAxisTitle(const QString& x_title,const QString& y_title);
    void updateChart(const QModelIndex &index);
    void updateCN0Chart(const QModelIndex &index);

    void updateChart_noIndex(const boost::circular_buffer<QPointF>& buffer);
    void updateChart_noIndex(const boost::circular_buffer<QPointF>& buffer, int index);

    void setLegend(const int& index, const QString& name);

private:
    std::unique_ptr<QtCharts::QChart> chart_;
    std::vector<std::unique_ptr<QLineSeries>> lineSeries_;
    std::vector<std::unique_ptr<QScatterSeries>> scatterSeries_;

    bool isPoint_ = false;
};


#endif  // GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H
