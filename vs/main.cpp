#include <iostream>
#include "load_data.hpp"
#include "fft_impl.h"
#include "timer_wrapper.h"
#include "analysis.h"


#define GEN_DATA_FS         (200000)
#define GEN_DATA_FIN        (9216.3)

void test_sine(fft_data_t* real, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++)
    {
        real[i] = sin((double)2.0 * M_PI * (double)i * (double)GEN_DATA_FIN / (double)GEN_DATA_FS);
    }
}

void fft_testbench()
{
    //std::vector<double> tdata_vec = load_data("testdata/fft@65536pt,120db,-124db", "tdata_win");
    std::vector<double> tdata_vec = load_data("testdata/fft@1048576pt,120db,-124db", "tdata_win");
    uint32_t tdata_size = (uint32_t)tdata_vec.size();
    std::cout << "load data, size = " << tdata_size << std::endl;

    double* real = new double[tdata_size];
    double* img = new double[tdata_size];
    std::copy(tdata_vec.begin(), tdata_vec.end(), real);
    std::fill_n(img, tdata_size, 0);

    /*
    uint32_t tdata_size = 1048576;
    fft_data_t* real = (fft_data_t*)malloc(tdata_size * sizeof(fft_data_t));
    fft_data_t* img = (fft_data_t*)malloc(tdata_size * sizeof(fft_data_t));
    test_sine(real, tdata_size);
    */

    double init_time = 0, calc_time = 0;
    fft_t* fft;
    TIMED_EXEC_R(fft_init, fft, &init_time, tdata_size);    // fft = fft_init(tdata_size);
    if (fft == NULL)
    {
        std::cout << "no enough memory or error when calling fft_init()" << std::endl;
        return;
    }
    TIMED_EXEC(fft_calc, &calc_time, fft, real, img);       // fft_calc(fft, real, img);
    FFT_MAG(fft, FFT_FFTN_HALF);

    /*
    for (uint32_t i = 0; i < 16; i++)
    {
        printf("k[%d]: %f\n", i, fft->rez[i]);
    }
    */

    printf("|\tFFT N |\tINIT Elapsed |\tCALC Elapsed |\n");
    printf("|\t----- |\t------------ |\t------------ |\n");
    printf("|%12ld |\t%9.3lf ms |\t%9.3lf ms |\n", fft->fftn, init_time / 1000.0, calc_time / 1000.0);

    for (uint32_t i = 0; i < 16; i++)
    {
        TIMED_EXEC_R(fft_init, fft, &init_time, tdata_size);    // fft = fft_init(tdata_size);
        TIMED_EXEC(fft_calc, &calc_time, fft, real, img);       // fft_calc(fft, real, img);
        printf("|%12ld |\t%9.3lf ms |\t%9.3lf ms |\n", fft->fftn, init_time / 1000.0, calc_time / 1000.0);
    }

    fft_deinit(fft);

}

void analysis_testbench()
{
    window_t* win = get_window_by_name("HFT144D");

    //std::vector<double> tdata_vec = load_data("testdata/fft@65536pt,130db,-124db", "tdata");
    std::vector<double> tdata_vec = load_data("testdata/fft@1048576pt,180db,-194db", "tdata");
    uint32_t tdata_size = (uint32_t)tdata_vec.size();
    std::cout << "load data, size = " << tdata_size << std::endl;

    double* real = new double[tdata_size];
    double* img = new double[tdata_size];
    std::copy(tdata_vec.begin(), tdata_vec.end(), real);
    std::fill_n(img, tdata_size, 0);

    double fs = 20000;
    uint8_t hdn = 5;

    analysis_t* analysis = analysis_init(tdata_size, fs, win, hdn);
    if (analysis == NULL)
    {
        std::cout << "no enough memory or error when calling analysis_init()" << std::endl;
        return;
    }
    analysis_run(analysis, real, img);

    analysis_deinit(analysis);
}


int main()
{
    printf("CARROT-FFT-LIBRARY TESTBENCH @ https://github.com/CRThu \n");

    //printf("\n");
    //fft_testbench();
    printf("\n");
    analysis_testbench();
    return 0;
}