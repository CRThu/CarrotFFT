#pragma once
#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "fft_impl.h"

#define ANALYSIS_VERSION            "1.0.0"

#define ANALYSIS_HD_FREQ_SEARCH_BINS    (5)     

#define ANALYSIS_DEBUG_LOG_INFO     (1)
#define ANALYSIS_DEBUG_DATA_INFO    (1)
#define ANALYSIS_DEBUG_PRINT_START  (0)
#define ANALYSIS_DEBUG_PRINT_END    (FFT_DEBUG_PRINT_START + 32)

#if ANALYSIS_DEBUG_LOG_INFO || ANALYSIS_DEBUG_DATA_INFO
#include <stdio.h>
#endif

#define INDEX_CHECK(IDX, SIZE)          ((IDX) < 0) ? 0 : ((IDX) >= (SIZE) ? (SIZE) - 1 : (IDX))
#define INDEX_TO_FREQ(IDX, FFTN, FS)    (1.0 * IDX * (FS) / (FFTN))

    typedef struct window_t
    {
        char* name;
        /// <summary>
        /// window coeff
        /// </summary>
        fft_data_t* coeff;
        uint8_t coeff_size;
        uint8_t mainlobe_bins;
        uint8_t hdlobe_bins;
    } window_t;

    typedef struct analysis_report_t
    {
        double fsignal;
        double fsignal_db;

        double* fhd;
        double* fhd_db;

        double fspur;
        double fspur_db;

        double snr;
        double thd;
        double sfdr;
        double sinad;
        double enob;
    } analysis_report_t;

    typedef struct analysis_t
    {
        /// <summary>
        /// n: fft->fftn
        /// data: fft->rez
        /// </summary>
        fft_t* fft;

        /// <summary>
        /// fft window
        /// </summary>
        window_t* win;

        /// <summary>
        /// sample frequency
        /// </summary>
        double fs;

        /// <summary>
        /// calculated max distortion
        /// </summary>
        uint8_t hdn;

        /// <summary>
        /// windows data
        /// </summary>
        fft_data_t* win_data;
    } analysis_t;

    extern const window_t windows[];
    window_t* get_window_by_name(const char* name);
    analysis_t* analysis_init(uint32_t fftn, double fs, window_t* win, uint8_t hdn);
    void analysis_deinit(analysis_t* analysis);
    void window_calc(analysis_t* analysis);
    void analysis_run(analysis_t* analysis, fft_data_t* tdata, fft_data_t* timg);


#ifdef __cplusplus
}
#endif
#endif // _ANALYSIS_H_