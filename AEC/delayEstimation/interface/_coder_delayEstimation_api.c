/*
 * File: _coder_delayEstimation_api.c
 *
 * MATLAB Coder version            : 3.3
 * C/C++ source code generated on  : 27-Apr-2017 21:17:15
 */

/* Include Files */
#include "tmwtypes.h"
#include "_coder_delayEstimation_api.h"
#include "_coder_delayEstimation_mex.h"

/* Type Definitions */
#ifndef struct_emxArray__common
#define struct_emxArray__common

struct emxArray__common
{
  void *data;
  int32_T *size;
  int32_T allocatedSize;
  int32_T numDimensions;
  boolean_T canFreeData;
};

#endif                                 /*struct_emxArray__common*/

#ifndef typedef_emxArray__common
#define typedef_emxArray__common

typedef struct emxArray__common emxArray__common;

#endif                                 /*typedef_emxArray__common*/

/* Variable Definitions */
emlrtCTX emlrtRootTLSGlobal = NULL;
emlrtContext emlrtContextGlobal = { true,/* bFirstTime */
  false,                               /* bInitialized */
  131450U,                             /* fVersionInfo */
  NULL,                                /* fErrorFunction */
  "delayEstimation",                   /* fFunctionName */
  NULL,                                /* fRTCallStack */
  false,                               /* bDebugMode */
  { 2045744189U, 2170104910U, 2743257031U, 4284093946U },/* fSigWrd */
  NULL                                 /* fSigMem */
};

/* Function Declarations */
static void b_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real32_T *y);
static const mxArray *b_emlrt_marshallOut(const emxArray_real32_T *u);
static void c_emlrt_marshallIn(const mxArray *xn, const char_T *identifier,
  emxArray_real32_T *y);
static const mxArray *c_emlrt_marshallOut(const emxArray_real32_T *u);
static void d_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real32_T *y);
static real32_T e_emlrt_marshallIn(const mxArray *M, const char_T *identifier);
static void emlrt_marshallIn(const mxArray *farEndSound, const char_T
  *identifier, emxArray_real32_T *y);
static const mxArray *emlrt_marshallOut(const real32_T u);
static void emxEnsureCapacity(emxArray__common *emxArray, int32_T oldNumel,
  uint32_T elementSize);
static void emxFree_real32_T(emxArray_real32_T **pEmxArray);
static void emxInit_real32_T(emxArray_real32_T **pEmxArray, int32_T
  numDimensions, boolean_T doPush);
static void emxInit_real32_T1(emxArray_real32_T **pEmxArray, int32_T
  numDimensions, boolean_T doPush);
static real32_T f_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId);
static void g_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real32_T *ret);
static void h_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real32_T *ret);
static real32_T i_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier *
  msgId);

/* Function Definitions */

/*
 * Arguments    : const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 *                emxArray_real32_T *y
 * Return Type  : void
 */
static void b_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real32_T *y)
{
  g_emlrt_marshallIn(emlrtAlias(u), parentId, y);
  emlrtDestroyArray(&u);
}

/*
 * Arguments    : const emxArray_real32_T *u
 * Return Type  : const mxArray *
 */
static const mxArray *b_emlrt_marshallOut(const emxArray_real32_T *u)
{
  const mxArray *y;
  const mxArray *m1;
  real32_T *pData;
  int32_T i0;
  int32_T i;
  y = NULL;
  m1 = emlrtCreateNumericArray(2, *(int32_T (*)[2])u->size, mxSINGLE_CLASS,
    mxREAL);
  pData = (real32_T *)mxGetData(m1);
  i0 = 0;
  for (i = 0; i < u->size[1]; i++) {
    pData[i0] = u->data[u->size[0] * i];
    i0++;
  }

  emlrtAssign(&y, m1);
  return y;
}

/*
 * Arguments    : const mxArray *xn
 *                const char_T *identifier
 *                emxArray_real32_T *y
 * Return Type  : void
 */
static void c_emlrt_marshallIn(const mxArray *xn, const char_T *identifier,
  emxArray_real32_T *y)
{
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = (const char *)identifier;
  thisId.fParent = NULL;
  thisId.bParentIsCell = false;
  d_emlrt_marshallIn(emlrtAlias(xn), &thisId, y);
  emlrtDestroyArray(&xn);
}

/*
 * Arguments    : const emxArray_real32_T *u
 * Return Type  : const mxArray *
 */
static const mxArray *c_emlrt_marshallOut(const emxArray_real32_T *u)
{
  const mxArray *y;
  const mxArray *m2;
  real32_T *pData;
  int32_T i1;
  int32_T i;
  y = NULL;
  m2 = emlrtCreateNumericArray(1, *(int32_T (*)[2])u->size, mxSINGLE_CLASS,
    mxREAL);
  pData = (real32_T *)mxGetData(m2);
  i1 = 0;
  for (i = 0; i < u->size[0]; i++) {
    pData[i1] = u->data[i];
    i1++;
  }

  emlrtAssign(&y, m2);
  return y;
}

/*
 * Arguments    : const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 *                emxArray_real32_T *y
 * Return Type  : void
 */
static void d_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId, emxArray_real32_T *y)
{
  h_emlrt_marshallIn(emlrtAlias(u), parentId, y);
  emlrtDestroyArray(&u);
}

/*
 * Arguments    : const mxArray *M
 *                const char_T *identifier
 * Return Type  : real32_T
 */
static real32_T e_emlrt_marshallIn(const mxArray *M, const char_T *identifier)
{
  real32_T y;
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = (const char *)identifier;
  thisId.fParent = NULL;
  thisId.bParentIsCell = false;
  y = f_emlrt_marshallIn(emlrtAlias(M), &thisId);
  emlrtDestroyArray(&M);
  return y;
}

/*
 * Arguments    : const mxArray *farEndSound
 *                const char_T *identifier
 *                emxArray_real32_T *y
 * Return Type  : void
 */
static void emlrt_marshallIn(const mxArray *farEndSound, const char_T
  *identifier, emxArray_real32_T *y)
{
  emlrtMsgIdentifier thisId;
  thisId.fIdentifier = (const char *)identifier;
  thisId.fParent = NULL;
  thisId.bParentIsCell = false;
  b_emlrt_marshallIn(emlrtAlias(farEndSound), &thisId, y);
  emlrtDestroyArray(&farEndSound);
}

/*
 * Arguments    : const real32_T u
 * Return Type  : const mxArray *
 */
static const mxArray *emlrt_marshallOut(const real32_T u)
{
  const mxArray *y;
  const mxArray *m0;
  y = NULL;
  m0 = emlrtCreateNumericMatrix(1, 1, mxSINGLE_CLASS, mxREAL);
  *(real32_T *)mxGetData(m0) = u;
  emlrtAssign(&y, m0);
  return y;
}

/*
 * Arguments    : emxArray__common *emxArray
 *                int32_T oldNumel
 *                uint32_T elementSize
 * Return Type  : void
 */
static void emxEnsureCapacity(emxArray__common *emxArray, int32_T oldNumel,
  uint32_T elementSize)
{
  int32_T newNumel;
  int32_T i;
  void *newData;
  if (oldNumel < 0) {
    oldNumel = 0;
  }

  newNumel = 1;
  for (i = 0; i < emxArray->numDimensions; i++) {
    newNumel *= emxArray->size[i];
  }

  if (newNumel > emxArray->allocatedSize) {
    i = emxArray->allocatedSize;
    if (i < 16) {
      i = 16;
    }

    while (i < newNumel) {
      if (i > 1073741823) {
        i = MAX_int32_T;
      } else {
        i <<= 1;
      }
    }

    newData = emlrtCallocMex((uint32_T)i, elementSize);
    if (emxArray->data != NULL) {
      memcpy(newData, emxArray->data, elementSize * oldNumel);
      if (emxArray->canFreeData) {
        emlrtFreeMex(emxArray->data);
      }
    }

    emxArray->data = newData;
    emxArray->allocatedSize = i;
    emxArray->canFreeData = true;
  }
}

/*
 * Arguments    : emxArray_real32_T **pEmxArray
 * Return Type  : void
 */
static void emxFree_real32_T(emxArray_real32_T **pEmxArray)
{
  if (*pEmxArray != (emxArray_real32_T *)NULL) {
    if (((*pEmxArray)->data != (real32_T *)NULL) && (*pEmxArray)->canFreeData) {
      emlrtFreeMex((void *)(*pEmxArray)->data);
    }

    emlrtFreeMex((void *)(*pEmxArray)->size);
    emlrtFreeMex((void *)*pEmxArray);
    *pEmxArray = (emxArray_real32_T *)NULL;
  }
}

/*
 * Arguments    : emxArray_real32_T **pEmxArray
 *                int32_T numDimensions
 *                boolean_T doPush
 * Return Type  : void
 */
static void emxInit_real32_T(emxArray_real32_T **pEmxArray, int32_T
  numDimensions, boolean_T doPush)
{
  emxArray_real32_T *emxArray;
  int32_T i;
  *pEmxArray = (emxArray_real32_T *)emlrtMallocMex(sizeof(emxArray_real32_T));
  if (doPush) {
    emlrtPushHeapReferenceStackR2012b(emlrtRootTLSGlobal, (void *)pEmxArray,
      (void (*)(void *))emxFree_real32_T);
  }

  emxArray = *pEmxArray;
  emxArray->data = (real32_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int32_T *)emlrtMallocMex((uint32_T)(sizeof(int32_T)
    * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

/*
 * Arguments    : emxArray_real32_T **pEmxArray
 *                int32_T numDimensions
 *                boolean_T doPush
 * Return Type  : void
 */
static void emxInit_real32_T1(emxArray_real32_T **pEmxArray, int32_T
  numDimensions, boolean_T doPush)
{
  emxArray_real32_T *emxArray;
  int32_T i;
  *pEmxArray = (emxArray_real32_T *)emlrtMallocMex(sizeof(emxArray_real32_T));
  if (doPush) {
    emlrtPushHeapReferenceStackR2012b(emlrtRootTLSGlobal, (void *)pEmxArray,
      (void (*)(void *))emxFree_real32_T);
  }

  emxArray = *pEmxArray;
  emxArray->data = (real32_T *)NULL;
  emxArray->numDimensions = numDimensions;
  emxArray->size = (int32_T *)emlrtMallocMex((uint32_T)(sizeof(int32_T)
    * numDimensions));
  emxArray->allocatedSize = 0;
  emxArray->canFreeData = true;
  for (i = 0; i < numDimensions; i++) {
    emxArray->size[i] = 0;
  }
}

/*
 * Arguments    : const mxArray *u
 *                const emlrtMsgIdentifier *parentId
 * Return Type  : real32_T
 */
static real32_T f_emlrt_marshallIn(const mxArray *u, const emlrtMsgIdentifier
  *parentId)
{
  real32_T y;
  y = i_emlrt_marshallIn(emlrtAlias(u), parentId);
  emlrtDestroyArray(&u);
  return y;
}

/*
 * Arguments    : const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 *                emxArray_real32_T *ret
 * Return Type  : void
 */
static void g_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real32_T *ret)
{
  static const int32_T dims[1] = { -1 };

  const boolean_T bv0[1] = { true };

  int32_T iv0[1];
  int32_T i2;
  emlrtCheckVsBuiltInR2012b(emlrtRootTLSGlobal, msgId, src, "single|double",
    false, 1U, dims, &bv0[0], iv0);
  i2 = ret->size[0];
  ret->size[0] = iv0[0];
  emxEnsureCapacity((emxArray__common *)ret, i2, sizeof(real32_T));
  emlrtImportArrayR2015b(emlrtRootTLSGlobal, src, ret->data, 4, false);
  emlrtDestroyArray(&src);
}

/*
 * Arguments    : const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 *                emxArray_real32_T *ret
 * Return Type  : void
 */
static void h_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier
  *msgId, emxArray_real32_T *ret)
{
  static const int32_T dims[2] = { 1, -1 };

  const boolean_T bv1[2] = { false, true };

  int32_T iv1[2];
  int32_T i3;
  emlrtCheckVsBuiltInR2012b(emlrtRootTLSGlobal, msgId, src, "single|double",
    false, 2U, dims, &bv1[0], iv1);
  i3 = ret->size[0] * ret->size[1];
  ret->size[0] = iv1[0];
  ret->size[1] = iv1[1];
  emxEnsureCapacity((emxArray__common *)ret, i3, sizeof(real32_T));
  emlrtImportArrayR2015b(emlrtRootTLSGlobal, src, ret->data, 4, false);
  emlrtDestroyArray(&src);
}

/*
 * Arguments    : const mxArray *src
 *                const emlrtMsgIdentifier *msgId
 * Return Type  : real32_T
 */
static real32_T i_emlrt_marshallIn(const mxArray *src, const emlrtMsgIdentifier *
  msgId)
{
  real32_T ret;
  static const int32_T dims = 0;
  emlrtCheckBuiltInR2012b(emlrtRootTLSGlobal, msgId, src, "single|double", false,
    0U, &dims);
  emlrtImportArrayR2015b(emlrtRootTLSGlobal, src, &ret, 4, false);
  emlrtDestroyArray(&src);
  return ret;
}

/*
 * Arguments    : const mxArray * const prhs[5]
 *                const mxArray *plhs[3]
 * Return Type  : void
 */
void LMS_api(const mxArray * const prhs[5], const mxArray *plhs[3])
{
  emxArray_real32_T *xn;
  emxArray_real32_T *dn;
  emxArray_real32_T *b_yn;
  emxArray_real32_T *W;
  emxArray_real32_T *en;
  real32_T M;
  real32_T mu;
  real32_T itr;
  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  emxInit_real32_T1(&xn, 2, true);
  emxInit_real32_T1(&dn, 2, true);
  emxInit_real32_T1(&b_yn, 2, true);
  emxInit_real32_T1(&W, 2, true);
  emxInit_real32_T(&en, 1, true);

  /* Marshall function inputs */
  c_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[0]), "xn", xn);
  c_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[1]), "dn", dn);
  M = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[2]), "M");
  mu = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[3]), "mu");
  itr = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[4]), "itr");

  /* Invoke the target function */
  LMS(xn, dn, M, mu, itr, b_yn, W, en);

  /* Marshall function outputs */
  plhs[0] = b_emlrt_marshallOut(b_yn);
  plhs[1] = b_emlrt_marshallOut(W);
  plhs[2] = c_emlrt_marshallOut(en);
  emxFree_real32_T(&en);
  emxFree_real32_T(&W);
  emxFree_real32_T(&b_yn);
  emxFree_real32_T(&dn);
  emxFree_real32_T(&xn);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : const mxArray * const prhs[5]
 *                const mxArray *plhs[3]
 * Return Type  : void
 */
void NLMS_api(const mxArray * const prhs[5], const mxArray *plhs[3])
{
  emxArray_real32_T *xn;
  emxArray_real32_T *dn;
  emxArray_real32_T *b_yn;
  emxArray_real32_T *W;
  emxArray_real32_T *en;
  real32_T M;
  real32_T mu;
  real32_T itr;
  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  emxInit_real32_T1(&xn, 2, true);
  emxInit_real32_T1(&dn, 2, true);
  emxInit_real32_T1(&b_yn, 2, true);
  emxInit_real32_T1(&W, 2, true);
  emxInit_real32_T(&en, 1, true);

  /* Marshall function inputs */
  c_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[0]), "xn", xn);
  c_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[1]), "dn", dn);
  M = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[2]), "M");
  mu = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[3]), "mu");
  itr = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[4]), "itr");

  /* Invoke the target function */
  NLMS(xn, dn, M, mu, itr, b_yn, W, en);

  /* Marshall function outputs */
  plhs[0] = b_emlrt_marshallOut(b_yn);
  plhs[1] = b_emlrt_marshallOut(W);
  plhs[2] = c_emlrt_marshallOut(en);
  emxFree_real32_T(&en);
  emxFree_real32_T(&W);
  emxFree_real32_T(&b_yn);
  emxFree_real32_T(&dn);
  emxFree_real32_T(&xn);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : const mxArray * const prhs[4]
 *                const mxArray *plhs[3]
 * Return Type  : void
 */
void RLS_api(const mxArray * const prhs[4], const mxArray *plhs[3])
{
  emxArray_real32_T *xn;
  emxArray_real32_T *dn;
  emxArray_real32_T *b_yn;
  emxArray_real32_T *W;
  emxArray_real32_T *en;
  real32_T M;
  real32_T itr;
  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  emxInit_real32_T1(&xn, 2, true);
  emxInit_real32_T1(&dn, 2, true);
  emxInit_real32_T1(&b_yn, 2, true);
  emxInit_real32_T1(&W, 2, true);
  emxInit_real32_T(&en, 1, true);

  /* Marshall function inputs */
  c_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[0]), "xn", xn);
  c_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[1]), "dn", dn);
  M = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[2]), "M");
  itr = e_emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[3]), "itr");

  /* Invoke the target function */
  RLS(xn, dn, M, itr, b_yn, W, en);

  /* Marshall function outputs */
  plhs[0] = b_emlrt_marshallOut(b_yn);
  plhs[1] = b_emlrt_marshallOut(W);
  plhs[2] = c_emlrt_marshallOut(en);
  emxFree_real32_T(&en);
  emxFree_real32_T(&W);
  emxFree_real32_T(&b_yn);
  emxFree_real32_T(&dn);
  emxFree_real32_T(&xn);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : const mxArray * const prhs[2]
 *                const mxArray *plhs[1]
 * Return Type  : void
 */
void delayEstimation_api(const mxArray * const prhs[2], const mxArray *plhs[1])
{
  emxArray_real32_T *farEndSound;
  emxArray_real32_T *nearEndSound;
  real32_T delay;
  emlrtHeapReferenceStackEnterFcnR2012b(emlrtRootTLSGlobal);
  emxInit_real32_T(&farEndSound, 1, true);
  emxInit_real32_T(&nearEndSound, 1, true);

  /* Marshall function inputs */
  emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[0]), "farEndSound",
                   farEndSound);
  emlrt_marshallIn(emlrtAliasP((const mxArray *)prhs[1]), "nearEndSound",
                   nearEndSound);

  /* Invoke the target function */
  delay = delayEstimation(farEndSound, nearEndSound);

  /* Marshall function outputs */
  plhs[0] = emlrt_marshallOut(delay);
  emxFree_real32_T(&nearEndSound);
  emxFree_real32_T(&farEndSound);
  emlrtHeapReferenceStackLeaveFcnR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void delayEstimation_atexit(void)
{
  mexFunctionCreateRootTLS();
  emlrtEnterRtStackR2012b(emlrtRootTLSGlobal);
  emlrtLeaveRtStackR2012b(emlrtRootTLSGlobal);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
  delayEstimation_xil_terminate();
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void delayEstimation_initialize(void)
{
  mexFunctionCreateRootTLS();
  emlrtClearAllocCountR2012b(emlrtRootTLSGlobal, false, 0U, 0);
  emlrtEnterRtStackR2012b(emlrtRootTLSGlobal);
  emlrtFirstTimeR2012b(emlrtRootTLSGlobal);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
void delayEstimation_terminate(void)
{
  emlrtLeaveRtStackR2012b(emlrtRootTLSGlobal);
  emlrtDestroyRootTLS(&emlrtRootTLSGlobal);
}

/*
 * File trailer for _coder_delayEstimation_api.c
 *
 * [EOF]
 */
