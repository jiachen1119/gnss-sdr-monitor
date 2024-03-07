#ifndef GNSS_SDR_MONITOR_CONSTELLATION_DELEGATE_H_
#define GNSS_SDR_MONITOR_CONSTELLATION_DELEGATE_H_

#include <QStyledItemDelegate>

class ConstellationDelegate : public QStyledItemDelegate
{
public:
    explicit ConstellationDelegate(QWidget *parent = nullptr);
    ~ConstellationDelegate() override;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
        const QModelIndex &index) const;
};

#endif  // GNSS_SDR_MONITOR_CONSTELLATION_DELEGATE_H_
