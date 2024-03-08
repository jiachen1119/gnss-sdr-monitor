#include "TabBarStyle.h"
#include <QPainter>
#include <QStyleOptionTab>
#include <QDebug>

TabBarStyle::TabBarStyle(Qt::Orientation orientation/* = Qt::Vertical*/)
    : QProxyStyle()
{
    orientation_ = orientation;
}

TabBarStyle::~TabBarStyle() = default;

void TabBarStyle::drawControl(QStyle::ControlElement element,
    const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{// <1> 此处element类型为：CE_TabBarTab,CE_TabBarTabShape,CE_TabBarTabLabel；
    // CE = Control Element
   // <2> 当element == CE_TabBarTabLabel时，QProxyStyle::drawControl函数会调用drawItemText函数，
   // <3> 由于drawItemText函数内得到的rect,并没有此处得到的controlRect容易理解
   // <4> 所以我们要重新实现drawItemText函数，并让该函数体为空，即不要让drawItemText函数绘制文本
   // <5> 而将绘制文本工作放在此处进行处理
	
    // 步骤一：调用父类的绘制控件函数
    QProxyStyle::drawControl(element, option, painter, widget);
	
    // 步骤二：重新绘制tab标签页文本
    if (element == CE_TabBarTabLabel) {
        if (const auto *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect controlRect = tab->rect;
			
            QString tabText;
            if (orientation_ == Qt::Vertical)
            {
                // 将文本字符串换行处理
                for (int i = 0; i < tab->text.length(); i++)
                {
                    tabText.append(tab->text.at(i));
                    tabText.append('\n');
                }
                if (tabText.length() > 1)
                    tabText = tabText.mid(0, tabText.length() - 1);
            }
            else
                tabText = tab->text;
		
            // 文本居中对齐
            QTextOption option;
            option.setAlignment(Qt::AlignCenter);
            QPen pen = painter->pen();
            pen.setColor(tab->palette.color(QPalette::WindowText));	// 文本颜色
            painter->setPen(pen);
            painter->drawText(controlRect, tabText, option);
        }
    }
}

void TabBarStyle::drawItemText(QPainter *painter, const QRect &rect, int flags, const QPalette &pal, bool enabled, const QString &text, QPalette::ColorRole textRole /*= QPalette::NoRole*/) const
{
    // 重写虚函数，但是函数体内什么都不用写，原因如下：
    // <1> 因为drawControl函数中得到的controlRect是整个tab标签页的大小（tab标签页指某一个选项卡页面，不是指整个tabBar）
    // <2> 而此处rect得到的不知道是什么大小
    // <3> 所以索性就在drawControl函数中绘制文本了（在drawControl函数中我们已经将字符串做换行处理并重新绘制文本了）
}

QSize TabBarStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget /*= nullptr*/) const
{
    QSize size = contentsSize;
    if (type == CT_TabBarTab)
    {
        if (orientation_ == Qt::Vertical)
        {
            size.rwidth() += 10;
            size.rheight() += 20;
        }
        else // orientation_ == Qt::Horizontal
        {
            size.transpose();//（tab页标签在WEST方向，并且文字水平横向排列时使用）
            size.rwidth() += 10;
            size.rheight() += 50;
        }
    }

    return size;
}