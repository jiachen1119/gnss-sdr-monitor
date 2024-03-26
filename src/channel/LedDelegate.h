/*!
 * \file led_delegate.h
 * \brief Interface of a delegate that draws an led light indicator on
 * the view using the information from the model.
 */


#ifndef GNSS_SDR_MONITOR_LED_DELEGATE_H_
#define GNSS_SDR_MONITOR_LED_DELEGATE_H_

#include <QStyledItemDelegate>

class LedDelegate : public QStyledItemDelegate
{
public:
    explicit LedDelegate(QObject *parent = nullptr);
    ~LedDelegate() override = default;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif  // GNSS_SDR_MONITOR_LED_DELEGATE_H_
