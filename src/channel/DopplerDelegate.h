/*!
 * \file doppler_delegate.h
 * \brief Interface of a delegate that draws a Doppler vs. time graph on
 * the view using the information from the model.
 */


#ifndef GNSS_SDR_MONITOR_DOPPLER_DELEGATE_H_
#define GNSS_SDR_MONITOR_DOPPLER_DELEGATE_H_

#include <QStyledItemDelegate>

class DopplerDelegate : public QStyledItemDelegate
{
public:
    DopplerDelegate(QWidget *parent = nullptr);
    ~DopplerDelegate() override = default;

public slots:
    void setBufferSize(int size);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const override;

private:
    void drawGuides(QPainter *painter, QRect cellRect, QRect sparklineRect, QRect textRect) const;

    int bufferSize_;
};

#endif  // GNSS_SDR_MONITOR_DOPPLER_DELEGATE_H_
