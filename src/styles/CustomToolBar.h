//
// Created by tang on 24-4-13.
//

#ifndef GNSS_SDR_MONITOR_CUSTOMTOOLBAR_H
#define GNSS_SDR_MONITOR_CUSTOMTOOLBAR_H

#include <QToolBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>

class CustomToolBar : public QToolBar
{
    Q_OBJECT
public:
    explicit CustomToolBar(QWidget *parent = nullptr);

public slots:
    void setTabName(const int &index);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *label_tab_name_;
    QLabel *label_project_;
    QFrame* line_;
    QSpacerItem *spacer_;

    QAction *start_;
    QAction *stop_;
    QAction *clear_;
};


#endif  // GNSS_SDR_MONITOR_CUSTOMTOOLBAR_H
