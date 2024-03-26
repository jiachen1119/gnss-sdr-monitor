#include "TabBarStyle.h"

QSize TabBarStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
    const QSize &size, const QWidget *widget) const
{
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
    if (type == QStyle::CT_TabBarTab) {
        s.transpose();
        s.rwidth() = 70 + 15 + 15; // 设置每个tabBar中item的大小 rwidth是可以修改的
        s.rheight() = 90 + 15 + 10 + 5;
    }
    return s;
}
void TabBarStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    //设置label
    if (element == CE_TabBarTabLabel) {
        if (const auto *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            // 获取这个tabbar的矩形区域范围
            QRect all_rect = tab->rect;

            // 字体
            QTextOption text_option;
            text_option.setAlignment(Qt::AlignHCenter|Qt::AlignBottom);

            // 选中状态 使用位运算的原因是 可能状态有多个，只要存在被选中就可以了
            if (tab->state & QStyle::State_Selected) {
                // 保存之前的状态 以便使用restore恢复
                painter->setPen(0xffffff);
                painter->setBrush(QBrush(0xffffff));
                painter->drawRect(all_rect.adjusted(0, 0, 0, 0));

                painter->setFont(QFont("", 13, QFont::Bold));
                painter->setPen(0x720320);
                painter->drawText(all_rect.adjusted(0,0,0,-5), tab->text, text_option);
            }

            //hover状态 鼠标移动状态
            else if (tab->state & QStyle::State_MouseOver) {
                auto current_color = tab->palette.window().color().lighter(150);
                painter->setBrush(current_color);
                painter->setPen(Qt::transparent);
                painter->drawRect(all_rect.adjusted(0, 0, 0, 0));

                painter->setFont(QFont("", 13, QFont::Bold));
                painter->setPen(0xFFFFFF);
                painter->drawText(all_rect.adjusted(0,0,0,-5), tab->text, text_option);
            }

            else {
                painter->setFont(QFont("", 10, QFont::Bold));
                painter->setPen(0xFFFFFF);
                painter->drawText(all_rect.adjusted(0,0,0,-5), tab->text, text_option);
            }

            // icon
            auto icon = tab->icon;
            QRect icon_rect = all_rect.adjusted(20,25,-20,-35);
            painter->drawPixmap(icon_rect,icon.pixmap(QSize(500,500)));

            return;
        }
    }
    if (element == CE_TabBarTab) {
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}