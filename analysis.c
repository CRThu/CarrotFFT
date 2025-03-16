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
            return (window_t*)&windows[i];
    }
    return NULL;
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

    // generate window wave
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
    for (uint32_t i = 0; i < analysis->fft->fftn; i++)
    {
        for (uint32_t j = 0; j < analysis->win->coeff_size; j++)
        {
            if (j % 2 == 0)
                analysis->win_data[i] += MATH_MUL(analysis->win->coeff[j], MATH_COS((fft_data_t)2.0 * MATH_PI * j * i / (fft_data_t)analysis->fft->fftn));
            else
                analysis->win_data[i] -= MATH_MUL(analysis->win->coeff[j], MATH_COS((fft_data_t)2.0 * MATH_PI * j * i / (fft_data_t)analysis->fft->fftn));
        }
#if ANALYSIS_DEBUG_DATA_INFO == 1
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("win[%6d]: %.12f\n", i, analysis->win_data[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
#endif
    }
}

void analysis_search_max(fft_data_t* data, uint32_t start_idx, uint32_t end_idx, uint32_t* max_idx, fft_data_t* max_val)
{
    *max_idx = 0;
    *max_val = data[0];

    for (uint32_t i = start_idx; i <= end_idx; i++)
    {
        if (data[i] > *max_val)
        {
            *max_idx = i;
            *max_val = data[i];
        }
    }
}

void analysis_run(analysis_t* analysis, fft_data_t* tdata, fft_data_t* timg)
{
    uint32_t fftn_half = FFT_FFTN(analysis->fft, FFT_FFTN_HALF);

#if ANALYSIS_DEBUG_DATA_INFO == 1
    for (uint32_t i = 0; i < analysis->fft->fftn; i++)
    {
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("tdata[%6d]: %.12f\n", i, tdata[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
    }
#endif

    // tdata_win = tdata .* windata;
    ARR_OPER2(MATH_MUL, tdata, analysis->win_data, analysis->fft->fftn);
#if ANALYSIS_DEBUG_DATA_INFO == 1
    for (uint32_t i = 0; i < analysis->fft->fftn; i++)
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
#if ANALYSIS_DEBUG_DATA_INFO == 1
    for (uint32_t i = 0; i < FFT_FFTN(analysis->fft, FFT_FFTN_HALF); i++)
    {
        if (i >= ANALYSIS_DEBUG_PRINT_START && i < ANALYSIS_DEBUG_PRINT_END)
            printf("fdatay_r[%6d]: %.12f\n", i, analysis->fft->rez[i]);
        else if (i == ANALYSIS_DEBUG_PRINT_END)
            printf("... \n");
    }
#endif

    // fdata_fdc_lr_idx = [ 1 ; 1 + win_mainlobe ];
    // fdatay_r_max = max(fdatay_r(fdata_fdc_lr_idx(2) : fftn / 2 + 1));
    // fdata_fbase_idx = find(fdatay_r == fdatay_r_db_max);
    uint32_t fsignal_idx;
    fft_data_t fsignal_max;

    analysis_search_max(analysis->fft->rez,
        INDEX_CHECK(analysis->win->mainlobe_bins, fftn_half),
        INDEX_CHECK(fftn_half - 1, fftn_half),
        &fsignal_idx,
        &fsignal_max);

    // fdata_fbase_f = fdatax(fdata_fbase_idx);
    double fsignal = INDEX_TO_FREQ(fsignal_idx, analysis->fft->fftn, analysis->fs);

#if ANALYSIS_DEBUG_LOG_INFO == 1
    printf("fsignal: %d %f\n", fsignal_idx, fsignal);
#endif

    // % fhd search
    // fdata_fhd_n = 2 : 1 : fhdn;
    for (int hdi = 2; hdi <= analysis->hdn; hdi++)
    {
        // fdata_fhd_calc_idx = fdata_fhd_n * (fdata_fbase_idx - 1) + 1;
        uint16_t fhd_idx_est = hdi * fsignal_idx;
        // fdata_fhd_search_lr_idx = [ fdata_fhd_calc_idx - fhd_search_bin; fdata_fhd_calc_idx + fhd_search_bin ]';
        // fdata_fhd_search_max(i) = max(fdatay_r_db_norm(fdata_fhd_search_lr_idx(i, 1) : fdata_fhd_search_lr_idx(i, 2)));
        // fdata_fhd_search_idx(i) = find(fdatay_r_db_norm == fdata_fhd_search_max(i));

        uint32_t fhdi_idx;
        double fhdi_db;
        analysis_search_max(analysis->fft->rez,
            INDEX_CHECK(fhd_idx_est - ANALYSIS_HD_FREQ_SEARCH_BINS, fftn_half),
            INDEX_CHECK(fhd_idx_est + ANALYSIS_HD_FREQ_SEARCH_BINS, fftn_half),
            &fhdi_idx,
            &fhdi_db);

        // fdata_fhd_f = fdatax(fdata_fhd_search_idx);
        double fhdi = INDEX_TO_FREQ(fhdi_idx, analysis->fft->fftn, analysis->fs);


#if ANALYSIS_DEBUG_LOG_INFO == 1
        printf("fhd[%d]: %d %f\n", hdi, fhdi_idx, fhdi);
#endif

    }

}