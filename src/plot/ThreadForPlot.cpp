//
// Created by tang on 24-4-15.
//

#include "ThreadForPlot.h"

ThreadForPlot::ThreadForPlot(QObject *parent) : QThread(parent)
{
}

void ThreadForPlot::run()
{
    while (!thread_stop_)
    {
        // do something
    }
}