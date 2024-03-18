//
// Created by tang on 24-3-18.
//
#pragma once
#ifndef GNSS_SDR_MONITOR_PVTSTRUCT_H
#define GNSS_SDR_MONITOR_PVTSTRUCT_H

#include <QVariant>
#include "monitor_pvt.pb.h"

typedef struct PVTStruct {
    uint32_t tow;
    uint32_t week;
    double rx_time;
    double clock_offset;

    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz;

    double cov_xx;
    double cov_yy;
    double cov_zz;
    double cov_xy;
    double cov_yz;
    double cov_zx;

    double latitude;
    double longitude;
    double height;

    uint32_t valid_satellite;
    uint32_t solution_status;
    uint32_t solution_type;
    float ambiguity_ratio_factor;
    float ambiguity_ratio_threshold;

    double gdop;
    double pdop;
    double hdop;
    double vdop;

}PVTStruct;

#endif  // GNSS_SDR_MONITOR_PVTSTRUCT_H
