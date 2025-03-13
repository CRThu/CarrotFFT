#include "analysis.h"

#include <malloc.h>
#include <string.h>

const window_t windows[] =
{
     {
        .name = "HFT144D",
        .coeff = (fft_data_t[]) {1, 1.96760033, 1.57983607, 0.81123644,0.22583558,0.02773848,0.00090360},
        .coeff_size = 7,
        .mainlobe_bins = 7,
        .hdlobe_bins = 5,
    },
};

window_t* get_window_by_name(const char* name)
{
    for (int i = 0; i < sizeof(windows) / sizeof(window_t); i++)
    {
        if (strcmp(windows->name, name) == 0)
            return &windows[i];
    }
}

analysis_t* analysis_init(uint32_t fftn, double fs, window_t* win, uint8_t hdn)
{
    if (win == NULL)
        return NULL;

    analysis_t* analysis = malloc(sizeof(analysis_t));
    if (analysis == NULL)
        return NULL;

    analysis->fs = fs;
    analysis->win = win;
    analysis->hdn = hdn;

    analysis->fft = fft_init(fftn);
    if (analysis->fft == NULL)
    {
        analysis_deinit(analysis);
        return NULL;
    }

    analysis->win_data = malloc(sizeof(fft_data_t) * fftn);
    if (analysis->win_data == NULL)
    {
        analysis_deinit(analysis);
        return NULL;
    }

    // 预生成
    window_calc(analysis);

    return analysis;
}

void analysis_deinit(analysis_t* analysis)
{
    fft_deinit(analysis->fft);
    free(analysis->win_data);
    free(analysis);
    analysis = NULL;
}

void window_calc(analysis_t* analysis)
{
    memset(analysis->win_data, 0, sizeof(fft_data_t) * analysis->fft->fftn);
    for (int i = 0; i < analysis->fft->fftn; i++)
    {
        for (int j = 0; j < analysis->win->coeff_size; j++)
        {
            if (j % 2 == 0)
                analysis->win_data[i] += MATH_MUL(analysis->win->coeff[j], MATH_COS((fft_data_t)2.0 * MATH_PI * j * i / (fft_data_t)analysis->fft->fftn));
            else
                analysis->win_data[i] -= MATH_MUL(analysis->win->coeff[j], MATH_COS((fft_data_t)2.0 * MATH_PI * j * i / (fft_data_t)analysis->fft->fftn));
        }
#if ANALYSIS_DEBUG_INFO == 1
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("win[%6d]: %.12f\n", i, analysis->win_data[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
#endif
    }
}

void analysis_run(analysis_t* analysis, fft_data_t* tdata, fft_data_t* timg)
{
#if ANALYSIS_DEBUG_INFO == 1
    for (int i = 0; i < analysis->fft->fftn; i++)
    {
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("tdata[%6d]: %.12f\n", i, tdata[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
    }
#endif
    // tdata_win = tdata .* windata;
    ARR_OPER2(MATH_MUL, tdata, analysis->win_data, analysis->fft->fftn);
#if ANALYSIS_DEBUG_INFO == 1
    for (int i = 0; i < analysis->fft->fftn; i++)
    {
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("tdata_win[%6d]: %.12f\n", i, tdata[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
    }
#endif
    // fdatay_c = fft(tdata_win, fftn);
    fft_calc(analysis->fft, tdata, timg);
    // fdatay_r = abs(fdatay_c_half);
    FFT_MAG(analysis->fft, FFT_FFTN_HALF);
#if ANALYSIS_DEBUG_INFO == 1
    for (int i = 0; i < FFT_FFTN(analysis->fft, FFT_FFTN_HALF); i++)
    {
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("fdatay_r[%6d]: %.12f\n", i, analysis->fft->rez[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
    }
#endif

    // fdatay_r_db = 20 * log10(fdatay_r);
    FFT_DB(analysis->fft->rez, FFT_FFTN(analysis->fft, FFT_FFTN_HALF), FFT_DB_V);
#if ANALYSIS_DEBUG_INFO == 1
    for (int i = 0; i < FFT_FFTN(analysis->fft, FFT_FFTN_HALF); i++)
    {
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("fdatay_r_db[%6d]: %.12f\n", i, analysis->fft->rez[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
    }
#endif
}