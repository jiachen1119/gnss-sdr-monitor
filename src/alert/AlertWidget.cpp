//
// Created by tang on 24-4-9.
//

#include "AlertWidget.h"
#include <iostream>
AlertWidget::AlertWidget(QWidget *parent): QWidget(parent)
{
    layout_ = new QGridLayout(this);
    layout_->setRowStretch(0, 5);
    layout_->setRowStretch(1, 3);
    layout_->setColumnStretch(0, 1);
    layout_->setColumnStretch(1, 1);

    al_level_ = new CustomChartView(nullptr, false,2);
    al_level_->setTitle(QStringLiteral("AL Level"));
    al_level_->setAxisTitle("Time (s)", "Level");

    position_corrected_ = new CustomChartView(nullptr, true);
    position_corrected_->setTitle(QStringLiteral("Position after Correction"));

    layout_->addWidget(al_level_, 0, 0, 1, 1);
    layout_->addWidget(position_corrected_, 0, 1, 1, 1);

    log_ = new QTextBrowser;
    layout_->addWidget(log_, 1, 0, 1, 2);
    log_->setContentsMargins(5, 5, 5, 5);

    // QTextBrowser content setting
    log_->setMarkdown(
        "## 尚未开启卫星完好性检测  ......\n"
        "\n"
        "## 开始卫星完好性故障检测，将提供告警信息和定位信息\n"
        "\n"
        "### 第20个历元存在故障\n"
        "\n"
        "故障源是第<font color=red >1</font>个通道,对应卫星号为：<font color=red>32</font>\n"
        "故障源是第<font color=red >2</font>个通道,对应卫星号为：<font color=red>25</font>\n"
        "\n"
        "### 第40个历元存在故障\n"
        "\n"
        "故障源是第<font color=red >3</font>个通道,对应卫星号为：<font color=red>10</font>\n"
        "故障源是第<font color=red >4</font>个通道,对应卫星号为：<font color=red>22</font>\n"
        "故障源是第<font color=red >5</font>个通道,对应卫星号为：<font color=red>31</font>");

    boost::circular_buffer<QPointF> buffer_HPE;
    boost::circular_buffer<QPointF> buffer_HPL;
    buffer_HPE.resize(50);
    buffer_HPL.resize(50);
    buffer_pos_.resize(50);

    QFile file("/home/tang/1.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        QString line1 = in.readLine().trimmed();
        line1.replace(QRegExp("[\\s]+"), " ");
        QStringList list1 = line1.split(" ");
        for (int i = 0; i < list1.size(); i++)
        {
            buffer_HPE.push_back(QPointF(i, list1.at(i).toFloat()));
        }

        QString line2 = in.readLine().trimmed();
        line2.replace(QRegExp("[\\s]+"), " ");
        QStringList list2 = line2.split(" ");
        for (int j = 0; j < list2.size(); j++)
        {
            buffer_HPL.push_back(QPointF(j, list2.at(j).toFloat()));
        }
    }
    file.close();
    al_level_->setLineSeriesWidth(0,5);
    al_level_->setLineSeriesWidth(1,5);

    al_level_->updateChart_noIndex(buffer_HPE,0);
    al_level_->updateChart_noIndex(buffer_HPL,1);

    al_level_->setLegend(0, QStringLiteral("HPE"));
    al_level_->setLegend(1, QStringLiteral("HPL"));

}
void AlertWidget::positionShow(PVTStruct in)
{
    buffer_pos_.push_back(QPointF(in.latitude,in.longitude));
    position_corrected_->updateChart_noIndex(buffer_pos_);
}
