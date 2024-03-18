/*!
 * \file altitude_widget.h
 * \brief Interface of a widget that shows the altitude in a chart as
 * reported by the receiver.
 */


#ifndef GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
#define GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_

#include <boost/circular_buffer.hpp>
#include <QChartView>
#include <QLineSeries>
#include <QWidget>
#include "lib/Variance.h"

class AltitudeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AltitudeWidget(QWidget *parent = nullptr);

public slots:
    void addData(qreal tow, qreal altitude);
    void redraw();
    void clear();
    void setBufferSize(size_t size);

private:
    size_t bufferSize_;
    boost::circular_buffer<QPointF> altitudeBuffer_;
    QtCharts::QChartView *chartView_ = nullptr;
    QtCharts::QLineSeries *series_ = nullptr;

    double min_x;
    double min_y;

    double max_x;
    double max_y;
};

#endif  // GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
