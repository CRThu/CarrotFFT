#pragma once

#ifndef _FFT_H_
#define _FFT_H_

#include <malloc.h>
#include "complex.h"

#define PI      acos(-1.0)

extern uint32_t FFT_N;
extern uint8_t FFT_LOGN;

extern complex_t* pWNm;
extern uint32_t pWNmCnt;

complex_t GetWNm(uint32_t* index);
complex_t PrintWNm();
void InvertedArray(complex_t* complexArray);
void FFTCalc(complex_t* complexFFT);

#endif // _FFT_H_
