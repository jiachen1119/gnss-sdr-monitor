/*!
 * \file dop_widget.cpp
 * \brief Implementation of a widget that shows the dilution of precision
 * in a chart as reported by the receiver.
 */


#include "dop_widget.h"
#include <QChart>
#include <QGraphicsLayout>
#include <QLayout>

DOPWidget::DOPWidget(QWidget *parent) : QWidget(parent)
{
    // Default buffer size.
    bufferSize_ = BUFFER_SIZE_FOR_PVT;

    gdopBuffer_.resize(bufferSize_);
    gdopBuffer_.clear();

    pdopBuffer_.resize(bufferSize_);
    pdopBuffer_.clear();

    hdopBuffer_.resize(bufferSize_);
    hdopBuffer_.clear();

    vdopBuffer_.resize(bufferSize_);
    vdopBuffer_.clear();

    gdopSeries_ = new QtCharts::QLineSeries();
    gdopSeries_->setName("GDOP");

    m_pdopSeries = new QtCharts::QLineSeries();
    m_pdopSeries->setName("PDOP");

    m_hdopSeries = new QtCharts::QLineSeries();
    m_hdopSeries->setName("HDOP");

    m_vdopSeries = new QtCharts::QLineSeries();
    m_vdopSeries->setName("VDOP");

    m_chartView = new QtCharts::QChartView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    this->setLayout(layout);
    layout->addWidget(m_chartView);

    QtCharts::QChart *chart = m_chartView->chart();
    chart->addSeries(gdopSeries_);
    chart->addSeries(m_pdopSeries);
    chart->addSeries(m_hdopSeries);
    chart->addSeries(m_vdopSeries);

    chart->setTitle("DOP vs Time");
    chart->createDefaultAxes();
    chart->axes(Qt::Horizontal).back()->setTitleText("TOW [s]");
    chart->axes(Qt::Vertical).back()->setTitleText("DOP");
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setContentsMargins(-18, -18, -14, -16);

    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setContentsMargins(0, 0, 0, 0);

    min_x = std::numeric_limits<double>::max();
    max_x = -std::numeric_limits<double>::max();

    min_y = std::numeric_limits<double>::max();
    max_y = -std::numeric_limits<double>::max();
}

/*!
 Adds the \a gdop, \a pdop, \a hdop, \a vdop and associated \a tow to the widget's internal data structures.
 */
void DOPWidget::addData(qreal tow, qreal gdop, qreal pdop, qreal hdop, qreal vdop)
{
    gdopBuffer_.push_back(QPointF(tow, gdop));
    pdopBuffer_.push_back(QPointF(tow, pdop));
    hdopBuffer_.push_back(QPointF(tow, hdop));
    vdopBuffer_.push_back(QPointF(tow, vdop));
}

/*!
 Redraws the chart by calling populateSeries() on all series objects.
 */
void DOPWidget::redraw()
{
    populateSeries(gdopBuffer_, gdopSeries_);
    populateSeries(pdopBuffer_, m_pdopSeries);
    populateSeries(hdopBuffer_, m_hdopSeries);
    populateSeries(vdopBuffer_, m_vdopSeries);
}

/*!
 Clears all the data from the widget's internal data structures.
 */
void DOPWidget::clear()
{
    gdopBuffer_.clear();
    pdopBuffer_.clear();
    hdopBuffer_.clear();
    vdopBuffer_.clear();

    gdopSeries_->clear();
    m_pdopSeries->clear();
    m_hdopSeries->clear();
    m_vdopSeries->clear();
}

/*!
 Sets the size of the internal circular buffers that store the widget's data.
 */
void DOPWidget::setBufferSize(size_t size)
{
    bufferSize_ = size;

    gdopBuffer_.resize(bufferSize_);
    pdopBuffer_.resize(bufferSize_);
    hdopBuffer_.resize(bufferSize_);
    vdopBuffer_.resize(bufferSize_);
}

/*!
 Replaces the old data in the \a series object with the new data from the \a buffer, casuing the chart to repaint.
 */
void DOPWidget::populateSeries(boost::circular_buffer<QPointF> buffer, QtCharts::QLineSeries *series)
{
    if (!buffer.empty())
    {
        double min_x = std::numeric_limits<double>::max();
        double max_x = -std::numeric_limits<double>::max();

        /*
        double min_y = std::numeric_limits<double>::max();
        double max_y = -std::numeric_limits<double>::max();
        */

        QtCharts::QChart *chart = m_chartView->chart();
        QPointF p;
        QVector<QPointF> vec;

        for (size_t i = 0; i < buffer.size(); i++)
        {
            p = buffer.at(i);
            vec << p;

            min_x = std::min(min_x, p.x());
            min_y = std::min(min_y, p.y());

            max_x = std::max(max_x, p.x());
            max_y = std::max(max_y, p.y());
        }

        series->replace(vec);

        chart->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
        chart->axes(Qt::Vertical).back()->setRange(min_y, max_y);
    }
}
