//
// Created by tang on 24-3-18.
//

#ifndef GNSS_SDR_MONITOR_VARIANCE_H
#define GNSS_SDR_MONITOR_VARIANCE_H

// 这里的buffer size 主要根据接收机输出的跟踪数据频率来决定的
// 输出频率默认为20ms，这里放大了五倍，也就是100ms输出一次
// 如果size为1000，也就是绘图将会展示100s的数据
#define BUFFER_SIZE_FOR_CHANNEL 600

// PVT的输出频率为100ms,为了展示10s的路径和位置信息，size设置为100
#define BUFFER_SIZE_FOR_PVT 100

#endif  // GNSS_SDR_MONITOR_VARIANCE_H
