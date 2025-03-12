#include "fft_impl.h"
#include <malloc.h>

/// <summary>
/// 
/// </summary>
/// <param name="n">n=2^z</param>
fft_t* fft_init(uint32_t n)
{
    // 2^N and n >= 4 check
    if (n < 4 && !(n & (n - 1)))
        return NULL;

    fft_t* fft = malloc(sizeof(fft_t));
    if (fft == NULL)
        return NULL;

    fft->fftn = n;
    fft->wmn_real = malloc(WMN_SIZE(fft) * sizeof(fft_data_t));
    fft->wmn_img = malloc(WMN_SIZE(fft) * sizeof(fft_data_t));
    if (fft->wmn_real == NULL || fft->wmn_img == NULL)
    {
        fft_deinit(fft);
        return NULL;
    }

    fft_pwmn(fft);
}

void fft_deinit(fft_t* fft)
{
    free(fft->wmn_real);
    free(fft->wmn_img);
    free(fft);
    fft = NULL;
}

void fft_pwmn(fft_t* fft)
{
    for (uint32_t m = 0; m < WMN_SIZE(fft); m++)
    {
        fft->wmn_real[m] = MATH_COS((double)2.0 * MATH_PI * (double)m / (double)fft->fftn);
        fft->wmn_img[m] = -MATH_SIN((double)2.0 * MATH_PI * (double)m / (double)fft->fftn);
    }
}

void rfft_calc(fft_t* fft, fft_data_t* real, fft_data_t* img)
{
}
