/*!combine the altitude part into customChartView
 * \file altitude_widget.cpp
 * \brief Implementation of a widget that shows the altitude in a chart as
 * reported by the receiver.
 */

#include "AltitudeWidget.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QLayout>

/*!
 Constructs an altitude widget.
 */
AltitudeWidget::AltitudeWidget(QWidget *parent) : QWidget(parent)
{
    // Default buffer size.
    bufferSize_ = BUFFER_SIZE_FOR_PVT;

    altitudeBuffer_.resize(bufferSize_);
    altitudeBuffer_.clear();

    auto layout = new QVBoxLayout(this);
    chartView_ = std::make_unique<CustomChartView>(nullptr, false);
    layout->addWidget(chartView_.get());
    chartView_->setTitle("Altitude");
    chartView_->setAxisTitle("TOW (s)","Altitude (m)");
}

/*!
 Adds the \a alitude and associated \a tow to the widget's internal data structures.
 */
void AltitudeWidget::addData(qreal tow, qreal altitude)
{
    altitudeBuffer_.push_back(QPointF(tow, altitude));
}


/*!
 Clears all the data from the widget's internal data structures.
 */
void AltitudeWidget::clear()
{
    altitudeBuffer_.clear();
    chartView_->chart()->series().clear();
}

/*!
 Sets the size of the internal circular buffer that stores the widget's data.
 */
void AltitudeWidget::setBufferSize(size_t size)
{
    bufferSize_ = size;
    altitudeBuffer_.resize(bufferSize_);
}

void AltitudeWidget::redraw()
{
    chartView_->updateChart_noIndex(altitudeBuffer_);
}
