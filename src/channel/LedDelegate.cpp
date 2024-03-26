/*!
 * \file led_delegate.cpp
 * \brief Implementation of a delegate that draws an led light indicator on
 * the view using the information from the model.
 */


#include "LedDelegate.h"
#include <QDebug>
#include <QPainter>

#include <QDebug>
#include <QPainter>

LedDelegate::LedDelegate(QObject *parent)
{
}

void LedDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    int radius = 0.3 * QStyledItemDelegate::sizeHint(option, index).height();

    int size = qMin(option.rect.width(), option.rect.height());

    bool state = index.model()->data(index, Qt::DisplayRole).toBool();

    painter->save();

    if (state)
    {
        painter->setBrush(QBrush(QColor("#01FF70"), Qt::SolidPattern));
    }
    else
    {
        painter->setBrush(QBrush(QColor("#FF4136"), Qt::SolidPattern));
    }

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    painter->drawEllipse(QRectF(option.rect.x() + option.rect.width() / 2 - radius,
        option.rect.y() + option.rect.height() / 2 - radius,
        2 * radius, 2 * radius));

    painter->restore();
}

QSize LedDelegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return {QStyledItemDelegate::sizeHint(option, index).height(),
        QStyledItemDelegate::sizeHint(option, index).height()};
}