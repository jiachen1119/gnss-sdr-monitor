/*!
 * \file cn0_delegate.cpp
 * \brief Implementation of a delegate that draws a CN0 vs. time graph on
 * the view using the information from the model.
 */


#include "Cn0Delegate.h"
#include "Variance.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <limits>

#define SPARKLINE_MIN_EM_WIDTH 10

Cn0Delegate::Cn0Delegate(QWidget *parent) : QStyledItemDelegate(parent)
{
    // Default buffer size.
    bufferSize_ = BUFFER_SIZE_FOR_CHANNEL;

    // Default CN0 range.
    minCn0_ = 20;
    maxCn0_ = 55;

    // Default state of auto range function.
    autoRangeEnabled_ = false;
}

/*!
 Sets the size of the internal vector that stores the sparkline data.
 */
void Cn0Delegate::setBufferSize(size_t size)
{
    bufferSize_ = size;
}

/*!
 Sets the manual limits of the vertical axis for rendering the sparkline.
 */
void Cn0Delegate::setCn0Range(double min, double max)
{
    if (min < max)
    {
        minCn0_ = min;
        maxCn0_ = max;
    }
}

/*!
 Sets the state of the auto range function.
 */
void Cn0Delegate::setAutoRangeEnabled(bool enabled)
{
    autoRangeEnabled_ = enabled;
}

void Cn0Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    bool outOfScale = false;

    QVector<QPointF> points;
    QVector<double> x_data, y_data;
    QList<QVariant> var = index.data(Qt::DisplayRole).toList();
    for (const auto & i : var)
    {
        points << i.toPointF();
        x_data << i.toPointF().x();
        y_data << i.toPointF().y();
    }

    double min_x = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();

    double min_y = minCn0_;
    double max_y = maxCn0_;

    if (autoRangeEnabled_)
    {
        min_y = std::numeric_limits<double>::max();
        max_y = -std::numeric_limits<double>::max();
    }

    int em_w = option.fontMetrics.height();

    int hGap = em_w / 4;
    int vGap = (option.rect.height() - option.fontMetrics.height()) / 2;
    int cGap = em_w / 4;

    int cellWidth = option.rect.width();
    int cellHeight = option.rect.height();

    int contentWidth = cellWidth - 2 * hGap;
    int contentHeight = option.fontMetrics.height();

    int usableContentWidth = contentWidth - cGap;
    int textWidth = option.fontMetrics.horizontalAdvance("00.0");
    int sparklineWidth = usableContentWidth - textWidth;

    // Offset for translating the origin of the painting coordinate system to the top left corner of the cell.
    QPoint offset = option.rect.topLeft();

    // Translated origins.
    QPoint cellOrigin = QPoint(0, 0);
    QPoint sparklineOrigin = QPoint(hGap, vGap);
    QPoint textOrigin = QPoint(hGap + sparklineWidth + cGap, vGap);

    // Translated rectangles.
    QRect cellRect = QRect(cellOrigin, QSize(cellWidth, cellHeight));
    QRect sparklineRect = QRect(sparklineOrigin, QSize(sparklineWidth, contentHeight));
    QRect textRect = QRect(textOrigin, QSize(textWidth, contentHeight));

    QPointF val;
    QVector<QPointF> fpoints;
    QStyledItemDelegate::paint(painter, option, index);

    if (points.isEmpty() || bufferSize_ < 1.0 || contentHeight <= 0)
    {
        return;
    }

    // Remove first points until the number of elements is within the designated buffer size.
    while (points.length() > bufferSize_)
    {
        points.removeFirst();
    }

    foreach (val, points)
    {
        // Find the min and max values of the time data (horizontal axis).
        if (val.x() < min_x)
        {
            min_x = val.x();
        }

        if (val.x() > max_x)
        {
            max_x = val.x();
        }

        // Find the min and max values of the CN0 data (vertical axis) if auto range is enabled.
        if (autoRangeEnabled_)
        {
            if (val.y() < min_y)
            {
                min_y = val.y();
            }

            if (val.y() > max_y)
            {
                max_y = val.y();
            }
        }
    }

    // Map the real CN0 data to the sparkline coordinate system.
    foreach (val, points)
    {
        double x_in = val.x();
        double y_in = val.y();

        double x_out = 0;
        double y_out = 0;

        if (!autoRangeEnabled_)
        {
            if (y_in > maxCn0_)
            {
                // Value is out of scale!
                outOfScale = true;

                // Exit the loop.
                break;
            }
        }

        x_out = sparklineWidth * (x_in - min_x) / (max_x - min_x);
        y_out = contentHeight - (contentHeight * (y_in - min_y) / (max_y - min_y));

        fpoints.append(QPointF(x_out, y_out));
    }

    QStyleOptionViewItem option_vi = option;
    QStyledItemDelegate::initStyleOption(&option_vi, index);

    painter->save();

    if (QApplication::style()->objectName().contains("vista"))
    {
        // QWindowsVistaStyle::drawControl does this internally. Unfortunately there
        // doesn't appear to be a more general way to do this.
        option_vi.palette.setColor(QPalette::All, QPalette::HighlightedText, option_vi.palette.color(QPalette::Active, QPalette::Text));
    }

    QPalette::ColorGroup cg = option_vi.state & QStyle::State_Enabled
                                  ? QPalette::Normal
                                  : QPalette::Disabled;
    if (cg == QPalette::Normal && !(option_vi.state & QStyle::State_Active))
        cg = QPalette::Inactive;
#if defined(Q_OS_WIN)
    if (option_vi.state & QStyle::State_Selected)
    {
#else
    if ((option_vi.state & QStyle::State_Selected) && !(option_vi.state & QStyle::State_MouseOver))
    {
#endif
        painter->setPen(option_vi.palette.color(cg, QPalette::HighlightedText));
    }
    else
    {
        painter->setPen(option_vi.palette.color(cg, QPalette::Text));
    }

    // Enable antialiasing.
    painter->setRenderHint(QPainter::Antialiasing, true);

    // Translate painting origin to cellOrigin.
    painter->translate(offset.x(), offset.y());

    if (outOfScale)
    {
        // Sprakline data is out of scale.
        // Draw an OUT OF SCALE notice in red color.
        painter->setPen(Qt::red);
        painter->drawRect(sparklineRect);
        painter->drawText(sparklineRect, Qt::AlignCenter, "OUT OF SCALE");
    }
    else
    {
        // Sprakline data is within the scale.
        // Draw the CN0 sparkline.

        // Translate painting origin to sparklineOrigin.
        painter->translate(hGap, vGap);

        // Fill area below the CN0 sparkline with a gradient.
        QPointF startPoint(fpoints.first().x(), contentHeight);
        QPointF endPoint(fpoints.last().x(), contentHeight);
        fpoints.push_front(startPoint);
        fpoints.push_back(endPoint);

        QLinearGradient gradient(QPointF(0, 0), QPointF(0, contentHeight));
        gradient.setColorAt(1, Qt::white);
        gradient.setColorAt(0, Qt::gray);

        painter->setBrush(QBrush(gradient));
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(QPolygonF(fpoints));

        // Draw CN0 sparkline.
        fpoints.removeFirst();
        fpoints.removeLast();
        painter->setPen(Qt::black);
        painter->drawPolyline(QPolygonF(fpoints));

        // Translate painting origin back to cellOrigin.
        painter->translate(-hGap, -vGap);
    }

    // Get the value of the last CN0 smple.
    double lastCN0 = var.last().toPointF().y();

    // If the value of the last CN0 sample is outside of the designated scale use red color otherwise use black.
    if (lastCN0 < minCn0_ || lastCN0 > maxCn0_)
    {
        painter->setPen(Qt::red);
    }
    else
    {
        painter->setPen(Qt::black);
    }

    // Display value of the last CN0 sample next to the sparkline.
    painter->drawText(textRect, QString::number(lastCN0, 'f', 1));

    // Draw visual guides for debugging.
    //drawGuides(painter, cellRect, sparklineRect, textRect);

    painter->restore();
}

QSize Cn0Delegate::sizeHint(const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
    return {option.fontMetrics.height() * SPARKLINE_MIN_EM_WIDTH, QStyledItemDelegate::sizeHint(option, index).height()};
}

/*!
  Draws a set of visual guides to assist in the debugging of the delegate design.
 */
void Cn0Delegate::drawGuides(QPainter *painter, QRect cellRect, QRect sparklineRect, QRect textRect) const
{
    // Set pen color to red.
    painter->setPen(Qt::red);
    painter->setBrush(Qt::NoBrush);

    // Draw cell border.
    painter->drawRect(cellRect);

    // Draw sparkline border.
    painter->drawRect(sparklineRect);

    // Draw text border.
    painter->drawRect(textRect);

    // Draw centerline in cyan.
    painter->setPen(Qt::cyan);
    painter->drawLine(cellRect.x(), cellRect.y() + cellRect.height() / 2,
        cellRect.x() + cellRect.width(), cellRect.y() + cellRect.height() / 2);

    // Create a new pen with increased width for drawing origins.
    QPen pen = QPen();
    pen.setWidth(5);

    // Draw cell origin.
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawPoint(cellRect.topLeft());

    // Draw sparkline origin.
    pen.setColor(Qt::green);
    painter->setPen(pen);
    painter->drawPoint(sparklineRect.topLeft());

    // Draw text origin.
    pen.setColor(Qt::blue);
    painter->setPen(pen);
    painter->drawPoint(textRect.topLeft());
}
