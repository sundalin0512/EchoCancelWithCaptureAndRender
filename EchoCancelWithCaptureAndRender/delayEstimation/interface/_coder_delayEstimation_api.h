/*
 * File: _coder_delayEstimation_api.h
 *
 * MATLAB Coder version            : 3.3
 * C/C++ source code generated on  : 27-Apr-2017 21:17:15
 */

#ifndef _CODER_DELAYESTIMATION_API_H
#define _CODER_DELAYESTIMATION_API_H

/* Include Files */
#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"
#include <stddef.h>
#include <stdlib.h>
#include "_coder_delayEstimation_api.h"

/* Type Definitions */
#ifndef struct_emxArray_real32_T
#define struct_emxArray_real32_T

struct emxArray_real32_T
{
  real32_T *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /*struct_emxArray_real32_T*/

#ifndef typedef_emxArray_real32_T
#define typedef_emxArray_real32_T

typedef struct emxArray_real32_T emxArray_real32_T;

#endif                                 /*typedef_emxArray_real32_T*/

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

/* Function Declarations */
extern void LMS(emxArray_real32_T *xn, emxArray_real32_T *dn, real32_T M,
                real32_T mu, real32_T itr, emxArray_real32_T *b_yn,
                emxArray_real32_T *W, emxArray_real32_T *en);
extern void LMS_api(const mxArray * const prhs[5], const mxArray *plhs[3]);
extern void NLMS(emxArray_real32_T *xn, emxArray_real32_T *dn, real32_T M,
                 real32_T mu, real32_T itr, emxArray_real32_T *b_yn,
                 emxArray_real32_T *W, emxArray_real32_T *en);
extern void NLMS_api(const mxArray * const prhs[5], const mxArray *plhs[3]);
extern void RLS(emxArray_real32_T *xn, emxArray_real32_T *dn, real32_T M,
                real32_T itr, emxArray_real32_T *b_yn, emxArray_real32_T *W,
                emxArray_real32_T *en);
extern void RLS_api(const mxArray * const prhs[4], const mxArray *plhs[3]);
extern real32_T delayEstimation(emxArray_real32_T *farEndSound,
  emxArray_real32_T *nearEndSound);
extern void delayEstimation_api(const mxArray * const prhs[2], const mxArray
  *plhs[1]);
extern void delayEstimation_atexit(void);
extern void delayEstimation_initialize(void);
extern void delayEstimation_terminate(void);
extern void delayEstimation_xil_terminate(void);

#endif

/*
 * File trailer for _coder_delayEstimation_api.h
 *
 * [EOF]
 */
