//
// Created by tang on 24-4-13.
//

#include "CustomBar.h"

CustomBar::CustomBar(QWidget *parent) : QToolBar(parent)
{
    label_tab_name_ = new QLabel(QStringLiteral("CHANNEL"),this);
    label_tab_name_->setFont(QFont("Calibri",15,QFont::Bold));
    label_project_ = new QLabel(QStringLiteral("Tang"),this);

    this->setMovable(false);
    this->addWidget(label_tab_name_);
    this->addWidget(label_project_);
    QSizePolicy policy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    this->setSizePolicy(policy);
    this->setMinimumHeight(20);

}

void CustomBar::resizeEvent(QResizeEvent *event)
{
    QToolBar::resizeEvent(event);
    label_tab_name_->setFont(QFont("Andale Mono",this->height()/2,QFont::Black));
}
