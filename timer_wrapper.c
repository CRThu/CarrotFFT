// timer_wrapper.c
#include "timer_wrapper.h"


#if PLATFORM_WINDOWS

#include <windows.h>

double get_timestamp_us(void)
{
    static LARGE_INTEGER freq;
    static LARGE_INTEGER start;
    static int initialized = 0;

    if (!initialized)
    {
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start);
        initialized = 1;
    }

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - start.QuadPart) * 1e6 / freq.QuadPart;
}

#elif PLATFORM_POSIX

#include <time.h>

double get_timestamp_us(void)
{
    static struct timespec start;
    static int initialized = 0;

    if (!initialized)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        initialized = 1;
        return 0.0;
    }

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - start.tv_sec) * 1e6 + (now.tv_nsec - start.tv_nsec) / 1e3;
}

#elif PLATFORM_STM32

// 需要用户提前配置定时器（例如TIM2）
static TIM_HandleTypeDef* htim = &htim2; // 根据实际配置修改

double get_timestamp_us(void)
{
    static uint32_t overflow_count = 0;
    static uint32_t last_counter = 0;
    const uint32_t counter = __HAL_TIM_GET_COUNTER(htim);

    // 处理定时器溢出
    if (counter < last_counter)
    {
        overflow_count++;
    }
    last_counter = counter;

    const uint32_t max_count = __HAL_TIM_GET_AUTORELOAD(htim);
    return (double)(overflow_count * (max_count + 1) + counter) * (htim->Instance->PSC + 1) * 1e6 / HAL_RCC_GetPCLK1Freq();
}

#endif