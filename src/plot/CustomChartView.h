//
// Created by tang on 24-3-19.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H
#define GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H

#include <QtCharts>
#include <QTimer>
#include "Variance.h"

class CustomChartView : public QtCharts::QChartView
{
    Q_OBJECT
public:
    explicit CustomChartView(QWidget *parent, bool isPoint);
    void setTitle(const QString& title);
    void setAxisTitle(const QString& x_title,const QString& y_title);

public slots:
    void updateChart(const QModelIndex &index);
    void updateCN0Chart(const QModelIndex &index);

private:
    std::unique_ptr<QtCharts::QChart> chart_;
    std::unique_ptr<QLineSeries> lineSeries_;
    std::unique_ptr<QScatterSeries> scatterSeries_;

};


#endif  // GNSS_SDR_MONITOR_CUSTOMCHARTVIEW_H
