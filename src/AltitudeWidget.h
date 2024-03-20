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
#include "Variance.h"
#include "CustomChartView.h"

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
    std::unique_ptr<CustomChartView> chartView_;
};

#endif  // GNSS_SDR_MONITOR_ALTITUDE_WIDGET_H_
