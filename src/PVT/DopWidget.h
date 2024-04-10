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
#include "CustomChartView.h"
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
    size_t bufferSize_;

    boost::circular_buffer<QPointF> gdop_buffer_;
    boost::circular_buffer<QPointF> pdop_buffer_;
    boost::circular_buffer<QPointF> hdop_buffer_;
    boost::circular_buffer<QPointF> vdop_buffer_;

    CustomChartView* chart_view_;

};

#endif  // GNSS_SDR_MONITOR_DOP_WIDGET_H_
