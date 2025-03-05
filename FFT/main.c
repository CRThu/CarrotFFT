
#include <time.h>
#include <string.h>
#include <windows.h>
#include <psapi.h>
#pragma  comment(lib,"Psapi.lib")

#include "timer_wrapper.h"
#include "complex.h"
#include "FFT.h"


// DEFINE
#define SAMPLE_CNT          1048576
#define SINE_FS             0.05

// TESTBENCH
//#define VERIFY_TESTBENCH    10
#define SPEED_TESTBENCH     1048576
//#define SPEED_TESTBENCH     1048576*256

// Sine
void GenSine(complex_t* sine, double fs, double cnt)
{
    for (uint32_t i = 0; i < cnt; i++)
    {
        sine[i] = CreateComplex(sin((double)2.0 * PI * (double)i * (double)fs), 0);
    }
}

void test(int* t)
{
    Sleep(*t);
}

int main()
{
    printf("CARROT-FFT-LIBRARY TESTBENCH @ https://github.com/CRThu \n");

    int t = 1;
    double elapsed = timed_exec(test, &t);
    printf("TIME WRAPPER RETURNED: %.3lf ms\r\n", elapsed / 1000.0);


    int iter = 1;
    int fft_sample_cnt = SAMPLE_CNT;
#if SPEED_TESTBENCH != 0
    iter = (int)ceil(log2(SPEED_TESTBENCH));
    fft_sample_cnt = 2;
#endif

    printf("\n");
    printf("|\tFFT N |\tINIT Elapsed |\tCALC Elapsed |\tMemory Usage |\n");
    printf("|\t----- |\t------------ |\t------------ |\t------------ |\n");

    for (int i = 0; i < iter; i++)
    {
        clock_t t_start;
        clock_t t_stop;
        double InitElapsedTime;
        double CalcElapsedTime;

        // INIT
        t_start = clock();
        FFT_Init(fft_sample_cnt);
        t_stop = clock();
        InitElapsedTime = ((double)t_stop - (double)t_start) / (double)CLOCKS_PER_SEC;

        // WNm For Debug
        //PrintWNm();

        // MALLOC
        complex_t* fft_io = NULL;
        fft_io = (complex_t*)malloc(sizeof(complex_t) * FFT_N);
        if (fft_io == NULL)
            return -1;
        memset(fft_io, 0, sizeof(complex_t) * FFT_N);

        // GEN SINE WAVE
        GenSine(fft_io, SINE_FS, fft_sample_cnt);

        // FFT CALC
        t_start = clock();
        InvertedArray(fft_io);
        FFTCalc(fft_io);
        t_stop = clock();
        CalcElapsedTime = ((double)t_stop - (double)t_start) / (double)CLOCKS_PER_SEC;

        HANDLE handle = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS pmc;
        GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));

        printf("|%12ld |\t%10.3lf s |\t%10.3lf s |\t%9ld KB |\n", FFT_N, InitElapsedTime, CalcElapsedTime, (uint64_t)pmc.WorkingSetSize / 1024);

#if VERIFY_TESTBENCH != 0
        for (uint32_t i = 0; i < FFT_N; (FFT_N <= VERIFY_TESTBENCH) ? (i++) : (i += FFT_N / VERIFY_TESTBENCH))
        {
            printf("[FFT k = %8d; X(k) = ", i);
            PrintComplex(fft_io[i]);
            printf(" (Mod = %.6f)]\n", ModComplex(fft_io[i]));
        }
#endif
        // FREE
        FFT_DeInit();
        free(fft_io);
        fft_io = NULL;

#if SPEED_TESTBENCH != 0
        fft_sample_cnt *= 2;
#endif
    }
    return 0;
}