//
// File: main.cpp
//
// MATLAB Coder version            : 3.3
// C/C++ source code generated on  : 27-Apr-2017 21:17:15
//

//***********************************************************************
// This automatically generated example C main file shows how to call
// entry-point functions that MATLAB Coder generated. You must customize
// this file for your application. Do not modify this file directly.
// Instead, make a copy of this file, modify it, and integrate it into
// your development environment.
//
// This file initializes entry-point function arguments to a default
// size and value before calling the entry-point functions. It does
// not store or use any values returned from the entry-point functions.
// If necessary, it does pre-allocate memory for returned values.
// You can use this file as a starting point for a main function that
// you can deploy in your application.
//
// After you copy the file, and before you deploy it, you must make the
// following changes:
// * For variable-size function arguments, change the example sizes to
// the sizes that your application requires.
// * Change the example values of function arguments to the values that
// your application requires.
// * If the entry-point functions return values, store these values or
// otherwise use them as required by your application.
//
//***********************************************************************
// Include Files
#include "rt_nonfinite.h"
#include "delayEstimation.h"
#include "main.h"
#include "delayEstimation_emxAPI.h"

// Function Declarations
static emxArray_real32_T *argInit_1xUnbounded_real32_T();
static emxArray_real32_T *argInit_Unboundedx1_real32_T();
static float argInit_real32_T();
static void main_LMS();
static void main_NLMS();
static void main_RLS();
static void main_delayEstimation();

// Function Definitions

//
// Arguments    : void
// Return Type  : emxArray_real32_T *
//
static emxArray_real32_T *argInit_1xUnbounded_real32_T()
{
  emxArray_real32_T *result;
  static int iv1[2] = { 1, 2 };

  int idx1;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result = emxCreateND_real32_T(2, *(int (*)[2])&iv1[0]);

  // Loop over the array to initialize each element.
  for (idx1 = 0; idx1 < result->size[1U]; idx1++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result->data[result->size[0] * idx1] = argInit_real32_T();
  }

  return result;
}

//
// Arguments    : void
// Return Type  : emxArray_real32_T *
//
static emxArray_real32_T *argInit_Unboundedx1_real32_T()
{
  emxArray_real32_T *result;
  static int iv0[1] = { 2 };

  int idx0;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result = emxCreateND_real32_T(1, *(int (*)[1])&iv0[0]);

  // Loop over the array to initialize each element.
  for (idx0 = 0; idx0 < result->size[0U]; idx0++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result->data[idx0] = argInit_real32_T();
  }

  return result;
}

//
// Arguments    : void
// Return Type  : float
//
static float argInit_real32_T()
{
  return 0.0F;
}

//
// Arguments    : void
// Return Type  : void
//
static void main_LMS()
{
  emxArray_real32_T *b_yn;
  emxArray_real32_T *W;
  emxArray_real32_T *en;
  emxArray_real32_T *xn;
  emxArray_real32_T *dn;
  emxInitArray_real32_T(&b_yn, 2);
  emxInitArray_real32_T(&W, 2);
  emxInitArray_real32_T(&en, 1);

  // Initialize function 'LMS' input arguments.
  // Initialize function input argument 'xn'.
  xn = argInit_1xUnbounded_real32_T();

  // Initialize function input argument 'dn'.
  dn = argInit_1xUnbounded_real32_T();

  // Call the entry-point 'LMS'.
  LMS(xn, dn, argInit_real32_T(), argInit_real32_T(), argInit_real32_T(), b_yn,
      W, en);
  emxDestroyArray_real32_T(en);
  emxDestroyArray_real32_T(W);
  emxDestroyArray_real32_T(b_yn);
  emxDestroyArray_real32_T(dn);
  emxDestroyArray_real32_T(xn);
}

//
// Arguments    : void
// Return Type  : void
//
static void main_NLMS()
{
  emxArray_real32_T *b_yn;
  emxArray_real32_T *W;
  emxArray_real32_T *en;
  emxArray_real32_T *xn;
  emxArray_real32_T *dn;
  emxInitArray_real32_T(&b_yn, 2);
  emxInitArray_real32_T(&W, 2);
  emxInitArray_real32_T(&en, 1);

  // Initialize function 'NLMS' input arguments.
  // Initialize function input argument 'xn'.
  xn = argInit_1xUnbounded_real32_T();

  // Initialize function input argument 'dn'.
  dn = argInit_1xUnbounded_real32_T();

  // Call the entry-point 'NLMS'.
  NLMS(xn, dn, argInit_real32_T(), argInit_real32_T(), argInit_real32_T(), b_yn,
       W, en);
  emxDestroyArray_real32_T(en);
  emxDestroyArray_real32_T(W);
  emxDestroyArray_real32_T(b_yn);
  emxDestroyArray_real32_T(dn);
  emxDestroyArray_real32_T(xn);
}

//
// Arguments    : void
// Return Type  : void
//
static void main_RLS()
{
  emxArray_real32_T *b_yn;
  emxArray_real32_T *W;
  emxArray_real32_T *en;
  emxArray_real32_T *xn;
  emxArray_real32_T *dn;
  emxInitArray_real32_T(&b_yn, 2);
  emxInitArray_real32_T(&W, 2);
  emxInitArray_real32_T(&en, 1);

  // Initialize function 'RLS' input arguments.
  // Initialize function input argument 'xn'.
  xn = argInit_1xUnbounded_real32_T();

  // Initialize function input argument 'dn'.
  dn = argInit_1xUnbounded_real32_T();

  // Call the entry-point 'RLS'.
  RLS(xn, dn, argInit_real32_T(), argInit_real32_T(), b_yn, W, en);
  emxDestroyArray_real32_T(en);
  emxDestroyArray_real32_T(W);
  emxDestroyArray_real32_T(b_yn);
  emxDestroyArray_real32_T(dn);
  emxDestroyArray_real32_T(xn);
}

//
// Arguments    : void
// Return Type  : void
//
static void main_delayEstimation()
{
  emxArray_real32_T *farEndSound;
  emxArray_real32_T *nearEndSound;
  float delay;

  // Initialize function 'delayEstimation' input arguments.
  // Initialize function input argument 'farEndSound'.
  farEndSound = argInit_Unboundedx1_real32_T();

  // Initialize function input argument 'nearEndSound'.
  nearEndSound = argInit_Unboundedx1_real32_T();

  // Call the entry-point 'delayEstimation'.
  delay = delayEstimation(farEndSound, nearEndSound);
  emxDestroyArray_real32_T(nearEndSound);
  emxDestroyArray_real32_T(farEndSound);
}

//
// Arguments    : int argc
//                const char * const argv[]
// Return Type  : int
//
int main(int, const char * const [])
{
  // Initialize the application.
  // You do not need to do this more than one time.
  delayEstimation_initialize();

  // Invoke the entry-point functions.
  // You can call entry-point functions multiple times.
  main_delayEstimation();
  main_LMS();
  main_NLMS();
  main_RLS();

  // Terminate the application.
  // You do not need to do this more than one time.
  delayEstimation_terminate();
  return 0;
}

//
// File trailer for main.cpp
//
// [EOF]
//
