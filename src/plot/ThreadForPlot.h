//
// Created by tang on 24-4-15.
//

#ifndef GNSS_SDR_MONITOR_THREADFORPLOT_H
#define GNSS_SDR_MONITOR_THREADFORPLOT_H

#include <QThread>

class ThreadForPlot : public QThread
{
    Q_OBJECT
public:
    explicit ThreadForPlot(QObject *parent = nullptr);
    void run() override;

private:
    bool thread_stop_ = false;
};


#endif  // GNSS_SDR_MONITOR_THREADFORPLOT_H
