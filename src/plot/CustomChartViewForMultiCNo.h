//
// Created by tang on 24-4-16.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMCHARTVIEWFORMULTICNO_H
#define GNSS_SDR_MONITOR_CUSTOMCHARTVIEWFORMULTICNO_H

#include <QWidget>
#include <QtCharts>

class CustomChartViewForMultiCNo : public QtCharts::QChartView
{
Q_OBJECT
public:
    explicit CustomChartViewForMultiCNo(QWidget *parent = nullptr);
    void addSeries(int index, const QString& name);
    void removeSeries(int index);
    void updateSeries(std::map<int,QVector<QPointF>> map);

private:
    std::unique_ptr<QtCharts::QChart> chart_;
    QMap<int, QLineSeries*> lineSeries_;

    QValueAxis *axis_x_;
    QValueAxis *axis_y_;

};


#endif  // GNSS_SDR_MONITOR_CUSTOMCHARTVIEWFORMULTICNO_H
