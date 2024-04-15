//
// Created by tang on 24-4-13.
//

#include "CustomToolBar.h"
#include "Variance.h"

CustomToolBar::CustomToolBar(QWidget *parent) : QToolBar(parent)
{
    label_tab_name_ = new QLabel(QStringLiteral("CHANNEL"),this);
    this->addSeparator();
    label_project_ = new QLabel(QStringLiteral("Tang"),this);

    // Create a vertical line (QFrame) as a separator
    line_ = new QFrame(this);
    line_->setFrameShape(QFrame::VLine);
    line_->setFrameShadow(QFrame::Sunken);
    line_->setLineWidth(0);
    line_->setMidLineWidth(5);
    line_->setPalette(QPalette(QColor(0x720320)));

    spacer_ = new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum);

    QPalette palette;
    palette.setColor(QPalette::ButtonText, QColor(0x720320));
    label_tab_name_->setPalette(palette);

    this->setMovable(false);
    this->addWidget(label_tab_name_);
    this->addWidget(line_);
    this->addWidget(label_project_);
    QSizePolicy policy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    this->setSizePolicy(policy);
    this->setMinimumHeight(20);

}

void CustomToolBar::resizeEvent(QResizeEvent *event)
{
    QToolBar::resizeEvent(event);
    label_tab_name_->setFont(QFont("Archivo",this->height()/2,QFont::DemiBold));
    label_tab_name_->setContentsMargins(this->height()/8,0,this->height()/4,0);
}

void CustomToolBar::setTabName(const int &index)
{
    switch (index) {
        case TAB_CHANNEL:
            label_tab_name_->setText(QStringLiteral("CHANNEL"));
            break;
        case TAB_SOLUTION:
            label_tab_name_->setText(QStringLiteral("SOLUTION"));
            break;
        case TAB_ALERT:
            label_tab_name_->setText(QStringLiteral("ALERT"));
            break;
        case TAB_SETTINGS:
            label_tab_name_->setText(QStringLiteral("SETTINGS"));
            break;
        case TAB_ABOUT:
            label_tab_name_->setText(QStringLiteral("ABOUT"));
            break;
        default:
            break;
    }
}