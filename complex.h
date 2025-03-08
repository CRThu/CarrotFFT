#pragma once
#ifndef _COMPLEX_H_
#define _COMPLEX_H_

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

typedef double complex_float_t;

typedef struct complex_t
{
    complex_float_t Rez;
    complex_float_t Imz;
}complex_t;

complex_t CreateComplex(complex_float_t Rez, complex_float_t Imz);
complex_t AddComplex(complex_t complexA, complex_t complexB);
complex_t SubComplex(complex_t complexA, complex_t complexB);
complex_t MulComplex(complex_t complexA, complex_t complexB);
complex_t ConjComplex(complex_t complexA);
complex_t CCWRot90Complex(complex_t complexA);
complex_t CCWRot180Complex(complex_t complexA);
complex_t CCWRot270Complex(complex_t complexA);
complex_float_t ModComplex(complex_t complexA);
void SwapComplex(complex_t* complexA, complex_t* complexB);
void PrintComplex(complex_t complexA);

#endif // _COMPLEX_H_
