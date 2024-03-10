#include "TabBarStyle.h"

QSize TabBarStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
    const QSize &size, const QWidget *widget) const
{
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
    if (type == QStyle::CT_TabBarTab) {
        s.transpose();
        s.rwidth() = 90; // 设置每个tabBar中item的大小
        s.rheight() = 44;
    }
    return s;
}
void TabBarStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    //设置lab
    if (element == CE_TabBarTabLabel) {
        if (const auto *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect allRect = tab->rect;
            //选中状态
            if (tab->state & QStyle::State_Selected) {
                painter->save();
                painter->setPen(0xffffff);
                painter->setBrush(QBrush(0xffffff));
                //painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                painter->drawRect(allRect.adjusted(0, 0, 0, 0));
                painter->restore();
            }
            //hover状态 鼠标移动状态
            else if (tab->state & QStyle::State_MouseOver) {
                painter->save();
                painter->setPen(0xECECEC);//画框
                painter->setBrush(QBrush(0xECECEC));
                painter->drawRect(allRect.adjusted(0, 0, 0, 0));
                painter->restore();
            } else {
                painter->setPen(0x33CCFF);
            }
            //字体
            QTextOption option;
            option.setAlignment(Qt::AlignCenter);
            painter->setFont(QFont("楷体", 12, QFont::Bold));
            painter->setPen(0x0A0A0A);
            painter->drawText(allRect, tab->text, option);
            return;
        }
    }
    if (element == CE_TabBarTab) {
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}