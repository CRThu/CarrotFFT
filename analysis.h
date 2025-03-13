#pragma once
#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "fft_impl.h"

#define ANALYSIS_VERSION     "1.0.0"
#define ANALYSIS_DEBUG_INFO  (1)

#define ARR_MUL_C(ARR1, C, SIZE)   do {                 \
    for (int _i = 0; _i < (SIZE); _i++)                 \
    {                                                   \
        ARR1[_i] = MATH_MUL(ARR1[_i], C);               \
    }                                                   \
} while(0)

#define ARR_MUL(ARR1, ARR2, SIZE)   do {                \
    for (int _i = 0; _i < (SIZE); _i++)                 \
    {                                                   \
        ARR1[_i] = MATH_MUL(ARR1[_i], ARR2[_i]);        \
    }                                                   \
} while(0)

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


#ifdef __cplusplus
}
#endif
#endif // _ANALYSIS_H_