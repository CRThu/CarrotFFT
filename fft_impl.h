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
#define FFT_DEBUG_PRINT_START   (0)
#define FFT_DEBUG_PRINT_END     (FFT_DEBUG_PRINT_START + 32)

#if FFT_DEBUG_INFO
#include <stdio.h>
#endif

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

#define FFT_FFTN_ALL                (0)
#define FFT_FFTN_HALF               (1)

#define FFT_DB_V                    (0)
#define FFT_DB_P                    (1)

#define FFT_FFTN(FFT, HALF)         ((HALF) == FFT_FFTN_HALF ? (((FFT)->fftn >> 1) + 1) : (FFT)->fftn)

#if MATH_IMPL_TYPE == USE_STD_MATH  // math.h impl
#define _USE_MATH_DEFINES
#include <math.h>
#define MATH_PI                     (M_PI)
#define MATH_COS(ANGLE)             (cos(ANGLE))
#define MATH_SIN(ANGLE)             (sin(ANGLE))
#define MATH_ADD(A,B)               ((A) + (B))
#define MATH_SUB(A,B)               ((A) - (B))
#define MATH_MUL(A,B)               ((A) * (B))
#define MATH_DIV(A,B)               ((A) / (B))
#define MATH_SQRT(A)                (sqrt(A))
#define MATH_LOG2(A)                (log2(A))
#define MATH_LOG10(A)               (log10(A))

#endif

#define MATH_ABS(REZ, IMZ)          MATH_SQRT(MATH_ADD(MATH_MUL(REZ, REZ), MATH_MUL(IMZ, IMZ)))

#define ARR_OPER2_C(OP, ARR1, C, SIZE)   do {           \
    for (uint32_t _i = 0; _i < (SIZE); _i++)            \
    {                                                   \
        ARR1[_i] = OP(ARR1[_i], C);                     \
    }                                                   \
} while(0)

#define ARR_OPER2(OP, ARR1, ARR2, SIZE)   do {          \
    for (uint32_t _i = 0; _i < (SIZE); _i++)            \
    {                                                   \
        ARR1[_i] = OP(ARR1[_i], ARR2[_i]);              \
    }                                                   \
} while(0)

#define ARR_OPER1(OP, ARR1, SIZE)   do {                \
    for (uint32_t _i = 0; _i < (SIZE); _i++)            \
    {                                                   \
        ARR1[_i] = OP(ARR1[_i]);                        \
    }                                                   \
} while(0)


#define FFT_MAG(FFT, HALF)          ARR_OPER2(MATH_ABS, (FFT)->rez, (FFT)->imz, FFT_FFTN(FFT, HALF))
#define FFT_DB(ARR, FFTN, DBTYPE) do {                                          \
    ARR_OPER1(MATH_LOG10, ARR, FFTN);                                           \
    ARR_OPER2_C(MATH_MUL, ARR, ((DBTYPE) == FFT_DB_V ? 20.0 : 10.0), FFTN);     \
    } while (0)

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


#ifdef __cplusplus
}
#endif
#endif // _FFT_IMPL_H_