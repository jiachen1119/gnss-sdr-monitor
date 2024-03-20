//
// Created by tang on 24-3-18.
//

#ifndef GNSS_SDR_MONITOR_VARIANCE_H
#define GNSS_SDR_MONITOR_VARIANCE_H

// 这里的buffer size 主要根据接收机输出的跟踪数据频率来决定的
// 输出频率默认为20ms，这里放大了五倍，也就是100ms输出一次
// 如果size为1000，也就是绘图将会展示100s的数据
#define BUFFER_SIZE_FOR_CHANNEL 600
#define TIMER_MS_FOR_CHANNEL 500

// PVT的输出频率为100ms,为了展示10s的路径和位置信息，size设置为100
#define BUFFER_SIZE_FOR_PVT 100

// 定义table column输出的显示
#define CHANNEL_ID              0
#define CHANNEL_SYSTEM          1
#define CHANNEL_PRN             2
#define CHANNEL_ACQ_DOPPLER     3
#define CHANNEL_CONSTELLATION   4
#define CHANNEL_CN0             5
#define CHANNEL_DOPPLER         6
#define CHANNEL_TOW             7
#define CHANNEL_TLM             8
#define CHANNEL_PSEUDORANGE     9

// 定义tab bar的页面显示
#define TAB_DASHBORAD   0
#define TAB_SOLUTION    1
#define TAB_ALARM       2
#define TAB_SETTINGS    3
#define TAB_ABOUT       4

#endif  // GNSS_SDR_MONITOR_VARIANCE_H
