#pragma once
#ifndef _TIMER_WRAPPER_H_
#define _TIMER_WRAPPER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define PLATFORM_POSIX 1
#elif defined(USE_HAL_DRIVER)
#define PLATFORM_STM32 1
#else
#error "NO SUPPORTED PLATFORM"
#endif


double get_timestamp_us(void);

// TIMED_EXEC(func, &time, a, b, c);
#define TIMED_EXEC(func, ptime, ...) do {   \
    double _start = get_timestamp_us();     \
    func(__VA_ARGS__);                      \
    double _end = get_timestamp_us();       \
    (*ptime) = _end - _start;               \
} while(0)

// TIMED_EXEC_R(func, ret, &time, a, b, c);
#define TIMED_EXEC_R(func, ret, ptime, ...) do {   \
    double _start = get_timestamp_us();     \
    ret = func(__VA_ARGS__);                \
    double _end = get_timestamp_us();       \
    (*ptime) = _end - _start;               \
} while(0)

#ifdef __cplusplus
}
#endif

#endif // _TIMER_WRAPPER_H_