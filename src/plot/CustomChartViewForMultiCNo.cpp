//
// Created by tang on 24-4-16.
//

#include "CustomChartViewForMultiCNo.h"
CustomChartViewForMultiCNo::CustomChartViewForMultiCNo(QWidget *parent) : QtCharts::QChartView(parent)
{
    // 抗锯齿
    this->setRenderHint(QPainter::Antialiasing, true);
    this->setContentsMargins(0,0,0,0);
    // 不去设置parent
    chart_ = std::make_unique<QChart>();
    // 必须设置chartview的chart，否则不会出现图线
    this->setChart(chart_.get());

    // createDefaultAxes 不起作用，因为没有数据
    chart_->layout()->setContentsMargins(0,0,0,0);
    chart_->setContentsMargins(-18, -18, -14, -16);

    // 设置坐标轴
    axis_x_ = new QValueAxis();
    axis_x_->setTitleText(QStringLiteral("TOW (s)"));
    axis_x_->setTitleFont(QFont("Calibri", 15, QFont::Medium));

    axis_y_ = new QValueAxis();
    axis_y_->setTitleText(QStringLiteral("C/N0 (dB-Hz)"));
    axis_y_->setTitleFont(QFont("Calibri", 15, QFont::Medium));
    axis_y_->setRange(25, 55);

    chart_->addAxis(axis_x_, Qt::AlignBottom);
    chart_->addAxis(axis_y_, Qt::AlignLeft);

    chart_->legend()->setVisible(true);
    chart_->setTitle(QStringLiteral("C/N0 in Multi-Channel"));
    chart_->setTitleFont(QFont("Calibri", 20, QFont::Bold));
}

void CustomChartViewForMultiCNo::addSeries(int index, const QString &name)
{
    if (lineSeries_.contains(index))
    {
        qDebug() << "The series already exists!";
        return;
    }
    lineSeries_[index] = new QLineSeries(chart_.get());
    lineSeries_[index]->setName(name);
    chart_->addSeries(lineSeries_[index]);
    lineSeries_[index]->attachAxis(axis_x_);
    lineSeries_[index]->attachAxis(axis_y_);
}

void CustomChartViewForMultiCNo::removeSeries(int index)
{
    if (lineSeries_.contains(index)){
        chart_->removeSeries(lineSeries_[index]);
        lineSeries_.remove(index);
    }
}

void CustomChartViewForMultiCNo::updateSeries(std::map<int,QVector<QPointF>> map)
{
    if (!map.empty()){
        for (auto &it : map)
        {
            if (lineSeries_.contains(it.first))
            {
                lineSeries_[it.first]->replace(it.second);
            }
            else
            {
                addSeries(it.first, QString::number(it.first));
                lineSeries_[it.first]->replace(it.second);
            }
        }
        for (auto it = lineSeries_.begin(); it != lineSeries_.end(); ++it)
        {
            if (map.find(it.key()) == map.end())
            {
                removeSeries(it.key());
            }
        }
        axis_x_->setRange(map.begin()->second.first().x(), map.begin()->second.last().x());

    }
}


