//
// Created by tang on 24-3-19.
//

#include "CustomChannelView.h"
#include <QHeaderView>

CustomChannelView::CustomChannelView(QWidget *parent) : QTableView (parent)
{
    this->verticalHeader()->hide();
    this->setShowGrid(false);

    this->horizontalHeader()->setMinimumWidth(60);//设置水平单元格最小宽度
    this->verticalHeader()->setMinimumSectionSize(50);//设置垂直单元格最小高度
    this->verticalHeader()->setMaximumSectionSize(72);//设置垂直单元格最大高度
    this->verticalHeader()->setDefaultSectionSize(90);

    // 设置表格文字大小
    this->setFont(QFont("Calibri", 15, QFont::Medium));

    this->horizontalHeader()->setFixedHeight(40);

}

void CustomChannelView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    autoAdjustTableItemWidth();
    //    autoAdjustTableItemHeight();
}

void CustomChannelView::showEvent(QShowEvent *event)
{
    QTableView::showEvent(event);
    autoAdjustTableItemWidth();
}

void CustomChannelView::autoAdjustTableItemWidth()
{
    // 设置水平滚动条策略为按需显示
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // 获取水平表头
    auto hHeaderView = this->horizontalHeader();
    // 设置列的调整模式为根据内容自动调整大小
    hHeaderView->setSectionResizeMode(QHeaderView::ResizeToContents);
    // 计算所有列的总宽度
    int section_total_width = 0;
    for (int col = 0; col < hHeaderView->count(); ++col)
        section_total_width += hHeaderView->sectionSize(col);

    // 恢复列的调整模式为交互式
    hHeaderView->setSectionResizeMode(QHeaderView::Interactive);
    // 获取水平表头的宽度
    int hHeaderWidth = hHeaderView->width();
    // 如果水平表头的宽度大于所有列的总宽度
    if (hHeaderWidth > section_total_width){
        // 计算缩放比例
        float scale = hHeaderWidth / float(section_total_width);
        int width_sum = 0;
        // 根据缩放比例调整每列的宽度
        for (int col = 0; col < hHeaderView->count() - 1; ++col) {
            int cell_width = hHeaderView->sectionSize(col) * scale;
            hHeaderView->resizeSection(col, cell_width);
            width_sum += cell_width;
        }
        // 调整最后一列的宽度，确保总宽度与表头宽度一致
        hHeaderView->resizeSection(hHeaderView->count() - 1, hHeaderWidth - width_sum);
        // 关闭水平滚动条，因为此时表头宽度已经足够显示所有列
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}
