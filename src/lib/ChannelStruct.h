//
// Created by tang on 24-3-26.
//
#pragma once
#ifndef GNSS_SDR_MONITOR_CHANNELSTRUCT_H
#define GNSS_SDR_MONITOR_CHANNELSTRUCT_H

#include <string>

using std::string;
typedef uint64_t uint64;
typedef uint32_t uint32;
typedef int32_t int32;
typedef int64_t int64;

typedef struct ChannelStruct {
    string system; // GNSS constellation: "G" for GPS, "R" for Glonass, "S" for SBAS, "E" for Galileo and "C" for Beidou.
    // GNSS signal: "1C" for GPS L1 C/A, "1B" for Galileo E1b/c, "1G" for Glonass L1 C/A,
    // "2S" for GPS L2 L2C(M), "2G" for Glonass L2 C/A, "L5" for GPS L5 and "5X" for Galileo E5a
    string signal;

    uint32 prn;
    int32 channel_id;

    double acq_delay_samples; // Coarse code delay estimation, in samples
    double acq_doppler_hz;  // Coarse Doppler estimation in each channel, in Hz
    uint64 acq_samplestamp_samples;  // Number of samples at signal SampleStamp
    uint32 acq_doppler_step;  // Step of the frequency bin in the search grid, in Hz
    bool flag_valid_acquisition;  // Acquisition status

    int64 fs;  // Sampling frequency, in samples per second
    double prompt_i;  // In-phase (real) component of the prompt correlator output
    double prompt_q;  // Quadrature (imaginary) component of the prompt correlator output
    double cn0_db_hz;  // Carrier-to-Noise density ratio, in dB-Hz
    double carrier_doppler_hz ;  // Doppler estimation, in [Hz].
    double carrier_phase_rads;  // Carrier phase estimation, in rad
    double code_phase_samples;  // Code phase in samples
    uint64 tracking_sample_counter;  // Sample counter indicating the number of processed samples
    bool flag_valid_symbol_output;  // Indicates the validity of signal tracking
    int32 correlation_length_ms;  // Time duration of coherent correlation integration, in ms

    bool flag_valid_word;  // Indicates the validity of the decoded navigation message word
    uint32 tow_at_current_symbol_ms;  // Time of week of the current symbol, in ms

    double pseudorange_m;  // Pseudorange computation, in m
    double rx_time;  // Receiving time after the start of the week, in s
    bool flag_valid_pseudorange;  // Pseudorange computation status
    double interp_tow_ms;  // Interpolated time of week, in ms


}ChannelStruct;


#endif  // GNSS_SDR_MONITOR_CHANNELSTRUCT_H
