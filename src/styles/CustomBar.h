//
// Created by tang on 24-4-13.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMBAR_H
#define GNSS_SDR_MONITOR_CUSTOMBAR_H

#include <QToolBar>
#include <QLabel>
#include <QVBoxLayout>

class CustomBar : public QToolBar
{
    Q_OBJECT
public:
    explicit CustomBar(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *label_tab_name_;
    QLabel *label_project_;

    QAction *start_;
    QAction *stop_;
    QAction *clear_;
};


#endif  // GNSS_SDR_MONITOR_CUSTOMBAR_H
