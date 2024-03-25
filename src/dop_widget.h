/*!
 * \file dop_widget.h
 * \brief Interface of a widget that shows the dilution of precision
 * in a chart as reported by the receiver.
 *
 * \author Álvaro Cebrián Juan, 2019. acebrianjuan(at)gmail.com
 *
 */


#ifndef GNSS_SDR_MONITOR_DOP_WIDGET_H_
#define GNSS_SDR_MONITOR_DOP_WIDGET_H_

#include <boost/circular_buffer.hpp>
#include <QChartView>
#include <QLineSeries>
#include <QWidget>
#include "Variance.h"

class DOPWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DOPWidget(QWidget *parent = nullptr);

public slots:
    void addData(qreal tow, qreal gdop, qreal pdop, qreal hdop, qreal vdop);
    void redraw();
    void clear();
    void setBufferSize(size_t size);

private:
    void populateSeries(boost::circular_buffer<QPointF> queue, QtCharts::QLineSeries *series);

    size_t bufferSize_;

    boost::circular_buffer<QPointF> gdopBuffer_;
    boost::circular_buffer<QPointF> pdopBuffer_;
    boost::circular_buffer<QPointF> hdopBuffer_;
    boost::circular_buffer<QPointF> vdopBuffer_;

    QtCharts::QChartView *m_chartView = nullptr;

    QtCharts::QLineSeries *gdopSeries_ = nullptr;
    QtCharts::QLineSeries *m_pdopSeries = nullptr;
    QtCharts::QLineSeries *m_hdopSeries = nullptr;
    QtCharts::QLineSeries *m_vdopSeries = nullptr;

    double min_x;
    double min_y;

    double max_x;
    double max_y;
};

#endif  // GNSS_SDR_MONITOR_DOP_WIDGET_H_
