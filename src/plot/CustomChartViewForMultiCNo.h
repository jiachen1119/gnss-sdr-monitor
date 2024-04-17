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

    std::map<int, QString> channel_name_;
    QColor color_gps_ = QColor(0X720320);
    QColor color_bds_ = QColor(0X207203);
    QColor color_gal_ = QColor(0X032072);

public slots:
    void updateName(std::map<int, QString> channel_name);

};


#endif  // GNSS_SDR_MONITOR_CUSTOMCHARTVIEWFORMULTICNO_H
