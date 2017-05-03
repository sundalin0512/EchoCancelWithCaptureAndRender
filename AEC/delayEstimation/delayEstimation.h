//
// File: delayEstimation.h
//
// MATLAB Coder version            : 3.3
// C/C++ source code generated on  : 27-Apr-2017 21:26:58
//
#ifndef DELAYESTIMATION_H
#define DELAYESTIMATION_H

// Include Files
#include <cmath>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "rtwtypes.h"
#include "delayEstimation_types.h"

// Function Declarations
extern void LMS(const emxArray_real32_T *xn, const emxArray_real32_T *dn, float
                M, float mu, float itr, emxArray_real32_T *b_yn,
                emxArray_real32_T *W, emxArray_real32_T *en);
extern void NLMS(const emxArray_real32_T *xn, const emxArray_real32_T *dn, float
                 M, float mu, float itr, emxArray_real32_T *b_yn,
                 emxArray_real32_T *W, emxArray_real32_T *en);
extern void RLS(const emxArray_real32_T *xn, const emxArray_real32_T *dn, float
                M, float itr, emxArray_real32_T *b_yn, emxArray_real32_T *W,
                emxArray_real32_T *en);
extern float delayEstimation(const emxArray_real32_T *farEndSound, const
  emxArray_real32_T *nearEndSound);
extern void delayEstimation_initialize();
extern void delayEstimation_terminate();

#endif

//
// File trailer for delayEstimation.h
//
// [EOF]
//
