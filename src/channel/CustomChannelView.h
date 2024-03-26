//
// Created by tang on 24-3-19.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMCHANNELVIEW_H
#define GNSS_SDR_MONITOR_CUSTOMCHANNELVIEW_H
#include <QTableView>


class CustomChannelView : public QTableView
{
    Q_OBJECT
public:
    explicit CustomChannelView(QWidget *parent=nullptr);
    void autoAdjustTableItemWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

};


#endif  // GNSS_SDR_MONITOR_CUSTOMCHANNELVIEW_H
