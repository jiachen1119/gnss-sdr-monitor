//
// Created by tang on 24-3-19.
//

#include "CustomChartView.h"
CustomChartView::CustomChartView(QWidget *parent, bool isPoint) : QChartView(parent) {

    // 抗锯齿
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setContentsMargins(0,0,0,0);

    // 不去设置parent
    chart_ = std::make_unique<QChart>();
    if (isPoint){
        scatterSeries_ = std::make_unique<QScatterSeries>(chart_.get());
        scatterSeries_->setMarkerSize(8);
        chart_->addSeries(scatterSeries_.get());
        chart_->legend()->hide();
    }
    else{
        lineSeries_ = std::make_unique<QLineSeries>(chart_.get());
        chart_->addSeries(lineSeries_.get());
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
    lineSeries_->replace(points);

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
    lineSeries_->replace(points);

    chart_->axes(Qt::Horizontal).back()->setRange(min_x,max_x);
    chart_->axes(Qt::Vertical).back()->setRange(25,55);
}

void CustomChartView::setTitle(const QString& title)
{
    chart_->setTitle(title);
}

void CustomChartView::setAxisTitle(const QString& x_title, const QString& y_title)
{
    chart_->axes(Qt::Horizontal).back()->setTitleText(x_title);
    chart_->axes(Qt::Vertical).back()->setTitleText(y_title);
}
