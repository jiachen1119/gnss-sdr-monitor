/*!
 * \file cn0_delegate.h
 * \brief Interface of a delegate that draws a CN0 vs. time graph on
 * the view using the information from the model.
 */


#ifndef GNSS_SDR_MONITOR_CN0_DELEGATE_H_
#define GNSS_SDR_MONITOR_CN0_DELEGATE_H_

#include <QStyledItemDelegate>

class Cn0Delegate : public QStyledItemDelegate
{
public:
    Cn0Delegate(QWidget *parent = nullptr);
    ~Cn0Delegate() override = default;

public slots:
    void setBufferSize(size_t size);
    void setCn0Range(double min, double max);
    void setAutoRangeEnabled(bool enabled);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

private:
    void drawGuides(QPainter *painter, QRect cellRect, QRect sparklineRect, QRect textRect) const;
    size_t bufferSize_;
    double minCn0_;
    double maxCn0_;
    bool autoRangeEnabled_;
};

#endif  // GNSS_SDR_MONITOR_CN0_DELEGATE_H_
