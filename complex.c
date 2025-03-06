#include "complex.h"

complex_t CreateComplex(complex_float_t Rez, complex_float_t Imz)
{
    complex_t ComplexA = { Rez,Imz };
    return ComplexA;
}

complex_t AddComplex(complex_t complexA, complex_t complexB)
{
    complexA.Rez += complexB.Rez;
    complexA.Imz += complexB.Imz;
    return complexA;
}

complex_t SubComplex(complex_t complexA, complex_t complexB)
{
    complexA.Rez -= complexB.Rez;
    complexA.Imz -= complexB.Imz;
    return complexA;
}

complex_t MulComplex(complex_t complexA, complex_t complexB)
{
    complex_t complexC = {
        complexA.Rez * complexB.Rez - complexA.Imz * complexB.Imz,
        complexA.Rez * complexB.Imz + complexA.Imz * complexB.Rez };
    return complexC;
}

complex_t ConjComplex(complex_t complexA)
{
    complexA.Imz = -complexA.Imz;
    return complexA;
}

complex_t CCWRot90Complex(complex_t complexA)
{
    complex_t complexB = { -complexA.Imz,complexA.Rez };
    return complexB;
}

complex_t CCWRot180Complex(complex_t complexA)
{
    complex_t complexB = { -complexA.Rez,-complexA.Imz };
    return complexB;
}

complex_t CCWRot270Complex(complex_t complexA)
{
    complex_t complexB = { complexA.Imz,-complexA.Rez };
    return complexB;
}

complex_float_t ModComplex(complex_t complexA)
{
    return sqrt(complexA.Rez * complexA.Rez + complexA.Imz * complexA.Imz);
}

void SwapComplex(complex_t* complexA, complex_t* complexB)
{
    complex_t complexSwap;
    memcpy(&complexSwap, (void*)complexA, sizeof(complexSwap));
    memcpy((void*)complexA, (void*)complexB, sizeof(complexSwap));
    memcpy((void*)complexB, &complexSwap, sizeof(complexSwap));
}

void PrintComplex(complex_t complexA)
{
    printf("%c%.6f%c%.6fj", complexA.Rez >= 0 ? '+' : '-', fabs(complexA.Rez),
        complexA.Imz >= 0 ? '+' : '-', fabs(complexA.Imz));
}