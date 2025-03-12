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
    fft->wnm_rez = malloc(wnm_SIZE(fft) * sizeof(fft_data_t));
    fft->wnm_imz = malloc(wnm_SIZE(fft) * sizeof(fft_data_t));
    if (fft->wnm_rez == NULL || fft->wnm_imz == NULL)
    {
        fft_deinit(fft);
        return NULL;
    }

    fft_wnm_set(fft);

    return fft;
}

void fft_deinit(fft_t* fft)
{
    free(fft->wnm_rez);
    free(fft->wnm_imz);
    free(fft);
    fft = NULL;
}

void fft_wnm_set(fft_t* fft)
{
    for (uint32_t m = 0; m < wnm_SIZE(fft); m++)
    {
        fft->wnm_rez[m] = MATH_COS((double)2.0 * MATH_PI * (double)m / (double)fft->fftn);
        fft->wnm_imz[m] = -MATH_SIN((double)2.0 * MATH_PI * (double)m / (double)fft->fftn);

#if FFT_DEBUG_INFO == 1
        printf("wnm[%d]: %f + %fi\n", m, fft->wnm_rez[m], fft->wnm_imz[m]);
#endif
    }
}

void fft_wnm_get(fft_t* fft, uint32_t idx, fft_data_t* rez, fft_data_t* imz)
{
    uint32_t r = idx / wnm_SIZE(fft);
    idx = idx % wnm_SIZE(fft);
    if (r == 0)
    {
        // +Rez, +Imz
        *rez = +fft->wnm_rez[idx];
        *imz = +fft->wnm_imz[idx];
    }
    else if (r == 1)
    {
        //  +Imz, -Rez
        *rez = +fft->wnm_imz[idx];
        *imz = -fft->wnm_rez[idx];
    }
    else if (r == 2)
    {
        // -Rez, -Imz
        *rez = -fft->wnm_rez[idx];
        *imz = -fft->wnm_imz[idx];
    }
    else
    {
        //  -Imz, +Rez
        *rez = -fft->wnm_imz[idx];
        *imz = +fft->wnm_rez[idx];
    }
}

void fft_invert(fft_t* fft)
{
    uint32_t k = 0;
    uint32_t j = 0;

    fft_data_t swap_rez;
    fft_data_t swap_imz;

    for (uint32_t i = 0; i < fft->fftn - 1; i++)
    {
        if (i < j)
        {
            swap_rez = fft->rez[i];
            swap_imz = fft->imz[i];

            fft->rez[i] = fft->rez[j];
            fft->imz[i] = fft->imz[j];

            fft->rez[j] = swap_rez;
            fft->imz[j] = swap_imz;

#if FFT_DEBUG_INFO == 1
            printf("[invert]: [%d] <-> [%d]\n", i, j);
#endif
        }
        // K = 100..00
        k = fft->fftn / 2;
        while (j >= k)
        {
            j -= k;
            k /= 2;
        }
        j += k;
    }
}
void fft_calc(fft_t* fft, fft_data_t* rez, fft_data_t* imz)
{
    fft->rez = rez;
    fft->imz = imz;

#if FFT_DEBUG_INFO == 1
    for (uint32_t i = 0; i < fft->fftn; i++)
    {
        printf("t[%d]: %f + %fi\n", i, fft->rez[i], fft->imz[i]);
    }
#endif

    fft_invert(fft);

    // TODO : can be optimized in loop[J]
    uint32_t B, P;

    fft_data_t wnm_rez;
    fft_data_t wnm_imz;
    fft_data_t temp_rez;
    fft_data_t temp_imz;
    for (uint16_t L = 1; L <= MATH_LOG2(fft->fftn); L++)                // L = 1:M
    {
        B = pow(2, L - 1);                                              // B = 2^(L-1)
        for (uint32_t J = 0; J < B; J++)                                // j = 0:B-1
        {
            P = pow(2, MATH_LOG2(fft->fftn) - L) * J;                   // P = 2^(M-L)*J
            fft_wnm_get(fft, P, &wnm_rez, &wnm_imz);
            for (uint32_t K = J; K <= fft->fftn - 1; K += pow(2, L))    // K = J:N-1:2^L
            {
                // data[K+B] * wnm[P]
                temp_rez = fft->rez[K + B] * wnm_rez - fft->imz[K + B] * wnm_imz;
                temp_imz = fft->rez[K + B] * wnm_imz + fft->imz[K + B] * wnm_rez;

                // data[K+B] = data[K] - data[K+B] * wnm[P]
                fft->rez[K + B] = fft->rez[K] - temp_rez;
                fft->imz[K + B] = fft->imz[K] - temp_imz;

                // data[K] = data[K] + data[K+B] * wnm[P]
                fft->rez[K] = fft->rez[K] + temp_rez;
                fft->imz[K] = fft->imz[K] + temp_imz;
            }
        }
    }

#if FFT_DEBUG_INFO == 1
    for (uint32_t i = 0; i < fft->fftn; i++)
    {
        double magnitude = sqrt(fft->rez[i] * fft->rez[i] + fft->imz[i] * fft->imz[i]);

        printf("k[%d]: %f + %fi (mag = %f)\n", i, fft->rez[i], fft->imz[i], magnitude);
    }
#endif
}

void fft_mag(fft_t* fft)
{
    for (uint32_t i = 0; i < fft->fftn; i++)
    {
        fft->rez[i] = sqrt(fft->rez[i] * fft->rez[i] + fft->imz[i] * fft->imz[i]);
    }
}