//
// Created by tang on 24-3-19.
//

#include "CustomTableView.h"
#include <QHeaderView>

CustomTableView::CustomTableView(QWidget *parent) : QTableView (parent)
{
    this->setSelectionMode(QTableView::NoSelection);
    this->verticalHeader()->setSectionsClickable(false);
    this->horizontalHeader()->hide();

    this->horizontalHeader()->setMinimumWidth(60);//设置水平单元格最小宽度
    this->verticalHeader()->setMinimumSectionSize(30);//设置垂直单元格最小高度
    this->verticalHeader()->setMaximumSectionSize(50);//设置垂直单元格最大高度
}

void CustomTableView::resizeEvent(QResizeEvent *event)
{
    QTableView::resizeEvent(event);
    autoAdjustTableItemWidth();
    autoAdjustTableItemHeight();
}

void CustomTableView::showEvent(QShowEvent *event)
{
    QTableView::showEvent(event);
    autoAdjustTableItemWidth();
    autoAdjustTableItemHeight();
}

void CustomTableView::autoAdjustTableItemWidth()
{
    // 设置水平滚动条策略为按需显示
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    // 获取水平表头
    QHeaderView *hHeaderView = this->horizontalHeader();
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

void CustomTableView::autoAdjustTableItemHeight()
{
    // 获取垂直表头
    QHeaderView *vHeaderView = this->verticalHeader();
    // 获取垂直表头的高度
    int vHeaderHeight = vHeaderView->height();
    // 计算每行的理论高度，以保证均匀分配表头高度
    int cell_height = vHeaderHeight / vHeaderView->count();

    // 如果每行的理论高度大于最大值
    if(cell_height > vHeaderView->maximumSectionSize()){
        // 将表头的调整模式设置为交互式
        vHeaderView->setSectionResizeMode(QHeaderView::Interactive);

        // 将所有行的高度设置为最大值
        for(int row = 0; row < vHeaderView->count(); row++)
            vHeaderView->resizeSection(row, vHeaderView->maximumSectionSize());
    }
    else{
        // 如果理论高度小于等于最大值，将表头的调整模式设置为拉伸（Stretch）
        vHeaderView->setSectionResizeMode(QHeaderView::Stretch);
    }
}
