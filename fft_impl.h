#pragma once
#ifndef _FFT_IMPL_H_
#define _FFT_IMPL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define FFT_IMPL_VERSION        "1.0.0"
#define FFT_DEBUG_INFO          (0)

#define FFT_TYPE_RFFT           (1)
#define FFT_TYPE_CFFT           (0) // TODO

    // CARROTFFT CONFIG
#define FFT_TYPE                FFT_TYPE_RFFT
#define FFT_DATA_TYPE           double
#define MATH_IMPL_TYPE          USE_STD_MATH

#if FFT_TYPE == FFT_TYPE_CFFT
#define wnm_SIZE(FFT)           ((FFT)->fftn >> 1 )
#elif FFT_TYPE == FFT_TYPE_RFFT
#define WNM_SIZE(FFT)           ((FFT)->fftn >> 2 )
#endif

#if MATH_IMPL_TYPE == USE_STD_MATH  // math.h impl
#define _USE_MATH_DEFINES
#include <math.h>
#define MATH_PI                     (M_PI)
#define MATH_COS(ANGLE)             (cos(ANGLE))
#define MATH_SIN(ANGLE)             (sin(ANGLE))
#define MATH_LOG2(FFTN)             ((uint32_t)log2(FFTN))
#define MATH_ADD(A,B)               ((A) + (B))
#define MATH_SUB(A,B)               ((A) - (B))
#define MATH_MUL(A,B)               ((A) * (B))
#define MATH_SQRT(A)                (sqrt(A))
#endif

    typedef FFT_DATA_TYPE fft_data_t;
    typedef struct fft_t
    {
        uint32_t fftn;

        fft_data_t* wnm_rez;
        fft_data_t* wnm_imz;

        fft_data_t* rez;
        fft_data_t* imz;
    }fft_t;

    fft_t* fft_init(uint32_t n);
    void fft_deinit(fft_t* fft);
    void fft_wnm_set(fft_t* fft);
    void fft_wnm_get(fft_t* fft, uint32_t idx, fft_data_t* rez, fft_data_t* imz);
    void fft_invert(fft_t* fft);
    void fft_calc(fft_t* fft, fft_data_t* rez, fft_data_t* imz);
    void fft_mag(fft_t* fft);



#ifdef __cplusplus
}
#endif
#endif // _FFT_IMPL_H_