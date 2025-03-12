#include "timer_wrapper.h"
#include "complex.h"

#include <iostream>
#include "load_data.hpp"
#include "fft_impl.h"


#define GEN_DATA_FS         (200000)
#define GEN_DATA_FIN        (9216.3)

void test_sine(fft_data_t* real, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        real[i] = sin((double)2.0 * M_PI * (double)i * (double)GEN_DATA_FIN / (double)GEN_DATA_FS);
    }
}

/*
void FftProc(complex_t* data)
{
    // FFT CALC
    InvertedArray(data);
    FFTCalc(data);
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

    //std::vector<double> tdata_vec = load_data("testdata/fft@65536pt,120db,-124db", "tdata_win");
    std::vector<double> tdata_vec = load_data("testdata/fft@1048576pt,120db,-124db", "tdata_win");
    uint32_t tdata_size = tdata_vec.size();
    std::cout << "load data, size = " << tdata_size << std::endl;

    double* real = new double[tdata_size];
    double* img = new double[tdata_size];
    std::copy(tdata_vec.begin(), tdata_vec.end(), real);
    std::fill_n(img, tdata_size, 0);


    //fft_data_t real[16] = { 0 };
    //fft_data_t img[16] = { 0 };
    //uint32_t tdata_size = 16;
    //test_sine(real, tdata_size);

    fft_t* fft = fft_init(tdata_size);
    fft_calc(fft, real, img);
    fft_mag(fft);

    for (uint32_t i = 0; i < 16; i++)
    {
        printf("k[%d]: %f\n", i, fft->rez[i]);
    }

    fft_deinit(fft);

    //printf("\n");
    //perf_test();
    //printf("\n");
    //verify_test();
    return 0;
}