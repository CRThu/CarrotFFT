#pragma once
#ifndef _FFT_IMPL_H_
#define _FFT_IMPL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define FFT_IMPL_VERSION "1.0.0"

#define FFT_TYPE_RFFT           (1)
#define FFT_TYPE_CFFT           (0) // TODO

// CARROTFFT CONFIG
#define FFT_TYPE                FFT_TYPE_RFFT
#define MATH_IMPL_TYPE          USE_STD_MATH

#if FFT_TYPE == FFT_TYPE_CFFT
#define WMN_SIZE(FFT)           ((FFT)->fftn / 2 )
#define WMN_GET(FFT, IDX)       ()
#elif FFT_TYPE == FFT_TYPE_RFFT
#define WMN_SIZE(FFT)           ((FFT)->fftn / 4 )
#define WMN_GET(FFT, IDX)       ()
#endif

#if MATH_IMPL_TYPE == USE_STD_MATH  // math.h impl
#define _USE_MATH_DEFINES
#include <math.h>
#define MATH_PI                     (M_PI)
#define MATH_COS(ANGLE)             (cos(ANGLE))
#define MATH_SIN(ANGLE)             (sin(ANGLE))
#endif

    typedef double fft_data_t;
    typedef struct fft_t
    {
        uint32_t fftn;

        fft_data_t* wmn_real;
        fft_data_t* wmn_img;

        //fft_data_t* real;
        //fft_data_t* img;
    }fft_t;

    fft_t* fft_init(uint32_t n);
    void fft_deinit(fft_t* fft);
    void rfft_calc(fft_t* fft, fft_data_t* real, fft_data_t* img);




#ifdef __cplusplus
}
#endif
#endif // _FFT_IMPL_H_