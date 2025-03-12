#include "timer_wrapper.h"
#include "complex.h"
#include "FFT.h"

#include <iostream>
#include "load_data.hpp"
#include "fft_impl.h"

void FftProc(complex_t* data)
{
    // FFT CALC
    InvertedArray(data);
    FFTCalc(data);
}

/*
#define GEN_DATA_FS         (200000)
#define GEN_DATA_FIN        (921.63)

#define FFT_PERF_MINN       (256)
#define FFT_PERF_MAXN       (1048576)
#define FFT_PERF_STEP       (2)

#define FFT_VERIFY_FFTN     (256)
complex_t* tdata[FFT_VERIFY_FFTN] = {};
complex_t* fdata[FFT_VERIFY_FFTN] = {};

void GenTestData(complex_t* sine, uint32_t* len)
{
    for (uint32_t i = 0; i < len; i++)
    {
        sine[i] = CreateComplex(sin((double)2.0 * PI * (double)i * (double)GEN_DATA_FIN) / (double)GEN_DATA_FS, 0);
    }
}


void perf_test()
{
    printf("|\tFFT N |\tINIT Elapsed |\tCALC Elapsed |\tMemory Usage |\n");
    printf("|\t----- |\t------------ |\t------------ |\t------------ |\n");

    for (uint32_t fftn = FFT_PERF_MINN; fftn <= FFT_PERF_MAXN; fftn *= FFT_PERF_STEP)
    {
        complex_t* data = malloc(sizeof(complex_t) * fftn);
        if (data == NULL)
        {
            printf("[Error]: No enough memory.\r\n");
            return;
        }
        memset(data, 0, sizeof(complex_t) * fftn);
        GenTestData(data, fftn);

        double InitElapsedTime = timed_exec(FFT_Init, &fftn);
        double CalcElapsedTime = timed_exec(FftProc, data);

        printf("|%12ld |\t%9.3lf ms |\t%9.3lf ms |\t%9ld KB |\n", fftn, InitElapsedTime / 1000.0, CalcElapsedTime / 1000.0, 0);

        // FREE
        FFT_DeInit();
        free(data);
        data = NULL;
    }
}

void verify_test()
{
    complex_t* data = malloc(sizeof(complex_t) * FFT_VERIFY_FFTN);
    if (data == NULL)
    {
        printf("[Error]: No enough memory.\r\n");
        return;
    }
    memset(data, 0, sizeof(complex_t) * FFT_VERIFY_FFTN);
    GenTestData(data, FFT_VERIFY_FFTN);

    uint32_t fftn = FFT_VERIFY_FFTN;
    FFT_Init(&fftn);

    // WNm For Debug
    PrintWNm();

    FftProc(data);

    // WNm For Debug
    for (uint32_t i = 0; i < fftn; i++)
    {
        printf("[FFT k = %8d; X(k) = ", i);
        PrintComplex(data[i]);
        printf(" (Mod = %.6f)]\n", ModComplex(data[i]));
    }

    // FREE
    FFT_DeInit();
    free(data);
    data = NULL;
}
*/
int main()
{
    printf("CARROT-FFT-LIBRARY TESTBENCH @ https://github.com/CRThu \n");

    std::vector<double> tdata_vec = load_data("testdata/fft@65536pt,120db,-124db", "tdata_win");
    uint32_t tdata_size = tdata_vec.size();
    std::cout << "load data, size = " << tdata_size << std::endl;

    double* real = new double[tdata_size];
    double* img = new double[tdata_size];
    std::copy(tdata_vec.begin(), tdata_vec.end(), real);
    std::fill_n(img, tdata_size, 0);

    fft_t* fft = fft_init(tdata_size);
    rfft_calc(fft, real, img);

    //printf("\n");
    //perf_test();
    //printf("\n");
    //verify_test();
    return 0;
}