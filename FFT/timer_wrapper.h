#pragma once
#ifndef _TIMER_WRAPPER_H_
#define _TIMER_WRAPPER_H_

#include <stdint.h>

#if defined(_WIN32)
#define PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define PLATFORM_POSIX 1
#elif defined(USE_HAL_DRIVER)
#define PLATFORM_STM32 1
#else
#error "NO SUPPORTED PLATFORM"
#endif

typedef void (*timed_func_t)(void*);
double timed_exec(timed_func_t func, void* arg);

#endif // _TIMER_WRAPPER_H_