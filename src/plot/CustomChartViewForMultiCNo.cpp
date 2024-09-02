//
// Created by tang on 24-4-16.
//

#include "CustomChartViewForMultiCNo.h"
#include <iostream>
#include <utility>
CustomChartViewForMultiCNo::CustomChartViewForMultiCNo(QWidget *parent) : QtCharts::QChartView(parent)
{
    // set a buffer for updating the chart (remove the channel that had no data)
    bufferPRN_ = new boost::circular_buffer<int>(15);
    timer_ = new QTimer(this);

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
    if (name.contains("GPS", Qt::CaseInsensitive))
    {
        lineSeries_[index]->setColor(color_gps_);
        color_gps_ = color_gps_.lighter(125);
    }
    else if (name.contains("GAL", Qt::CaseInsensitive))
    {
        lineSeries_[index]->setColor(color_gal_);
        color_gal_ = color_gal_.lighter(125);
    }
    else if (name.contains("BDS", Qt::CaseInsensitive))
    {
        lineSeries_[index]->setColor(color_bds_);
        color_bds_ = color_bds_.lighter(125);
    }
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
            // update the buffer of PRN
            bufferPRN_->push_back(it.first);
            if (lineSeries_.contains(it.first))
            {
                lineSeries_[it.first]->replace(it.second);
            }
            else
            {
                if (channel_name_.find(it.first) != channel_name_.end())
                {
                    addSeries(it.first, channel_name_[it.first]);
                }
                else
                {
                    qDebug() << "The channel name does not exist!";
                    addSeries(it.first, QString::number(it.first));
                }
                lineSeries_[it.first]->replace(it.second);
            }
        }
        // remove the channel that had no data
        if (bufferPRN_->full())
        {
            if (lineSeries_.empty())
            {
                bufferPRN_->clear();
                return;
            }
            for (auto &prn : *bufferPRN_)
            {
                std::cout<<"prn "<<prn<<std::endl;
            }
            for (auto it = lineSeries_.begin(); it != lineSeries_.end(); ++it)
            {
                std::cout<<"k " <<it.key()<<std::endl;
                if (std::find(bufferPRN_->begin(), bufferPRN_->end(), it.key()) == bufferPRN_->end())
                {
                    std::cout<<"remove "<<it.key()<<std::endl;
                    removeSeries(it.key());
                    break;
                }
            }
        }
        axis_x_->setRange(map.begin()->second.first().x(), map.begin()->second.last().x());
    }
    else
    {
        std::cout << "The map is empty!" << std::endl;
        bufferPRN_->clear();
        if (lineSeries_.empty())
        {
            return;
        }
        lineSeries_.clear();
        chart_->removeAllSeries();
    }
}

void CustomChartViewForMultiCNo::updateName(std::map<int, QString> channel_name)
{
    channel_name_ = std::move(channel_name);
}
