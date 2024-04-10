/*!
 * \file dop_widget.cpp
 * \brief Implementation of a widget that shows the dilution of precision
 * in a chart as reported by the receiver.
 */


#include "DopWidget.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QLayout>

DOPWidget::DOPWidget(QWidget *parent) : QWidget(parent)
{
    // Default buffer size.
    bufferSize_ = BUFFER_SIZE_FOR_PVT;

    gdop_buffer_.resize(bufferSize_);
    gdop_buffer_.clear();

    pdop_buffer_.resize(bufferSize_);
    pdop_buffer_.clear();

    hdop_buffer_.resize(bufferSize_);
    hdop_buffer_.clear();

    vdop_buffer_.resize(bufferSize_);
    vdop_buffer_.clear();

    chart_view_ = new CustomChartView(this, false, 4);
    chart_view_->setAutoScale(false);
    chart_view_->setLegend(0, QStringLiteral("GDOP"));
    chart_view_->setLegend(1, QStringLiteral("PDOP"));
    chart_view_->setLegend(2, QStringLiteral("HDOP"));
    chart_view_->setLegend(3, QStringLiteral("VDOP"));

    chart_view_->setTitle(QStringLiteral("DOP"));
    chart_view_->setAxisTitle("TOW (s)","DOP");
    chart_view_->chart()->axes(Qt::Vertical).back()->setRange(0, 10);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(chart_view_);
}

/*!
 Adds the \a gdop, \a pdop, \a hdop, \a vdop and associated \a tow to the widget's internal data structures.
 */
void DOPWidget::addData(qreal tow, qreal gdop, qreal pdop, qreal hdop, qreal vdop)
{
    gdop_buffer_.push_back(QPointF(tow, gdop));
    pdop_buffer_.push_back(QPointF(tow, pdop));
    hdop_buffer_.push_back(QPointF(tow, hdop));
    vdop_buffer_.push_back(QPointF(tow, vdop));
}

/*!
 Redraws the chart by calling populateSeries() on all series objects.
 */
void DOPWidget::redraw()
{
    chart_view_->updateChart_noIndex(gdop_buffer_,0);
    chart_view_->updateChart_noIndex(pdop_buffer_,1);
    chart_view_->updateChart_noIndex(hdop_buffer_,2);
    chart_view_->updateChart_noIndex(vdop_buffer_,3);
}

/*!
 Clears all the data from the widget's internal data structures.
 */
void DOPWidget::clear()
{
    gdop_buffer_.clear();
    pdop_buffer_.clear();
    hdop_buffer_.clear();
    vdop_buffer_.clear();

    chart_view_->clear();
}

/*!
 Sets the size of the internal circular buffers that store the widget's data.
 */
void DOPWidget::setBufferSize(size_t size)
{
    bufferSize_ = size;

    gdop_buffer_.resize(bufferSize_);
    pdop_buffer_.resize(bufferSize_);
    hdop_buffer_.resize(bufferSize_);
    vdop_buffer_.resize(bufferSize_);
}