//
// Created by tang on 24-3-19.
//

#include "CustomChartView.h"
CustomChartView::CustomChartView(QWidget *parent, bool isPoint) : QChartView(parent) {

    isPoint_ = isPoint;

    // 抗锯齿
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setContentsMargins(0,0,0,0);

    // 不去设置parent
    chart_ = std::make_unique<QChart>();
    // 必须设置chartview的chart，否则不会出现图线
    this->setChart(chart_.get());
    chart_->legend()->hide();
    if (isPoint){
        scatterSeries_.push_back(std::make_unique<QScatterSeries>(chart_.get()));
        scatterSeries_.at(0)->setMarkerSize(8);
        chart_->addSeries(scatterSeries_.at(0).get());
    }
    else{
        lineSeries_ .push_back(std::make_unique<QLineSeries>(chart_.get()));
        chart_->addSeries(lineSeries_.at(0).get());
    }

    chart_->createDefaultAxes();
    chart_->layout()->setContentsMargins(0,0,0,0);
    chart_->setContentsMargins(-18, -18, -14, -16);
}

CustomChartView::CustomChartView(QWidget* parent, bool isPoint, int num_series) : QChartView(parent)
{
    isPoint_ = isPoint;

    // 抗锯齿
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setContentsMargins(0,0,0,0);

    // 不去设置parent
    chart_ = std::make_unique<QChart>();
    // 必须设置chartview的chart，否则不会出现图线
    this->setChart(chart_.get());
    chart_->legend()->setVisible(true);
    chart_->legend()->setFont(QFont("Calibri", 15, QFont::Medium));
    for (int i = 0; i < num_series; ++i)
    {
        if (isPoint){
            scatterSeries_.push_back(std::make_unique<QScatterSeries>(chart_.get()));
            scatterSeries_.at(i)->setMarkerSize(8);
            chart_->addSeries(scatterSeries_.at(i).get());
        }
        else{
            lineSeries_ .push_back(std::make_unique<QLineSeries>(chart_.get()));
            chart_->addSeries(lineSeries_.at(i).get());
        }
    }
    chart_->createDefaultAxes();
    chart_->layout()->setContentsMargins(0,0,0,0);
    chart_->setContentsMargins(-18, -18, -14, -16);
}


void CustomChartView::updateChart(const QModelIndex &index)
{
    QVector<QPointF> points;
    QPointF p;

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();

    QList<QVariant> var = index.data(Qt::DisplayRole).toList();

    for (const auto & i : var)
    {
        p = i.toPointF();
        points << p;

        min_x = std::min(min_x, p.x());
        max_x = std::max(max_x, p.x());
        min_y = std::min(min_y, p.y());
        max_y = std::max(max_y, p.y());
    }
    if (isPoint_)
        scatterSeries_.at(0)->replace(points);
    else
        lineSeries_.at(0)->replace(points);

    chart_->axes(Qt::Horizontal).back()->setRange(min_x,max_x);
    chart_->axes(Qt::Vertical).back()->setRange(min_y,max_y);
}
void CustomChartView::updateCN0Chart(const QModelIndex &index)
{
    QVector<QPointF> points;
    QPointF p;

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    QList<QVariant> var = index.data(Qt::DisplayRole).toList();

    for (const auto & i : var)
    {
        p = i.toPointF();
        points << p;

        min_x = std::min(min_x, p.x());
        max_x = std::max(max_x, p.x());
    }
    lineSeries_.at(0)->replace(points);

    chart_->axes(Qt::Horizontal).back()->setRange(min_x,max_x);
    chart_->axes(Qt::Vertical).back()->setRange(25,55);
}

void CustomChartView::updateChart_noIndex(const boost::circular_buffer<QPointF>& buffer)
{
    if (!buffer.empty())
    {
        double min_x = std::numeric_limits<double>::max();
        double max_x = -std::numeric_limits<double>::max();

        double min_y = std::numeric_limits<double>::max();
        double max_y = -std::numeric_limits<double>::max();

        QVector<QPointF> points;

        for (auto i : buffer)
        {
            points << i;
            min_x = std::min(min_x, i.x());
            min_y = std::min(min_y, i.y());

            max_x = std::max(max_x, i.x());
            max_y = std::max(max_y, i.y());
        }

        if (isPoint_)
            scatterSeries_.at(0)->replace(points);
        else
            lineSeries_.at(0)->replace(points);

        chart_->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
        chart_->axes(Qt::Vertical).back()->setRange(min_y, max_y);
    }
    else{
        QVector<QPointF> empty_vector;
        if (isPoint_)
            scatterSeries_.at(0)->replace(empty_vector);
        else
            lineSeries_.at(0)->replace(empty_vector);
    }
}

void CustomChartView::updateChart_noIndex(const boost::circular_buffer<QPointF>& buffer, int index)
{
    if (index + 1 > std::max(scatterSeries_.size(),lineSeries_.size())){
        return;
    }
    if (!buffer.empty())
    {
        if (auto_scale_){
            double min_x = std::numeric_limits<double>::max();
            double max_x = -std::numeric_limits<double>::max();

            double min_y = std::numeric_limits<double>::max();
            double max_y = -std::numeric_limits<double>::max();

            QVector<QPointF> points;

            for (auto i : buffer)
            {
                points << i;
                min_x = std::min(min_x, i.x());
                min_y = std::min(min_y, i.y());

                max_x = std::max(max_x, i.x());
                max_y = std::max(max_y, i.y());
            }
            chart_->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
            chart_->axes(Qt::Vertical).back()->setRange(min_y, max_y);
            if (isPoint_)
                scatterSeries_.at(index)->replace(points);
            else
                lineSeries_.at(index)->replace(points);
        }
        else{
            double min_x = std::numeric_limits<double>::max();
            double max_x = -std::numeric_limits<double>::max();
            QVector<QPointF> points;

            for (auto i : buffer)
            {
                points << i;
                min_x = std::min(min_x, i.x());
                max_x = std::max(max_x, i.x());
            }
            chart_->axes(Qt::Horizontal).back()->setRange(min_x, max_x);
            if (isPoint_)
                scatterSeries_.at(index)->replace(points);
            else
                lineSeries_.at(index)->replace(points);
        }
    }
    else{
        QVector<QPointF> empty_vector;
        if (isPoint_)
            scatterSeries_.at(index)->replace(empty_vector);
        else
            lineSeries_.at(index)->replace(empty_vector);
    }
}

void CustomChartView::setTitle(const QString& title)
{
    chart_->setTitle(title);
    chart_->setTitleFont(QFont("Calibri", 16, QFont::Bold));
}

void CustomChartView::setAxisTitle(const QString& x_title, const QString& y_title)
{
    chart_->axes(Qt::Horizontal).back()->setTitleText(x_title);
    chart_->axes(Qt::Horizontal).back()->setTitleFont(QFont("Calibri", 13, QFont::DemiBold));
    chart_->axes(Qt::Horizontal).back()->setLabelsFont(QFont("Calibri", 12, QFont::Medium));

    chart_->axes(Qt::Vertical).back()->setTitleText(y_title);
    chart_->axes(Qt::Vertical).back()->setTitleFont(QFont("Calibri", 13, QFont::DemiBold));
    chart_->axes(Qt::Vertical).back()->setLabelsFont(QFont("Calibri", 12, QFont::Medium));

}

void CustomChartView::setLegend(const int& index, const QString& name)
{
    if (index + 1 > std::max(scatterSeries_.size(),lineSeries_.size()))
        return;
    if (scatterSeries_.size() > lineSeries_.size()){
        scatterSeries_.at(index)->setName(name);
    }
    else
    {
        lineSeries_.at(index)->setName(name);
    }
}

void CustomChartView::clear()
{
    if (!lineSeries_.empty()){
        for (const auto & i : lineSeries_)
        {
            i->clear();
        }
    }
    if (!scatterSeries_.empty()){
        for (const auto & j : scatterSeries_)
        {
            j->clear();
        }
    }
}

void CustomChartView::setAutoScale(bool open)
{
    auto_scale_ = open;
}
