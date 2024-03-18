/*!
 * \file altitude_widget.cpp
 * \brief Implementation of a widget that shows the altitude in a chart as
 * reported by the receiver.
 */

#include "altitude_widget.h"
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

    series_ = new QtCharts::QLineSeries();
    chartView_ = new QtCharts::QChartView(this);

    auto *layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->addWidget(chartView_);

    QtCharts::QChart *chart = chartView_->chart();

    chart->addSeries(series_);
    chart->setTitle("Altitude vs Time");
    chart->legend()->hide();
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
    chart->axes(Qt::Vertical).back()->setTitleText("Altitude [m]");
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-18, -18, -14, -16);

    chartView_->setRenderHint(QPainter::Antialiasing);
    chartView_->setContentsMargins(0, 0, 0, 0);

    min_x = std::numeric_limits<double>::max();
    max_x = -std::numeric_limits<double>::max();

    min_y = std::numeric_limits<double>::max();
    max_y = -std::numeric_limits<double>::max();
}

/*!
 Adds the \a alitude and associated \a tow to the widget's internal data structures.
 */
void AltitudeWidget::addData(qreal tow, qreal altitude)
{
    altitudeBuffer_.push_back(QPointF(tow, altitude));
}

/*!
 Redraws the chart by replacing the old data in the series object with the new data.
 */
void AltitudeWidget::redraw()
{
    if (!altitudeBuffer_.empty())
    {
        double min_x = std::numeric_limits<double>::max();
        double max_x = -std::numeric_limits<double>::max();

        double min_y = std::numeric_limits<double>::max();
        double max_y = -std::numeric_limits<double>::max();

        QtCharts::QChart *chart = chartView_->chart();
        QPointF p;
        QVector<QPointF> vec;

        for (size_t i = 0; i < altitudeBuffer_.size(); i++)
        {
            p = altitudeBuffer_.at(i);
            vec << p;

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        series_->replace(vec);

        chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
        chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
    }
}

/*!
 Clears all the data from the widget's internal data structures.
 */
void AltitudeWidget::clear()
{
    altitudeBuffer_.clear();
    series_->clear();
}

/*!
 Sets the size of the internal circular buffer that stores the widget's data.
 */
void AltitudeWidget::setBufferSize(size_t size)
{
    bufferSize_ = size;
    altitudeBuffer_.resize(bufferSize_);
}
