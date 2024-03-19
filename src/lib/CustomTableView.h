//
// Created by tang on 24-3-19.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMTABLEVIEW_H
#define GNSS_SDR_MONITOR_CUSTOMTABLEVIEW_H


#pragma once
#include <QTableView>

class CustomTableView : public QTableView
{
    Q_OBJECT
public:
    explicit CustomTableView(QWidget *parent=nullptr);
    void autoAdjustTableItemWidth();
    void autoAdjustTableItemHeight();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
};


#endif  // GNSS_SDR_MONITOR_CUSTOMTABLEVIEW_H
