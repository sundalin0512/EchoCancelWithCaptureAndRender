//
// File: delayEstimation.cpp
//
// MATLAB Coder version            : 3.3
// C/C++ source code generated on  : 27-Apr-2017 21:26:58
//

// Include Files
#include "rt_nonfinite.h"
#include "delayEstimation.h"
#include "delayEstimation_emxutil.h"

// Function Declarations
static void b_r2br_r2dit_trig(const emxArray_creal32_T *x, int n1_unsigned,
  const emxArray_real32_T *costab, const emxArray_real32_T *sintab,
  emxArray_creal32_T *y);
static void crosscorr(const emxArray_real32_T *x, const emxArray_real32_T *y,
                      double maxlag, emxArray_real32_T *c);
static int div_s32_floor(int numerator, int denominator);
static void fft(const emxArray_real32_T *x, double varargin_1,
                emxArray_creal32_T *y);
static void generate_twiddle_tables(int nRows, boolean_T useRadix2,
  emxArray_real32_T *costab, emxArray_real32_T *sintab, emxArray_real32_T
  *sintabinv);
static void get_algo_sizes(int n1, boolean_T useRadix2, int *N2blue, int *nRows);
static void ifft(const emxArray_creal32_T *x, emxArray_creal32_T *y);
static void r2br_r2dit_trig(const emxArray_creal32_T *x, int n1_unsigned, const
  emxArray_real32_T *costab, const emxArray_real32_T *sintab, emxArray_creal32_T
  *y);
static void r2br_r2dit_trig_impl(const emxArray_creal32_T *x, int xoffInit, int
  unsigned_nRows, const emxArray_real32_T *costab, const emxArray_real32_T
  *sintab, emxArray_creal32_T *y);
static double rt_powd_snf(double u0, double u1);

// Function Definitions

//
// Arguments    : const emxArray_creal32_T *x
//                int n1_unsigned
//                const emxArray_real32_T *costab
//                const emxArray_real32_T *sintab
//                emxArray_creal32_T *y
// Return Type  : void
//
static void b_r2br_r2dit_trig(const emxArray_creal32_T *x, int n1_unsigned,
  const emxArray_real32_T *costab, const emxArray_real32_T *sintab,
  emxArray_creal32_T *y)
{
  int j;
  int nRowsD2;
  int nRowsD4;
  int iDelta2;
  int iy;
  int ix;
  int ju;
  int i;
  boolean_T tst;
  float temp_re;
  float temp_im;
  float r;
  float twid_im;
  int ihi;
  j = x->size[0];
  if (!(j < n1_unsigned)) {
    j = n1_unsigned;
  }

  nRowsD2 = n1_unsigned / 2;
  nRowsD4 = nRowsD2 / 2;
  iDelta2 = y->size[0];
  y->size[0] = n1_unsigned;
  emxEnsureCapacity((emxArray__common *)y, iDelta2, sizeof(creal32_T));
  if (n1_unsigned > x->size[0]) {
    iy = y->size[0];
    iDelta2 = y->size[0];
    y->size[0] = iy;
    emxEnsureCapacity((emxArray__common *)y, iDelta2, sizeof(creal32_T));
    for (iDelta2 = 0; iDelta2 < iy; iDelta2++) {
      y->data[iDelta2].re = 0.0F;
      y->data[iDelta2].im = 0.0F;
    }
  }

  ix = 0;
  ju = 0;
  iy = 0;
  for (i = 1; i < j; i++) {
    y->data[iy] = x->data[ix];
    iDelta2 = n1_unsigned;
    tst = true;
    while (tst) {
      iDelta2 >>= 1;
      ju ^= iDelta2;
      tst = ((ju & iDelta2) == 0);
    }

    iy = ju;
    ix++;
  }

  y->data[iy] = x->data[ix];
  if (n1_unsigned > 1) {
    for (i = 0; i <= n1_unsigned - 2; i += 2) {
      temp_re = y->data[i + 1].re;
      temp_im = y->data[i + 1].im;
      y->data[i + 1].re = y->data[i].re - y->data[i + 1].re;
      y->data[i + 1].im = y->data[i].im - y->data[i + 1].im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }
  }

  iy = 2;
  iDelta2 = 4;
  ix = 1 + ((nRowsD4 - 1) << 2);
  while (nRowsD4 > 0) {
    for (i = 0; i < ix; i += iDelta2) {
      temp_re = y->data[i + iy].re;
      temp_im = y->data[i + iy].im;
      y->data[i + iy].re = y->data[i].re - temp_re;
      y->data[i + iy].im = y->data[i].im - temp_im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }

    ju = 1;
    for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
      r = costab->data[j];
      twid_im = sintab->data[j];
      i = ju;
      ihi = ju + ix;
      while (i < ihi) {
        temp_re = r * y->data[i + iy].re - twid_im * y->data[i + iy].im;
        temp_im = r * y->data[i + iy].im + twid_im * y->data[i + iy].re;
        y->data[i + iy].re = y->data[i].re - temp_re;
        y->data[i + iy].im = y->data[i].im - temp_im;
        y->data[i].re += temp_re;
        y->data[i].im += temp_im;
        i += iDelta2;
      }

      ju++;
    }

    nRowsD4 /= 2;
    iy = iDelta2;
    iDelta2 += iDelta2;
    ix -= iy;
  }

  if (y->size[0] > 1) {
    r = 1.0F / (float)y->size[0];
    iDelta2 = y->size[0];
    emxEnsureCapacity((emxArray__common *)y, iDelta2, sizeof(creal32_T));
    iy = y->size[0];
    for (iDelta2 = 0; iDelta2 < iy; iDelta2++) {
      y->data[iDelta2].re *= r;
      y->data[iDelta2].im *= r;
    }
  }
}

//
// Arguments    : const emxArray_real32_T *x
//                const emxArray_real32_T *y
//                double maxlag
//                emxArray_real32_T *c
// Return Type  : void
//
static void crosscorr(const emxArray_real32_T *x, const emxArray_real32_T *y,
                      double maxlag, emxArray_real32_T *c)
{
  int u0;
  int m;
  double mxl;
  double c0;
  int maxmn;
  double ceilLog2;
  double m2;
  int n;
  double Sn;
  emxArray_creal32_T *X;
  emxArray_creal32_T *Y;
  emxArray_creal32_T *b_X;
  int loop_ub;
  float Y_re;
  emxArray_real32_T *c1;
  float s;
  float X_re;
  float X_im;
  emxArray_real_T *r0;
  u0 = x->size[0];
  m = y->size[0];
  if (u0 > m) {
    m = u0;
  }

  mxl = (double)m - 1.0;
  if (maxlag < mxl) {
    mxl = maxlag;
  }

  c0 = frexp((double)(unsigned int)std::abs(2.0 * (double)m - 1.0), &maxmn);
  ceilLog2 = maxmn;
  if (c0 == 0.5) {
    ceilLog2 = (double)maxmn - 1.0;
  }

  m2 = rt_powd_snf(2.0, ceilLog2);
  u0 = x->size[0];
  m = y->size[0];
  if (u0 > m) {
    m = u0;
  }

  u0 = x->size[0];
  n = y->size[0];
  if (u0 < n) {
    n = u0;
  }

  c0 = 2.0 * (double)n - 1.0;
  if (mxl <= (double)n - 1.0) {
    Sn = mxl * ((c0 - mxl) - 1.0);
    if (mxl <= m - n) {
      c0 = (c0 + mxl * c0) + Sn;
    } else {
      c0 = ((c0 + (double)(m - n) * c0) + (mxl - (double)(m - n)) * ((((double)m
               - mxl) + (double)n) - 2.0)) + Sn;
    }
  } else if (mxl <= (double)m - 1.0) {
    Sn = ((double)n - 1.0) * ((double)n - 1.0);
    if (mxl <= m - n) {
      c0 = (c0 + mxl * c0) + Sn;
    } else {
      c0 = ((c0 + (double)(m - n) * c0) + (mxl - (double)(m - n)) * ((((double)m
               - mxl) + (double)n) - 2.0)) + Sn;
    }
  } else {
    c0 = 2.0 * (double)m * (double)n - ((double)((unsigned int)m + n) - 1.0);
  }

  if (c0 < m2 * (15.0 * ceilLog2 + 6.0)) {
    m = x->size[0];
    n = y->size[0];
    u0 = x->size[0];
    maxmn = y->size[0];
    if (u0 > maxmn) {
      maxmn = u0;
    }

    m2 = (double)maxmn - 1.0;
    if (mxl < m2) {
      m2 = mxl;
    }

    c0 = 2.0 * m2 + 1.0;
    u0 = c->size[0];
    c->size[0] = (int)c0;
    emxEnsureCapacity((emxArray__common *)c, u0, sizeof(float));
    loop_ub = (int)c0;
    for (u0 = 0; u0 < loop_ub; u0++) {
      c->data[u0] = 0.0F;
    }

    for (maxmn = 0; maxmn < (int)(m2 + 1.0); maxmn++) {
      c0 = (double)m - (double)maxmn;
      Sn = n;
      if (c0 < Sn) {
        Sn = c0;
      }

      s = 0.0F;
      for (u0 = 0; u0 < (int)Sn; u0++) {
        Y_re = x->data[(int)((double)maxmn + (1.0 + (double)u0)) - 1];
        s += y->data[u0] * Y_re;
      }

      c->data[(int)((m2 + (double)maxmn) + 1.0) - 1] = s;
    }

    for (maxmn = 0; maxmn < (int)m2; maxmn++) {
      Sn = (double)n - (1.0 + (double)maxmn);
      c0 = m;
      if (c0 < Sn) {
        Sn = c0;
      }

      s = 0.0F;
      for (u0 = 0; u0 < (int)Sn; u0++) {
        Y_re = y->data[(int)((1.0 + (double)maxmn) + (1.0 + (double)u0)) - 1];
        s += Y_re * x->data[u0];
      }

      c->data[(int)((m2 - (1.0 + (double)maxmn)) + 1.0) - 1] = s;
    }
  } else {
    emxInit_creal32_T(&X, 1);
    emxInit_creal32_T(&Y, 1);
    emxInit_creal32_T(&b_X, 1);
    fft(x, m2, X);
    fft(y, m2, Y);
    u0 = b_X->size[0];
    b_X->size[0] = X->size[0];
    emxEnsureCapacity((emxArray__common *)b_X, u0, sizeof(creal32_T));
    loop_ub = X->size[0];
    for (u0 = 0; u0 < loop_ub; u0++) {
      Y_re = Y->data[u0].re;
      s = -Y->data[u0].im;
      X_re = X->data[u0].re;
      X_im = X->data[u0].im;
      b_X->data[u0].re = X_re * Y_re - X_im * s;
      b_X->data[u0].im = X_re * s + X_im * Y_re;
    }

    emxFree_creal32_T(&Y);
    emxInit_real32_T(&c1, 1);
    ifft(b_X, X);
    u0 = c1->size[0];
    c1->size[0] = X->size[0];
    emxEnsureCapacity((emxArray__common *)c1, u0, sizeof(float));
    loop_ub = X->size[0];
    emxFree_creal32_T(&b_X);
    for (u0 = 0; u0 < loop_ub; u0++) {
      c1->data[u0] = X->data[u0].re;
    }

    emxFree_creal32_T(&X);
    emxInit_real_T(&r0, 2);
    if (mxl < 1.0) {
      u0 = r0->size[0] * r0->size[1];
      r0->size[0] = 1;
      r0->size[1] = 0;
      emxEnsureCapacity((emxArray__common *)r0, u0, sizeof(double));
    } else {
      u0 = r0->size[0] * r0->size[1];
      r0->size[0] = 1;
      r0->size[1] = (int)std::floor(mxl - 1.0) + 1;
      emxEnsureCapacity((emxArray__common *)r0, u0, sizeof(double));
      loop_ub = (int)std::floor(mxl - 1.0);
      for (u0 = 0; u0 <= loop_ub; u0++) {
        r0->data[r0->size[0] * u0] = 1.0 + (double)u0;
      }
    }

    if (1.0 > mxl + 1.0) {
      loop_ub = 0;
    } else {
      loop_ub = (int)(mxl + 1.0);
    }

    m2 -= mxl;
    u0 = c->size[0];
    c->size[0] = r0->size[1] + loop_ub;
    emxEnsureCapacity((emxArray__common *)c, u0, sizeof(float));
    maxmn = r0->size[1];
    for (u0 = 0; u0 < maxmn; u0++) {
      c->data[u0] = c1->data[(int)(m2 + r0->data[r0->size[0] * u0]) - 1];
    }

    for (u0 = 0; u0 < loop_ub; u0++) {
      c->data[u0 + r0->size[1]] = c1->data[u0];
    }

    emxFree_real_T(&r0);
    emxFree_real32_T(&c1);
  }
}

//
// Arguments    : int numerator
//                int denominator
// Return Type  : int
//
static int div_s32_floor(int numerator, int denominator)
{
  int quotient;
  unsigned int absNumerator;
  unsigned int absDenominator;
  boolean_T quotientNeedsNegation;
  unsigned int tempAbsQuotient;
  if (denominator == 0) {
    if (numerator >= 0) {
      quotient = MAX_int32_T;
    } else {
      quotient = MIN_int32_T;
    }
  } else {
    if (numerator < 0) {
      absNumerator = ~(unsigned int)numerator + 1U;
    } else {
      absNumerator = (unsigned int)numerator;
    }

    if (denominator < 0) {
      absDenominator = ~(unsigned int)denominator + 1U;
    } else {
      absDenominator = (unsigned int)denominator;
    }

    quotientNeedsNegation = ((numerator < 0) != (denominator < 0));
    tempAbsQuotient = absNumerator / absDenominator;
    if (quotientNeedsNegation) {
      absNumerator %= absDenominator;
      if (absNumerator > 0U) {
        tempAbsQuotient++;
      }

      quotient = -(int)tempAbsQuotient;
    } else {
      quotient = (int)tempAbsQuotient;
    }
  }

  return quotient;
}

//
// Arguments    : const emxArray_real32_T *x
//                double varargin_1
//                emxArray_creal32_T *y
// Return Type  : void
//
static void fft(const emxArray_real32_T *x, double varargin_1,
                emxArray_creal32_T *y)
{
  emxArray_real32_T *costab;
  emxArray_real32_T *sintab;
  emxArray_real32_T *sintabinv;
  emxArray_creal32_T *wwc;
  emxArray_creal32_T *fy;
  emxArray_creal32_T *fv;
  boolean_T useRadix2;
  int nInt2;
  int N2blue;
  int idx;
  int nInt2m1;
  int ihi;
  int nRowsD2;
  int rt;
  int nRowsD4;
  int j;
  float nt_im;
  float nt_re;
  float temp_re;
  float temp_im;
  float fv_re;
  float fv_im;
  float wwc_im;
  float b_fv_re;
  emxInit_real32_T1(&costab, 2);
  emxInit_real32_T1(&sintab, 2);
  emxInit_real32_T1(&sintabinv, 2);
  emxInit_creal32_T(&wwc, 1);
  emxInit_creal32_T(&fy, 1);
  emxInit_creal32_T(&fv, 1);
  if ((x->size[0] == 0) || ((int)varargin_1 == 0)) {
    nInt2 = y->size[0];
    y->size[0] = (int)varargin_1;
    emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
    if ((int)varargin_1 > x->size[0]) {
      ihi = y->size[0];
      nInt2 = y->size[0];
      y->size[0] = ihi;
      emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
      for (nInt2 = 0; nInt2 < ihi; nInt2++) {
        y->data[nInt2].re = 0.0F;
        y->data[nInt2].im = 0.0F;
      }
    }
  } else {
    useRadix2 = ((!((int)varargin_1 <= 0)) && (((int)varargin_1 & ((int)
      varargin_1 - 1)) == 0));
    get_algo_sizes((int)varargin_1, useRadix2, &N2blue, &idx);
    generate_twiddle_tables(idx, useRadix2, costab, sintab, sintabinv);
    if (useRadix2) {
      idx = x->size[0];
      nInt2m1 = (int)varargin_1;
      if (idx < nInt2m1) {
        nInt2m1 = idx;
      }

      nRowsD2 = (int)varargin_1 / 2;
      nRowsD4 = nRowsD2 / 2;
      nInt2 = y->size[0];
      y->size[0] = (int)varargin_1;
      emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
      if ((int)varargin_1 > x->size[0]) {
        ihi = y->size[0];
        nInt2 = y->size[0];
        y->size[0] = ihi;
        emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
        for (nInt2 = 0; nInt2 < ihi; nInt2++) {
          y->data[nInt2].re = 0.0F;
          y->data[nInt2].im = 0.0F;
        }
      }

      rt = 0;
      nInt2 = 0;
      idx = 0;
      for (N2blue = 1; N2blue < nInt2m1; N2blue++) {
        y->data[idx].re = x->data[rt];
        y->data[idx].im = 0.0F;
        idx = (int)varargin_1;
        useRadix2 = true;
        while (useRadix2) {
          idx >>= 1;
          nInt2 ^= idx;
          useRadix2 = ((nInt2 & idx) == 0);
        }

        idx = nInt2;
        rt++;
      }

      y->data[idx].re = x->data[rt];
      y->data[idx].im = 0.0F;
      if ((int)varargin_1 > 1) {
        for (N2blue = 0; N2blue <= (int)varargin_1 - 2; N2blue += 2) {
          temp_re = y->data[N2blue + 1].re;
          temp_im = y->data[N2blue + 1].im;
          y->data[N2blue + 1].re = y->data[N2blue].re - y->data[N2blue + 1].re;
          y->data[N2blue + 1].im = y->data[N2blue].im - y->data[N2blue + 1].im;
          y->data[N2blue].re += temp_re;
          y->data[N2blue].im += temp_im;
        }
      }

      idx = 2;
      rt = 4;
      nInt2 = 1 + ((nRowsD4 - 1) << 2);
      while (nRowsD4 > 0) {
        for (N2blue = 0; N2blue < nInt2; N2blue += rt) {
          temp_re = y->data[N2blue + idx].re;
          temp_im = y->data[N2blue + idx].im;
          y->data[N2blue + idx].re = y->data[N2blue].re - temp_re;
          y->data[N2blue + idx].im = y->data[N2blue].im - temp_im;
          y->data[N2blue].re += temp_re;
          y->data[N2blue].im += temp_im;
        }

        nInt2m1 = 1;
        for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
          nt_re = costab->data[j];
          nt_im = sintab->data[j];
          N2blue = nInt2m1;
          ihi = nInt2m1 + nInt2;
          while (N2blue < ihi) {
            temp_re = nt_re * y->data[N2blue + idx].re - nt_im * y->data[N2blue
              + idx].im;
            temp_im = nt_re * y->data[N2blue + idx].im + nt_im * y->data[N2blue
              + idx].re;
            y->data[N2blue + idx].re = y->data[N2blue].re - temp_re;
            y->data[N2blue + idx].im = y->data[N2blue].im - temp_im;
            y->data[N2blue].re += temp_re;
            y->data[N2blue].im += temp_im;
            N2blue += rt;
          }

          nInt2m1++;
        }

        nRowsD4 /= 2;
        idx = rt;
        rt += rt;
        nInt2 -= idx;
      }
    } else {
      nInt2m1 = ((int)varargin_1 + (int)varargin_1) - 1;
      nInt2 = wwc->size[0];
      wwc->size[0] = nInt2m1;
      emxEnsureCapacity((emxArray__common *)wwc, nInt2, sizeof(creal32_T));
      idx = (int)varargin_1;
      rt = 0;
      wwc->data[(int)varargin_1 - 1].re = 1.0F;
      wwc->data[(int)varargin_1 - 1].im = 0.0F;
      nInt2 = (int)varargin_1 << 1;
      for (j = 1; j < (int)varargin_1; j++) {
        ihi = (j << 1) - 1;
        if (nInt2 - rt <= ihi) {
          rt += ihi - nInt2;
        } else {
          rt += ihi;
        }

        nt_im = -3.14159274F * (float)rt / (float)(int)varargin_1;
        if (nt_im == 0.0F) {
          nt_re = 1.0F;
          nt_im = 0.0F;
        } else {
          nt_re = std::cos(nt_im);
          nt_im = std::sin(nt_im);
        }

        wwc->data[idx - 2].re = nt_re;
        wwc->data[idx - 2].im = -nt_im;
        idx--;
      }

      idx = 0;
      for (j = nInt2m1 - 1; j >= (int)varargin_1; j--) {
        wwc->data[j] = wwc->data[idx];
        idx++;
      }

      idx = (int)varargin_1;
      rt = x->size[0];
      if (idx < rt) {
        rt = idx;
      }

      nInt2 = y->size[0];
      y->size[0] = (int)varargin_1;
      emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
      if ((int)varargin_1 > x->size[0]) {
        ihi = y->size[0];
        nInt2 = y->size[0];
        y->size[0] = ihi;
        emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
        for (nInt2 = 0; nInt2 < ihi; nInt2++) {
          y->data[nInt2].re = 0.0F;
          y->data[nInt2].im = 0.0F;
        }
      }

      idx = 0;
      for (j = 0; j + 1 <= rt; j++) {
        nt_re = wwc->data[((int)varargin_1 + j) - 1].re;
        nt_im = wwc->data[((int)varargin_1 + j) - 1].im;
        y->data[j].re = nt_re * x->data[idx];
        y->data[j].im = nt_im * -x->data[idx];
        idx++;
      }

      while (rt + 1 <= (int)varargin_1) {
        y->data[rt].re = 0.0F;
        y->data[rt].im = 0.0F;
        rt++;
      }

      r2br_r2dit_trig_impl(y, 0, N2blue, costab, sintab, fy);
      r2br_r2dit_trig(wwc, N2blue, costab, sintab, fv);
      nInt2 = fy->size[0];
      emxEnsureCapacity((emxArray__common *)fy, nInt2, sizeof(creal32_T));
      idx = fy->size[0];
      for (nInt2 = 0; nInt2 < idx; nInt2++) {
        nt_re = fy->data[nInt2].re;
        nt_im = fy->data[nInt2].im;
        fv_re = fv->data[nInt2].re;
        fv_im = fv->data[nInt2].im;
        fy->data[nInt2].re = nt_re * fv_re - nt_im * fv_im;
        fy->data[nInt2].im = nt_re * fv_im + nt_im * fv_re;
      }

      b_r2br_r2dit_trig(fy, N2blue, costab, sintabinv, fv);
      idx = 0;
      for (j = (int)(float)(int)varargin_1 - 1; j + 1 <= wwc->size[0]; j++) {
        nt_re = wwc->data[j].re;
        fv_re = fv->data[j].re;
        nt_im = wwc->data[j].im;
        fv_im = fv->data[j].im;
        temp_re = wwc->data[j].re;
        temp_im = fv->data[j].im;
        wwc_im = wwc->data[j].im;
        b_fv_re = fv->data[j].re;
        y->data[idx].re = nt_re * fv_re + nt_im * fv_im;
        y->data[idx].im = temp_re * temp_im - wwc_im * b_fv_re;
        idx++;
      }
    }
  }

  emxFree_creal32_T(&fv);
  emxFree_creal32_T(&fy);
  emxFree_creal32_T(&wwc);
  emxFree_real32_T(&sintabinv);
  emxFree_real32_T(&sintab);
  emxFree_real32_T(&costab);
}

//
// Arguments    : int nRows
//                boolean_T useRadix2
//                emxArray_real32_T *costab
//                emxArray_real32_T *sintab
//                emxArray_real32_T *sintabinv
// Return Type  : void
//
static void generate_twiddle_tables(int nRows, boolean_T useRadix2,
  emxArray_real32_T *costab, emxArray_real32_T *sintab, emxArray_real32_T
  *sintabinv)
{
  emxArray_real32_T *costab1q;
  float e;
  int nRowsD4;
  int nd2;
  int k;
  int n2;
  emxInit_real32_T1(&costab1q, 2);
  e = 6.28318548F / (float)nRows;
  nRowsD4 = nRows / 2 / 2;
  nd2 = costab1q->size[0] * costab1q->size[1];
  costab1q->size[0] = 1;
  costab1q->size[1] = nRowsD4 + 1;
  emxEnsureCapacity((emxArray__common *)costab1q, nd2, sizeof(float));
  costab1q->data[0] = 1.0F;
  nd2 = nRowsD4 / 2;
  for (k = 1; k <= nd2; k++) {
    costab1q->data[k] = std::cos(e * (float)k);
  }

  for (k = nd2 + 1; k < nRowsD4; k++) {
    costab1q->data[k] = std::sin(e * (float)(nRowsD4 - k));
  }

  costab1q->data[nRowsD4] = 0.0F;
  if (!useRadix2) {
    nRowsD4 = costab1q->size[1] - 1;
    n2 = (costab1q->size[1] - 1) << 1;
    nd2 = costab->size[0] * costab->size[1];
    costab->size[0] = 1;
    costab->size[1] = n2 + 1;
    emxEnsureCapacity((emxArray__common *)costab, nd2, sizeof(float));
    nd2 = sintab->size[0] * sintab->size[1];
    sintab->size[0] = 1;
    sintab->size[1] = n2 + 1;
    emxEnsureCapacity((emxArray__common *)sintab, nd2, sizeof(float));
    costab->data[0] = 1.0F;
    sintab->data[0] = 0.0F;
    nd2 = sintabinv->size[0] * sintabinv->size[1];
    sintabinv->size[0] = 1;
    sintabinv->size[1] = n2 + 1;
    emxEnsureCapacity((emxArray__common *)sintabinv, nd2, sizeof(float));
    for (k = 1; k <= nRowsD4; k++) {
      sintabinv->data[k] = costab1q->data[nRowsD4 - k];
    }

    for (k = costab1q->size[1]; k <= n2; k++) {
      sintabinv->data[k] = costab1q->data[k - nRowsD4];
    }

    for (k = 1; k <= nRowsD4; k++) {
      costab->data[k] = costab1q->data[k];
      sintab->data[k] = -costab1q->data[nRowsD4 - k];
    }

    for (k = costab1q->size[1]; k <= n2; k++) {
      costab->data[k] = -costab1q->data[n2 - k];
      sintab->data[k] = -costab1q->data[k - nRowsD4];
    }
  } else {
    nRowsD4 = costab1q->size[1] - 1;
    n2 = (costab1q->size[1] - 1) << 1;
    nd2 = costab->size[0] * costab->size[1];
    costab->size[0] = 1;
    costab->size[1] = n2 + 1;
    emxEnsureCapacity((emxArray__common *)costab, nd2, sizeof(float));
    nd2 = sintab->size[0] * sintab->size[1];
    sintab->size[0] = 1;
    sintab->size[1] = n2 + 1;
    emxEnsureCapacity((emxArray__common *)sintab, nd2, sizeof(float));
    costab->data[0] = 1.0F;
    sintab->data[0] = 0.0F;
    for (k = 1; k <= nRowsD4; k++) {
      costab->data[k] = costab1q->data[k];
      sintab->data[k] = -costab1q->data[nRowsD4 - k];
    }

    for (k = costab1q->size[1]; k <= n2; k++) {
      costab->data[k] = -costab1q->data[n2 - k];
      sintab->data[k] = -costab1q->data[k - nRowsD4];
    }

    nd2 = sintabinv->size[0] * sintabinv->size[1];
    sintabinv->size[0] = 1;
    sintabinv->size[1] = 0;
    emxEnsureCapacity((emxArray__common *)sintabinv, nd2, sizeof(float));
  }

  emxFree_real32_T(&costab1q);
}

//
// Arguments    : int n1
//                boolean_T useRadix2
//                int *N2blue
//                int *nRows
// Return Type  : void
//
static void get_algo_sizes(int n1, boolean_T useRadix2, int *N2blue, int *nRows)
{
  int nn1m1;
  int pmax;
  int pmin;
  boolean_T exitg1;
  int p;
  int pow2p;
  *N2blue = 1;
  if (useRadix2) {
    *nRows = n1;
  } else {
    if (n1 > 0) {
      nn1m1 = (n1 + n1) - 1;
      pmax = 31;
      if (nn1m1 > MIN_int32_T) {
        if (nn1m1 < 0) {
          nn1m1 = -nn1m1;
        }

        if (nn1m1 <= 1) {
          pmax = 0;
        } else {
          pmin = 0;
          exitg1 = false;
          while ((!exitg1) && (pmax - pmin > 1)) {
            p = (pmin + pmax) >> 1;
            pow2p = 1 << p;
            if (pow2p == nn1m1) {
              pmax = p;
              exitg1 = true;
            } else if (pow2p > nn1m1) {
              pmax = p;
            } else {
              pmin = p;
            }
          }
        }
      }

      *N2blue = 1 << pmax;
    }

    *nRows = *N2blue;
  }
}

//
// Arguments    : const emxArray_creal32_T *x
//                emxArray_creal32_T *y
// Return Type  : void
//
static void ifft(const emxArray_creal32_T *x, emxArray_creal32_T *y)
{
  int n1;
  emxArray_real32_T *costab1q;
  int nInt2;
  boolean_T useRadix2;
  int N2blue;
  int nd2;
  float e;
  int nRowsD4;
  int k;
  emxArray_real32_T *costab;
  emxArray_real32_T *sintab;
  emxArray_real32_T *sintabinv;
  emxArray_creal32_T *wwc;
  int nInt2m1;
  int b_y;
  float denom_re;
  emxArray_creal32_T *fy;
  float wwc_im;
  emxArray_creal32_T *fv;
  float wwc_re;
  float fv_im;
  float b_wwc_im;
  float fv_re;
  float b_fv_im;
  float b_fv_re;
  n1 = x->size[0];
  if (x->size[0] == 0) {
    nInt2 = y->size[0];
    y->size[0] = 0;
    emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
  } else {
    emxInit_real32_T1(&costab1q, 2);
    useRadix2 = ((x->size[0] & (x->size[0] - 1)) == 0);
    get_algo_sizes(x->size[0], useRadix2, &N2blue, &nd2);
    e = 6.28318548F / (float)nd2;
    nRowsD4 = nd2 / 2 / 2;
    nInt2 = costab1q->size[0] * costab1q->size[1];
    costab1q->size[0] = 1;
    costab1q->size[1] = nRowsD4 + 1;
    emxEnsureCapacity((emxArray__common *)costab1q, nInt2, sizeof(float));
    costab1q->data[0] = 1.0F;
    nd2 = nRowsD4 / 2;
    for (k = 1; k <= nd2; k++) {
      costab1q->data[k] = std::cos(e * (float)k);
    }

    for (k = nd2 + 1; k < nRowsD4; k++) {
      costab1q->data[k] = std::sin(e * (float)(nRowsD4 - k));
    }

    costab1q->data[nRowsD4] = 0.0F;
    emxInit_real32_T1(&costab, 2);
    emxInit_real32_T1(&sintab, 2);
    emxInit_real32_T1(&sintabinv, 2);
    if (!useRadix2) {
      nd2 = costab1q->size[1] - 1;
      nRowsD4 = (costab1q->size[1] - 1) << 1;
      nInt2 = costab->size[0] * costab->size[1];
      costab->size[0] = 1;
      costab->size[1] = nRowsD4 + 1;
      emxEnsureCapacity((emxArray__common *)costab, nInt2, sizeof(float));
      nInt2 = sintab->size[0] * sintab->size[1];
      sintab->size[0] = 1;
      sintab->size[1] = nRowsD4 + 1;
      emxEnsureCapacity((emxArray__common *)sintab, nInt2, sizeof(float));
      costab->data[0] = 1.0F;
      sintab->data[0] = 0.0F;
      nInt2 = sintabinv->size[0] * sintabinv->size[1];
      sintabinv->size[0] = 1;
      sintabinv->size[1] = nRowsD4 + 1;
      emxEnsureCapacity((emxArray__common *)sintabinv, nInt2, sizeof(float));
      for (k = 1; k <= nd2; k++) {
        sintabinv->data[k] = costab1q->data[nd2 - k];
      }

      for (k = costab1q->size[1]; k <= nRowsD4; k++) {
        sintabinv->data[k] = costab1q->data[k - nd2];
      }

      for (k = 1; k <= nd2; k++) {
        costab->data[k] = costab1q->data[k];
        sintab->data[k] = -costab1q->data[nd2 - k];
      }

      for (k = costab1q->size[1]; k <= nRowsD4; k++) {
        costab->data[k] = -costab1q->data[nRowsD4 - k];
        sintab->data[k] = -costab1q->data[k - nd2];
      }
    } else {
      nd2 = costab1q->size[1] - 1;
      nRowsD4 = (costab1q->size[1] - 1) << 1;
      nInt2 = costab->size[0] * costab->size[1];
      costab->size[0] = 1;
      costab->size[1] = nRowsD4 + 1;
      emxEnsureCapacity((emxArray__common *)costab, nInt2, sizeof(float));
      nInt2 = sintab->size[0] * sintab->size[1];
      sintab->size[0] = 1;
      sintab->size[1] = nRowsD4 + 1;
      emxEnsureCapacity((emxArray__common *)sintab, nInt2, sizeof(float));
      costab->data[0] = 1.0F;
      sintab->data[0] = 0.0F;
      for (k = 1; k <= nd2; k++) {
        costab->data[k] = costab1q->data[k];
        sintab->data[k] = costab1q->data[nd2 - k];
      }

      for (k = costab1q->size[1]; k <= nRowsD4; k++) {
        costab->data[k] = -costab1q->data[nRowsD4 - k];
        sintab->data[k] = costab1q->data[k - nd2];
      }

      nInt2 = sintabinv->size[0] * sintabinv->size[1];
      sintabinv->size[0] = 1;
      sintabinv->size[1] = 0;
      emxEnsureCapacity((emxArray__common *)sintabinv, nInt2, sizeof(float));
    }

    emxFree_real32_T(&costab1q);
    if (useRadix2) {
      b_r2br_r2dit_trig(x, x->size[0], costab, sintab, y);
    } else {
      emxInit_creal32_T(&wwc, 1);
      nInt2m1 = (x->size[0] + x->size[0]) - 1;
      nInt2 = wwc->size[0];
      wwc->size[0] = nInt2m1;
      emxEnsureCapacity((emxArray__common *)wwc, nInt2, sizeof(creal32_T));
      nd2 = x->size[0];
      nRowsD4 = 0;
      wwc->data[x->size[0] - 1].re = 1.0F;
      wwc->data[x->size[0] - 1].im = 0.0F;
      nInt2 = x->size[0] << 1;
      for (k = 1; k < n1; k++) {
        b_y = (k << 1) - 1;
        if (nInt2 - nRowsD4 <= b_y) {
          nRowsD4 += b_y - nInt2;
        } else {
          nRowsD4 += b_y;
        }

        e = 3.14159274F * (float)nRowsD4 / (float)x->size[0];
        if (e == 0.0F) {
          denom_re = 1.0F;
          e = 0.0F;
        } else {
          denom_re = std::cos(e);
          e = std::sin(e);
        }

        wwc->data[nd2 - 2].re = denom_re;
        wwc->data[nd2 - 2].im = -e;
        nd2--;
      }

      nd2 = 0;
      for (k = nInt2m1 - 1; k >= n1; k--) {
        wwc->data[k] = wwc->data[nd2];
        nd2++;
      }

      nRowsD4 = x->size[0];
      nd2 = x->size[0];
      nInt2 = y->size[0];
      y->size[0] = nd2;
      emxEnsureCapacity((emxArray__common *)y, nInt2, sizeof(creal32_T));
      nd2 = 0;
      for (k = 0; k + 1 <= nRowsD4; k++) {
        denom_re = wwc->data[(n1 + k) - 1].re;
        e = wwc->data[(n1 + k) - 1].im;
        wwc_im = x->data[nd2].re;
        wwc_re = x->data[nd2].im;
        fv_im = x->data[nd2].im;
        b_wwc_im = x->data[nd2].re;
        y->data[k].re = denom_re * wwc_im + e * wwc_re;
        y->data[k].im = denom_re * fv_im - e * b_wwc_im;
        nd2++;
      }

      while (nRowsD4 + 1 <= n1) {
        y->data[nRowsD4].re = 0.0F;
        y->data[nRowsD4].im = 0.0F;
        nRowsD4++;
      }

      emxInit_creal32_T(&fy, 1);
      emxInit_creal32_T(&fv, 1);
      r2br_r2dit_trig_impl(y, 0, N2blue, costab, sintab, fy);
      r2br_r2dit_trig(wwc, N2blue, costab, sintab, fv);
      nInt2 = fy->size[0];
      emxEnsureCapacity((emxArray__common *)fy, nInt2, sizeof(creal32_T));
      nd2 = fy->size[0];
      for (nInt2 = 0; nInt2 < nd2; nInt2++) {
        e = fy->data[nInt2].re;
        wwc_im = fy->data[nInt2].im;
        fv_re = fv->data[nInt2].re;
        b_fv_im = fv->data[nInt2].im;
        fy->data[nInt2].re = e * fv_re - wwc_im * b_fv_im;
        fy->data[nInt2].im = e * b_fv_im + wwc_im * fv_re;
      }

      b_r2br_r2dit_trig(fy, N2blue, costab, sintabinv, fv);
      nd2 = 0;
      denom_re = (float)x->size[0];
      k = (int)(float)x->size[0] - 1;
      emxFree_creal32_T(&fy);
      while (k + 1 <= wwc->size[0]) {
        e = wwc->data[k].re;
        fv_re = fv->data[k].re;
        wwc_im = wwc->data[k].im;
        b_fv_im = fv->data[k].im;
        wwc_re = wwc->data[k].re;
        fv_im = fv->data[k].im;
        b_wwc_im = wwc->data[k].im;
        b_fv_re = fv->data[k].re;
        y->data[nd2].re = e * fv_re + wwc_im * b_fv_im;
        y->data[nd2].im = wwc_re * fv_im - b_wwc_im * b_fv_re;
        e = wwc->data[k].re;
        fv_re = fv->data[k].re;
        wwc_im = wwc->data[k].im;
        b_fv_im = fv->data[k].im;
        wwc_re = wwc->data[k].re;
        fv_im = fv->data[k].im;
        b_wwc_im = wwc->data[k].im;
        b_fv_re = fv->data[k].re;
        y->data[nd2].re = e * fv_re + wwc_im * b_fv_im;
        y->data[nd2].im = wwc_re * fv_im - b_wwc_im * b_fv_re;
        e = y->data[nd2].re;
        wwc_im = y->data[nd2].im;
        if (wwc_im == 0.0F) {
          y->data[nd2].re = e / denom_re;
          y->data[nd2].im = 0.0F;
        } else if (e == 0.0F) {
          y->data[nd2].re = 0.0F;
          y->data[nd2].im = wwc_im / denom_re;
        } else {
          y->data[nd2].re = e / denom_re;
          y->data[nd2].im = wwc_im / denom_re;
        }

        nd2++;
        k++;
      }

      emxFree_creal32_T(&fv);
      emxFree_creal32_T(&wwc);
    }

    emxFree_real32_T(&sintabinv);
    emxFree_real32_T(&sintab);
    emxFree_real32_T(&costab);
  }
}

//
// Arguments    : const emxArray_creal32_T *x
//                int n1_unsigned
//                const emxArray_real32_T *costab
//                const emxArray_real32_T *sintab
//                emxArray_creal32_T *y
// Return Type  : void
//
static void r2br_r2dit_trig(const emxArray_creal32_T *x, int n1_unsigned, const
  emxArray_real32_T *costab, const emxArray_real32_T *sintab, emxArray_creal32_T
  *y)
{
  int j;
  int nRowsD2;
  int nRowsD4;
  int iy;
  int iDelta;
  int ix;
  int ju;
  int i;
  boolean_T tst;
  float temp_re;
  float temp_im;
  float twid_re;
  float twid_im;
  int ihi;
  j = x->size[0];
  if (!(j < n1_unsigned)) {
    j = n1_unsigned;
  }

  nRowsD2 = n1_unsigned / 2;
  nRowsD4 = nRowsD2 / 2;
  iy = y->size[0];
  y->size[0] = n1_unsigned;
  emxEnsureCapacity((emxArray__common *)y, iy, sizeof(creal32_T));
  if (n1_unsigned > x->size[0]) {
    iDelta = y->size[0];
    iy = y->size[0];
    y->size[0] = iDelta;
    emxEnsureCapacity((emxArray__common *)y, iy, sizeof(creal32_T));
    for (iy = 0; iy < iDelta; iy++) {
      y->data[iy].re = 0.0F;
      y->data[iy].im = 0.0F;
    }
  }

  ix = 0;
  ju = 0;
  iy = 0;
  for (i = 1; i < j; i++) {
    y->data[iy] = x->data[ix];
    iDelta = n1_unsigned;
    tst = true;
    while (tst) {
      iDelta >>= 1;
      ju ^= iDelta;
      tst = ((ju & iDelta) == 0);
    }

    iy = ju;
    ix++;
  }

  y->data[iy] = x->data[ix];
  if (n1_unsigned > 1) {
    for (i = 0; i <= n1_unsigned - 2; i += 2) {
      temp_re = y->data[i + 1].re;
      temp_im = y->data[i + 1].im;
      y->data[i + 1].re = y->data[i].re - y->data[i + 1].re;
      y->data[i + 1].im = y->data[i].im - y->data[i + 1].im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }
  }

  iDelta = 2;
  iy = 4;
  ix = 1 + ((nRowsD4 - 1) << 2);
  while (nRowsD4 > 0) {
    for (i = 0; i < ix; i += iy) {
      temp_re = y->data[i + iDelta].re;
      temp_im = y->data[i + iDelta].im;
      y->data[i + iDelta].re = y->data[i].re - temp_re;
      y->data[i + iDelta].im = y->data[i].im - temp_im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }

    ju = 1;
    for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
      twid_re = costab->data[j];
      twid_im = sintab->data[j];
      i = ju;
      ihi = ju + ix;
      while (i < ihi) {
        temp_re = twid_re * y->data[i + iDelta].re - twid_im * y->data[i +
          iDelta].im;
        temp_im = twid_re * y->data[i + iDelta].im + twid_im * y->data[i +
          iDelta].re;
        y->data[i + iDelta].re = y->data[i].re - temp_re;
        y->data[i + iDelta].im = y->data[i].im - temp_im;
        y->data[i].re += temp_re;
        y->data[i].im += temp_im;
        i += iy;
      }

      ju++;
    }

    nRowsD4 /= 2;
    iDelta = iy;
    iy += iy;
    ix -= iDelta;
  }
}

//
// Arguments    : const emxArray_creal32_T *x
//                int xoffInit
//                int unsigned_nRows
//                const emxArray_real32_T *costab
//                const emxArray_real32_T *sintab
//                emxArray_creal32_T *y
// Return Type  : void
//
static void r2br_r2dit_trig_impl(const emxArray_creal32_T *x, int xoffInit, int
  unsigned_nRows, const emxArray_real32_T *costab, const emxArray_real32_T
  *sintab, emxArray_creal32_T *y)
{
  int j;
  int nRowsD2;
  int nRowsD4;
  int iy;
  int iDelta;
  int ix;
  int ju;
  int i;
  boolean_T tst;
  float temp_re;
  float temp_im;
  float twid_re;
  float twid_im;
  int ihi;
  j = x->size[0];
  if (!(j < unsigned_nRows)) {
    j = unsigned_nRows;
  }

  nRowsD2 = unsigned_nRows / 2;
  nRowsD4 = nRowsD2 / 2;
  iy = y->size[0];
  y->size[0] = unsigned_nRows;
  emxEnsureCapacity((emxArray__common *)y, iy, sizeof(creal32_T));
  if (unsigned_nRows > x->size[0]) {
    iDelta = y->size[0];
    iy = y->size[0];
    y->size[0] = iDelta;
    emxEnsureCapacity((emxArray__common *)y, iy, sizeof(creal32_T));
    for (iy = 0; iy < iDelta; iy++) {
      y->data[iy].re = 0.0F;
      y->data[iy].im = 0.0F;
    }
  }

  ix = xoffInit;
  ju = 0;
  iy = 0;
  for (i = 1; i < j; i++) {
    y->data[iy] = x->data[ix];
    iDelta = unsigned_nRows;
    tst = true;
    while (tst) {
      iDelta >>= 1;
      ju ^= iDelta;
      tst = ((ju & iDelta) == 0);
    }

    iy = ju;
    ix++;
  }

  y->data[iy] = x->data[ix];
  if (unsigned_nRows > 1) {
    for (i = 0; i <= unsigned_nRows - 2; i += 2) {
      temp_re = y->data[i + 1].re;
      temp_im = y->data[i + 1].im;
      y->data[i + 1].re = y->data[i].re - y->data[i + 1].re;
      y->data[i + 1].im = y->data[i].im - y->data[i + 1].im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }
  }

  iDelta = 2;
  iy = 4;
  ix = 1 + ((nRowsD4 - 1) << 2);
  while (nRowsD4 > 0) {
    for (i = 0; i < ix; i += iy) {
      temp_re = y->data[i + iDelta].re;
      temp_im = y->data[i + iDelta].im;
      y->data[i + iDelta].re = y->data[i].re - temp_re;
      y->data[i + iDelta].im = y->data[i].im - temp_im;
      y->data[i].re += temp_re;
      y->data[i].im += temp_im;
    }

    ju = 1;
    for (j = nRowsD4; j < nRowsD2; j += nRowsD4) {
      twid_re = costab->data[j];
      twid_im = sintab->data[j];
      i = ju;
      ihi = ju + ix;
      while (i < ihi) {
        temp_re = twid_re * y->data[i + iDelta].re - twid_im * y->data[i +
          iDelta].im;
        temp_im = twid_re * y->data[i + iDelta].im + twid_im * y->data[i +
          iDelta].re;
        y->data[i + iDelta].re = y->data[i].re - temp_re;
        y->data[i + iDelta].im = y->data[i].im - temp_im;
        y->data[i].re += temp_re;
        y->data[i].im += temp_im;
        i += iy;
      }

      ju++;
    }

    nRowsD4 /= 2;
    iDelta = iy;
    iy += iy;
    ix -= iDelta;
  }
}

//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_powd_snf(double u0, double u1)
{
  double y;
  double d0;
  double d1;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = rtNaN;
  } else {
    d0 = std::abs(u0);
    d1 = std::abs(u1);
    if (rtIsInf(u1)) {
      if (d0 == 1.0) {
        y = 1.0;
      } else if (d0 > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d1 == 0.0) {
      y = 1.0;
    } else if (d1 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = std::sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > std::floor(u1))) {
      y = rtNaN;
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

//
// LMS(Least Mean Squre)算法
//  输入参数:
//      xn   输入的信号序列      (行向量)
//      dn   所期望的响应序列    (行向量)
//      M    滤波器的阶数        (标量)
//      mu   收敛因子(步长)      (标量)     要求大于0,小于xn的相关矩阵最大特征值的倒数
//      itr  迭代次数            (标量)     默认为xn的长度,M<itr<length(xn)
//  输出参数:
//      W    滤波器的权值向量     (行向量)
//           大小为M
//      en   误差序列(itr x 1)    (行向量)
//      yn   实际输出序列         (行向量)
// Arguments    : const emxArray_real32_T *xn
//                const emxArray_real32_T *dn
//                float M
//                float mu
//                float itr
//                emxArray_real32_T *b_yn
//                emxArray_real32_T *W
//                emxArray_real32_T *en
// Return Type  : void
//
void LMS(const emxArray_real32_T *xn, const emxArray_real32_T *dn, float M,
         float mu, float itr, emxArray_real32_T *b_yn, emxArray_real32_T *W,
         emxArray_real32_T *en)
{
  int i0;
  int loop_ub;
  int k;
  emxArray_real32_T *x;
  emxArray_real32_T *b;
  float b_k;
  float c;
  int i1;
  int i2;
  int i3;
  int b_W;

  //  参数个数必须为4个或5个
  // 'LMS:16' if nargin == 4
  // 'LMS:18' elseif nargin == 5
  //  5个时满足M<itr<length(xn)
  // 'LMS:19' if itr>length(xn) || itr<M
  if ((itr > xn->size[1]) || (itr < M)) {
    // 'LMS:20' itr = length(xn);
    itr = (float)xn->size[1];
  }

  //  初始化参数
  // 'LMS:28' en = zeros(itr,1);
  i0 = en->size[0];
  en->size[0] = (int)itr;
  emxEnsureCapacity((emxArray__common *)en, i0, sizeof(float));
  loop_ub = (int)itr;
  for (i0 = 0; i0 < loop_ub; i0++) {
    en->data[i0] = 0.0F;
  }

  //  误差序列,en(k)表示第k次迭代时预期输出与实际输入的误差
  // 'LMS:29' W  = zeros(1,M);
  i0 = W->size[0] * W->size[1];
  W->size[0] = 1;
  W->size[1] = (int)M;
  emxEnsureCapacity((emxArray__common *)W, i0, sizeof(float));
  loop_ub = (int)M;
  for (i0 = 0; i0 < loop_ub; i0++) {
    W->data[i0] = 0.0F;
  }

  //  每一行代表一个加权参量,每一列代表-次迭代,初始为0
  // 'LMS:30' yn = zeros(1,length(xn));
  i0 = b_yn->size[0] * b_yn->size[1];
  b_yn->size[0] = 1;
  b_yn->size[1] = (int)(float)xn->size[1];
  emxEnsureCapacity((emxArray__common *)b_yn, i0, sizeof(float));
  loop_ub = (int)(float)xn->size[1];
  for (i0 = 0; i0 < loop_ub; i0++) {
    b_yn->data[i0] = 0.0F;
  }

  //  迭代计算
  // 'LMS:32' for k = M:itr
  i0 = (int)(itr + (1.0F - M));
  k = 0;
  emxInit_real32_T1(&x, 2);
  emxInit_real32_T(&b, 1);
  while (k <= i0 - 1) {
    b_k = M + (float)k;

    //  第k次迭代
    // 'LMS:33' x = xn(k:-1:k-M+1);
    c = b_k - M;
    if (c + 1.0F > b_k) {
      i1 = 1;
      i2 = 1;
      i3 = 0;
    } else {
      i1 = (int)b_k;
      i2 = -1;
      i3 = (int)(c + 1.0F);
    }

    b_W = x->size[0] * x->size[1];
    x->size[0] = 1;
    x->size[1] = div_s32_floor(i3 - i1, i2) + 1;
    emxEnsureCapacity((emxArray__common *)x, b_W, sizeof(float));
    loop_ub = div_s32_floor(i3 - i1, i2);
    for (i3 = 0; i3 <= loop_ub; i3++) {
      x->data[x->size[0] * i3] = xn->data[(i1 + i2 * i3) - 1];
    }

    //  滤波器M个抽头的输入
    //      y = W(:,k-1)' * x';        % 滤波器的输出
    // 'LMS:35' yn(k) = W * x';
    i1 = b->size[0];
    b->size[0] = x->size[1];
    emxEnsureCapacity((emxArray__common *)b, i1, sizeof(float));
    loop_ub = x->size[1];
    for (i1 = 0; i1 < loop_ub; i1++) {
      b->data[i1] = x->data[x->size[0] * i1];
    }

    if ((W->size[1] == 1) || (b->size[0] == 1)) {
      c = 0.0F;
      for (i1 = 0; i1 < W->size[1]; i1++) {
        c += W->data[W->size[0] * i1] * b->data[i1];
      }
    } else {
      c = 0.0F;
      for (i1 = 0; i1 < W->size[1]; i1++) {
        c += W->data[W->size[0] * i1] * b->data[i1];
      }
    }

    b_yn->data[(int)b_k - 1] = c;

    //      en(k) = dn(k) - y ;        % 第k次迭代的误差
    // 'LMS:37' en(k) = dn(k) - yn(k) ;
    en->data[(int)b_k - 1] = dn->data[(int)b_k - 1] - b_yn->data[(int)b_k - 1];

    //  滤波器权值计算的迭代式
    // 'LMS:39' W = W + 2*mu*en(k)*x;
    c = 2.0F * mu * en->data[(int)b_k - 1];
    i1 = W->size[0] * W->size[1];
    W->size[0] = 1;
    emxEnsureCapacity((emxArray__common *)W, i1, sizeof(float));
    loop_ub = W->size[0];
    b_W = W->size[1];
    loop_ub *= b_W;
    for (i1 = 0; i1 < loop_ub; i1++) {
      W->data[i1] += c * x->data[i1];
    }

    k++;
  }

  emxFree_real32_T(&b);
  emxFree_real32_T(&x);
}

//
// NLMS(Normalized Least Mean Squre)算法
//  输入参数:
//      xn   输入的信号序列      (行向量)
//      dn   所期望的响应序列    (行向量)
//      M    滤波器的阶数        (标量)
//      mu   收敛因子(步长)      (标量)     要求大于0,小于xn的相关矩阵最大特征值的倒数
//      itr  迭代次数            (标量)     默认为xn的长度,M<itr<length(xn)
//  输出参数:
//      W    滤波器的权值向量     (行向量)
//           大小为M
//      en   误差序列(itr x 1)    (行向量)
//      yn   实际输出序列         (行向量)
// Arguments    : const emxArray_real32_T *xn
//                const emxArray_real32_T *dn
//                float M
//                float mu
//                float itr
//                emxArray_real32_T *b_yn
//                emxArray_real32_T *W
//                emxArray_real32_T *en
// Return Type  : void
//
void NLMS(const emxArray_real32_T *xn, const emxArray_real32_T *dn, float M,
          float mu, float itr, emxArray_real32_T *b_yn, emxArray_real32_T *W,
          emxArray_real32_T *en)
{
  int i4;
  int loop_ub;
  int k;
  emxArray_real32_T *x;
  emxArray_real32_T *b;
  float b_k;
  float c;
  int b_W;
  int i5;
  int i6;
  int i7;

  //  参数个数必须为4个或5个
  // 'NLMS:16' if nargin == 4
  // 'NLMS:18' elseif nargin == 5
  //  5个时满足M<itr<length(xn)
  // 'NLMS:19' if itr>length(xn) || itr<M
  if ((itr > xn->size[1]) || (itr < M)) {
    // 'NLMS:20' itr = length(xn);
    itr = (float)xn->size[1];
  }

  //  初始化参数
  // 'NLMS:28' en = zeros(itr,1);
  i4 = en->size[0];
  en->size[0] = (int)itr;
  emxEnsureCapacity((emxArray__common *)en, i4, sizeof(float));
  loop_ub = (int)itr;
  for (i4 = 0; i4 < loop_ub; i4++) {
    en->data[i4] = 0.0F;
  }

  //  误差序列,en(k)表示第k次迭代时预期输出与实际输入的误差
  // 'NLMS:29' W  = zeros(1,M);
  i4 = W->size[0] * W->size[1];
  W->size[0] = 1;
  W->size[1] = (int)M;
  emxEnsureCapacity((emxArray__common *)W, i4, sizeof(float));
  loop_ub = (int)M;
  for (i4 = 0; i4 < loop_ub; i4++) {
    W->data[i4] = 0.0F;
  }

  //  每一行代表一个加权参量,每一列代表-次迭代,初始为0
  // 'NLMS:30' yn = zeros(1,itr);
  i4 = b_yn->size[0] * b_yn->size[1];
  b_yn->size[0] = 1;
  b_yn->size[1] = (int)itr;
  emxEnsureCapacity((emxArray__common *)b_yn, i4, sizeof(float));
  loop_ub = (int)itr;
  for (i4 = 0; i4 < loop_ub; i4++) {
    b_yn->data[i4] = 0.0F;
  }

  //  迭代计算
  // 'NLMS:33' for k = M:itr
  i4 = (int)(itr + (1.0F - M));
  k = 0;
  emxInit_real32_T1(&x, 2);
  emxInit_real32_T(&b, 1);
  while (k <= i4 - 1) {
    b_k = M + (float)k;

    //  第k次迭代
    // 'NLMS:34' x = xn(k:-1:k-M+1);
    c = b_k - M;
    if (c + 1.0F > b_k) {
      b_W = 1;
      i5 = 1;
      i6 = 0;
    } else {
      b_W = (int)b_k;
      i5 = -1;
      i6 = (int)(c + 1.0F);
    }

    i7 = x->size[0] * x->size[1];
    x->size[0] = 1;
    x->size[1] = div_s32_floor(i6 - b_W, i5) + 1;
    emxEnsureCapacity((emxArray__common *)x, i7, sizeof(float));
    loop_ub = div_s32_floor(i6 - b_W, i5);
    for (i7 = 0; i7 <= loop_ub; i7++) {
      x->data[x->size[0] * i7] = xn->data[(b_W + i5 * i7) - 1];
    }

    //  滤波器M个抽头的输入
    // 'NLMS:35' yn(k) = W * x';
    i7 = b->size[0];
    b->size[0] = x->size[1];
    emxEnsureCapacity((emxArray__common *)b, i7, sizeof(float));
    loop_ub = x->size[1];
    for (i7 = 0; i7 < loop_ub; i7++) {
      b->data[i7] = x->data[x->size[0] * i7];
    }

    if ((W->size[1] == 1) || (b->size[0] == 1)) {
      c = 0.0F;
      for (i7 = 0; i7 < W->size[1]; i7++) {
        c += W->data[W->size[0] * i7] * b->data[i7];
      }
    } else {
      c = 0.0F;
      for (i7 = 0; i7 < W->size[1]; i7++) {
        c += W->data[W->size[0] * i7] * b->data[i7];
      }
    }

    b_yn->data[(int)b_k - 1] = c;

    //  滤波器的输出
    // 'NLMS:36' en(k) = dn(k) - yn(k) ;
    en->data[(int)b_k - 1] = dn->data[(int)b_k - 1] - b_yn->data[(int)b_k - 1];

    //  第k次迭代的误差
    //  滤波器权值计算的迭代式
    // 'NLMS:39' W = W + mu*en(k)*x/(1+x*x');
    c = mu * en->data[(int)b_k - 1];
    i7 = b->size[0];
    b->size[0] = x->size[1];
    emxEnsureCapacity((emxArray__common *)b, i7, sizeof(float));
    loop_ub = x->size[1];
    for (i7 = 0; i7 < loop_ub; i7++) {
      b->data[i7] = x->data[x->size[0] * i7];
    }

    if ((div_s32_floor(i6 - b_W, i5) + 1 == 1) || (b->size[0] == 1)) {
      b_k = 0.0F;
      for (b_W = 0; b_W < x->size[1]; b_W++) {
        b_k += x->data[x->size[0] * b_W] * b->data[b_W];
      }
    } else {
      b_k = 0.0F;
      for (b_W = 0; b_W < x->size[1]; b_W++) {
        b_k += x->data[x->size[0] * b_W] * b->data[b_W];
      }
    }

    b_W = W->size[0] * W->size[1];
    W->size[0] = 1;
    emxEnsureCapacity((emxArray__common *)W, b_W, sizeof(float));
    loop_ub = W->size[0];
    b_W = W->size[1];
    loop_ub *= b_W;
    for (b_W = 0; b_W < loop_ub; b_W++) {
      W->data[b_W] += c * x->data[b_W] / (1.0F + b_k);
    }

    k++;
  }

  emxFree_real32_T(&b);
  emxFree_real32_T(&x);
}

//
// RLS(Least Mean Squre)算法
//  输入参数:
//      xn   输入的信号序列      (行向量)
//      dn   所期望的响应序列    (行向量)
//      M    滤波器的阶数        (标量)
//      itr  迭代次数            (标量)     默认为xn的长度,M<itr<length(xn)
//  输出参数:
//      W    滤波器的权值向量     (行向量)
//           大小为M
//      en   误差序列(itr x 1)    (行向量)
//      yn   实际输出序列         (行向量)
// Arguments    : const emxArray_real32_T *xn
//                const emxArray_real32_T *dn
//                float M
//                float itr
//                emxArray_real32_T *b_yn
//                emxArray_real32_T *W
//                emxArray_real32_T *en
// Return Type  : void
//
void RLS(const emxArray_real32_T *xn, const emxArray_real32_T *dn, float M,
         float itr, emxArray_real32_T *b_yn, emxArray_real32_T *W,
         emxArray_real32_T *en)
{
  int i8;
  int ar;
  emxArray_real32_T *p;
  int d;
  int k;
  int b_p;
  int n;
  emxArray_real32_T *x;
  emxArray_real32_T *b_k;
  emxArray_real32_T *c;
  emxArray_real32_T *b_c;
  emxArray_real32_T *c_c;
  emxArray_real32_T *d_c;
  float b_n;
  float e_c;
  int i9;
  int i10;
  int i11;
  int i12;
  unsigned int unnamed_idx_0;
  int m;
  int br;
  int i13;
  int ic;
  unsigned int unnamed_idx_1;
  int ib;
  int ia;

  //  参数个数必须为4个或5个
  // 'RLS:15' if nargin == 3
  // 'RLS:17' elseif nargin == 4
  //  5个时满足M<itr<length(xn)
  // 'RLS:18' if itr>length(xn) || itr<M
  if ((itr > xn->size[1]) || (itr < M)) {
    // 'RLS:19' itr = length(xn);
    itr = (float)xn->size[1];
  }

  //  初始化参数
  // 'RLS:27' en = zeros(itr,1);
  i8 = en->size[0];
  en->size[0] = (int)itr;
  emxEnsureCapacity((emxArray__common *)en, i8, sizeof(float));
  ar = (int)itr;
  for (i8 = 0; i8 < ar; i8++) {
    en->data[i8] = 0.0F;
  }

  //  误差序列,en(n)表示第k次迭代时预期输出与实际输入的误差
  // 'RLS:28' W  = zeros(1,M);
  i8 = W->size[0] * W->size[1];
  W->size[0] = 1;
  W->size[1] = (int)M;
  emxEnsureCapacity((emxArray__common *)W, i8, sizeof(float));
  ar = (int)M;
  for (i8 = 0; i8 < ar; i8++) {
    W->data[i8] = 0.0F;
  }

  //  每一行代表一个加权参量,每一列代表-次迭代,初始为0
  // 'RLS:29' yn = zeros(1,itr);
  i8 = b_yn->size[0] * b_yn->size[1];
  b_yn->size[0] = 1;
  b_yn->size[1] = (int)itr;
  emxEnsureCapacity((emxArray__common *)b_yn, i8, sizeof(float));
  ar = (int)itr;
  for (i8 = 0; i8 < ar; i8++) {
    b_yn->data[i8] = 0.0F;
  }

  emxInit_real32_T1(&p, 2);

  // 'RLS:30' lamda = 0.98;
  // 'RLS:31' p=0.1*eye(M,M);
  d = (int)M;
  i8 = p->size[0] * p->size[1];
  p->size[0] = (int)M;
  p->size[1] = (int)M;
  emxEnsureCapacity((emxArray__common *)p, i8, sizeof(float));
  ar = (int)M * (int)M;
  for (i8 = 0; i8 < ar; i8++) {
    p->data[i8] = 0.0F;
  }

  if (d > 0) {
    for (k = 0; k + 1 <= d; k++) {
      p->data[k + p->size[0] * k] = 1.0F;
    }
  }

  i8 = p->size[0] * p->size[1];
  emxEnsureCapacity((emxArray__common *)p, i8, sizeof(float));
  d = p->size[0];
  b_p = p->size[1];
  ar = d * b_p;
  for (i8 = 0; i8 < ar; i8++) {
    p->data[i8] *= 0.1F;
  }

  //  迭代计算
  // 'RLS:33' for n = M:itr
  i8 = (int)(itr + (1.0F - M));
  n = 0;
  emxInit_real32_T(&x, 1);
  emxInit_real32_T(&b_k, 1);
  emxInit_real32_T1(&c, 2);
  emxInit_real32_T1(&b_c, 2);
  emxInit_real32_T1(&c_c, 2);
  emxInit_real32_T1(&d_c, 2);
  while (n <= i8 - 1) {
    b_n = M + (float)n;

    //  第k次迭代
    // 'RLS:34' x = xn(n:-1:n-M+1)';
    e_c = b_n - M;
    if (e_c + 1.0F > b_n) {
      i9 = 1;
      i10 = 1;
      i11 = 0;
    } else {
      i9 = (int)b_n;
      i10 = -1;
      i11 = (int)(e_c + 1.0F);
    }

    i12 = x->size[0];
    x->size[0] = div_s32_floor(i11 - i9, i10) + 1;
    emxEnsureCapacity((emxArray__common *)x, i12, sizeof(float));
    ar = div_s32_floor(i11 - i9, i10);
    for (i12 = 0; i12 <= ar; i12++) {
      x->data[i12] = xn->data[(i9 + i10 * i12) - 1];
    }

    //  滤波器M个抽头的输入
    //      y = W(:,n-1)' * x';        % 滤波器的输出
    // 'RLS:36' k = ((1/lamda)*p*x)/(1+(1/lamda)*x'*p*x);
    i12 = c->size[0] * c->size[1];
    c->size[0] = p->size[0];
    c->size[1] = p->size[1];
    emxEnsureCapacity((emxArray__common *)c, i12, sizeof(float));
    ar = p->size[0] * p->size[1];
    for (i12 = 0; i12 < ar; i12++) {
      c->data[i12] = 1.02040815F * p->data[i12];
    }

    if ((c->size[1] == 1) || (div_s32_floor(i11 - i9, i10) + 1 == 1)) {
      i12 = b_k->size[0];
      b_k->size[0] = c->size[0];
      emxEnsureCapacity((emxArray__common *)b_k, i12, sizeof(float));
      ar = c->size[0];
      for (i12 = 0; i12 < ar; i12++) {
        b_k->data[i12] = 0.0F;
        br = c->size[1];
        for (i13 = 0; i13 < br; i13++) {
          b_k->data[i12] += c->data[i12 + c->size[0] * i13] * x->data[i13];
        }
      }
    } else {
      k = c->size[1];
      unnamed_idx_0 = (unsigned int)c->size[0];
      i12 = b_k->size[0];
      b_k->size[0] = (int)unnamed_idx_0;
      emxEnsureCapacity((emxArray__common *)b_k, i12, sizeof(float));
      m = c->size[0];
      d = b_k->size[0];
      i12 = b_k->size[0];
      b_k->size[0] = d;
      emxEnsureCapacity((emxArray__common *)b_k, i12, sizeof(float));
      for (i12 = 0; i12 < d; i12++) {
        b_k->data[i12] = 0.0F;
      }

      if (c->size[0] != 0) {
        b_p = 0;
        while ((m > 0) && (b_p <= 0)) {
          for (ic = 0; ic + 1 <= m; ic++) {
            b_k->data[ic] = 0.0F;
          }

          b_p = m;
        }

        br = 0;
        b_p = 0;
        while ((m > 0) && (b_p <= 0)) {
          ar = 0;
          i12 = br + k;
          for (ib = br; ib + 1 <= i12; ib++) {
            if (xn->data[(i9 + i10 * ib) - 1] != 0.0F) {
              ia = ar;
              for (ic = 0; ic + 1 <= m; ic++) {
                ia++;
                b_k->data[ic] += xn->data[(i9 + i10 * ib) - 1] * c->data[ia - 1];
              }
            }

            ar += m;
          }

          br += k;
          b_p = m;
        }
      }
    }

    i12 = b_c->size[0] * b_c->size[1];
    b_c->size[0] = 1;
    b_c->size[1] = x->size[0];
    emxEnsureCapacity((emxArray__common *)b_c, i12, sizeof(float));
    ar = x->size[0];
    for (i12 = 0; i12 < ar; i12++) {
      b_c->data[b_c->size[0] * i12] = 1.02040815F * x->data[i12];
    }

    if ((b_c->size[1] == 1) || (p->size[0] == 1)) {
      i12 = c_c->size[0] * c_c->size[1];
      c_c->size[0] = 1;
      c_c->size[1] = p->size[1];
      emxEnsureCapacity((emxArray__common *)c_c, i12, sizeof(float));
      ar = p->size[1];
      for (i12 = 0; i12 < ar; i12++) {
        c_c->data[c_c->size[0] * i12] = 0.0F;
        br = b_c->size[1];
        for (i13 = 0; i13 < br; i13++) {
          c_c->data[c_c->size[0] * i12] += b_c->data[b_c->size[0] * i13] *
            p->data[i13 + p->size[0] * i12];
        }
      }
    } else {
      k = b_c->size[1];
      unnamed_idx_1 = (unsigned int)p->size[1];
      i12 = c_c->size[0] * c_c->size[1];
      c_c->size[0] = 1;
      c_c->size[1] = (int)unnamed_idx_1;
      emxEnsureCapacity((emxArray__common *)c_c, i12, sizeof(float));
      d = p->size[1] - 1;
      i12 = c_c->size[0] * c_c->size[1];
      c_c->size[0] = 1;
      emxEnsureCapacity((emxArray__common *)c_c, i12, sizeof(float));
      ar = c_c->size[1];
      for (i12 = 0; i12 < ar; i12++) {
        c_c->data[c_c->size[0] * i12] = 0.0F;
      }

      if (p->size[1] != 0) {
        for (b_p = 1; b_p - 1 <= d; b_p++) {
          for (ic = b_p; ic <= b_p; ic++) {
            c_c->data[ic - 1] = 0.0F;
          }
        }

        br = 0;
        for (b_p = 0; b_p <= d; b_p++) {
          ar = 0;
          i12 = br + k;
          for (ib = br; ib + 1 <= i12; ib++) {
            if (p->data[ib] != 0.0F) {
              ia = ar;
              for (ic = b_p; ic + 1 <= b_p + 1; ic++) {
                ia++;
                c_c->data[ic] += p->data[ib] * b_c->data[ia - 1];
              }
            }

            ar++;
          }

          br += k;
        }
      }
    }

    if ((c_c->size[1] == 1) || (div_s32_floor(i11 - i9, i10) + 1 == 1)) {
      e_c = 0.0F;
      for (i12 = 0; i12 < c_c->size[1]; i12++) {
        e_c += c_c->data[c_c->size[0] * i12] * x->data[i12];
      }
    } else {
      e_c = 0.0F;
      for (i12 = 0; i12 < c_c->size[1]; i12++) {
        e_c += c_c->data[c_c->size[0] * i12] * x->data[i12];
      }
    }

    i12 = b_k->size[0];
    emxEnsureCapacity((emxArray__common *)b_k, i12, sizeof(float));
    ar = b_k->size[0];
    for (i12 = 0; i12 < ar; i12++) {
      b_k->data[i12] /= 1.0F + e_c;
    }

    // 'RLS:37' en(n) = dn(n)-W*x;
    if ((W->size[1] == 1) || (div_s32_floor(i11 - i9, i10) + 1 == 1)) {
      e_c = 0.0F;
      for (i12 = 0; i12 < W->size[1]; i12++) {
        e_c += W->data[W->size[0] * i12] * x->data[i12];
      }
    } else {
      e_c = 0.0F;
      for (i12 = 0; i12 < W->size[1]; i12++) {
        e_c += W->data[W->size[0] * i12] * x->data[i12];
      }
    }

    en->data[(int)b_n - 1] = dn->data[(int)b_n - 1] - e_c;

    // 'RLS:38' W=W+(k*conj(en(n))')';
    e_c = en->data[(int)b_n - 1];
    i12 = W->size[0] * W->size[1];
    W->size[0] = 1;
    emxEnsureCapacity((emxArray__common *)W, i12, sizeof(float));
    ar = W->size[1];
    for (i12 = 0; i12 < ar; i12++) {
      W->data[W->size[0] * i12] += b_k->data[i12] * e_c;
    }

    // 'RLS:39' p=(1/lamda)*p-(1/lamda)*k*x'*p;
    i12 = c->size[0] * c->size[1];
    c->size[0] = b_k->size[0];
    c->size[1] = x->size[0];
    emxEnsureCapacity((emxArray__common *)c, i12, sizeof(float));
    ar = b_k->size[0];
    for (i12 = 0; i12 < ar; i12++) {
      br = x->size[0];
      for (i13 = 0; i13 < br; i13++) {
        c->data[i12 + c->size[0] * i13] = 1.02040815F * b_k->data[i12] * x->
          data[i13];
      }
    }

    if ((c->size[1] == 1) || (p->size[0] == 1)) {
      i12 = d_c->size[0] * d_c->size[1];
      d_c->size[0] = c->size[0];
      d_c->size[1] = p->size[1];
      emxEnsureCapacity((emxArray__common *)d_c, i12, sizeof(float));
      ar = c->size[0];
      for (i12 = 0; i12 < ar; i12++) {
        br = p->size[1];
        for (i13 = 0; i13 < br; i13++) {
          d_c->data[i12 + d_c->size[0] * i13] = 0.0F;
          d = c->size[1];
          for (b_p = 0; b_p < d; b_p++) {
            d_c->data[i12 + d_c->size[0] * i13] += c->data[i12 + c->size[0] *
              b_p] * p->data[b_p + p->size[0] * i13];
          }
        }
      }
    } else {
      k = c->size[1];
      unnamed_idx_0 = (unsigned int)c->size[0];
      unnamed_idx_1 = (unsigned int)p->size[1];
      i12 = d_c->size[0] * d_c->size[1];
      d_c->size[0] = (int)unnamed_idx_0;
      d_c->size[1] = (int)unnamed_idx_1;
      emxEnsureCapacity((emxArray__common *)d_c, i12, sizeof(float));
      m = c->size[0];
      i12 = d_c->size[0] * d_c->size[1];
      emxEnsureCapacity((emxArray__common *)d_c, i12, sizeof(float));
      ar = d_c->size[1];
      for (i12 = 0; i12 < ar; i12++) {
        br = d_c->size[0];
        for (i13 = 0; i13 < br; i13++) {
          d_c->data[i13 + d_c->size[0] * i12] = 0.0F;
        }
      }

      if ((c->size[0] == 0) || (p->size[1] == 0)) {
      } else {
        d = c->size[0] * (p->size[1] - 1);
        b_p = 0;
        while ((m > 0) && (b_p <= d)) {
          i12 = b_p + m;
          for (ic = b_p; ic + 1 <= i12; ic++) {
            d_c->data[ic] = 0.0F;
          }

          b_p += m;
        }

        br = 0;
        b_p = 0;
        while ((m > 0) && (b_p <= d)) {
          ar = 0;
          i12 = br + k;
          for (ib = br; ib + 1 <= i12; ib++) {
            if (p->data[ib] != 0.0F) {
              ia = ar;
              i13 = b_p + m;
              for (ic = b_p; ic + 1 <= i13; ic++) {
                ia++;
                d_c->data[ic] += p->data[ib] * c->data[ia - 1];
              }
            }

            ar += m;
          }

          br += k;
          b_p += m;
        }
      }
    }

    i12 = p->size[0] * p->size[1];
    emxEnsureCapacity((emxArray__common *)p, i12, sizeof(float));
    d = p->size[0];
    b_p = p->size[1];
    ar = d * b_p;
    for (i12 = 0; i12 < ar; i12++) {
      p->data[i12] = 1.02040815F * p->data[i12] - d_c->data[i12];
    }

    // 'RLS:40' yn(n)=W*x;
    if ((W->size[1] == 1) || (div_s32_floor(i11 - i9, i10) + 1 == 1)) {
      e_c = 0.0F;
      for (i9 = 0; i9 < W->size[1]; i9++) {
        e_c += W->data[W->size[0] * i9] * x->data[i9];
      }
    } else {
      e_c = 0.0F;
      for (i9 = 0; i9 < W->size[1]; i9++) {
        e_c += W->data[W->size[0] * i9] * x->data[i9];
      }
    }

    b_yn->data[(int)b_n - 1] = e_c;
    n++;
  }

  emxFree_real32_T(&d_c);
  emxFree_real32_T(&c_c);
  emxFree_real32_T(&b_c);
  emxFree_real32_T(&c);
  emxFree_real32_T(&b_k);
  emxFree_real32_T(&x);
  emxFree_real32_T(&p);
}

//
// Arguments    : const emxArray_real32_T *farEndSound
//                const emxArray_real32_T *nearEndSound
// Return Type  : float
//
float delayEstimation(const emxArray_real32_T *farEndSound, const
                      emxArray_real32_T *nearEndSound)
{
  int ixstart;
  int n;
  emxArray_real32_T *Cxy2;
  emxArray_real32_T *c1;
  int b_nearEndSound[1];
  emxArray_real32_T c_nearEndSound;
  int itmp;
  float mtmp;
  int ix;
  boolean_T exitg1;

  // 'delayEstimation:3' N = length(farEndSound);
  //  %%
  //  X1 = fft(farEndSound, 2*N-1);
  //  X2 = fft(nearEndSound, 2*N-1);
  //  Sxy=X1.*conj(X2);
  //  Cxy=fftshift(ifft(Sxy));
  //  % Cxy=Cxy(1:N);
  //  [maxa,location]=max(Cxy);
  //  delay = N - location;
  //
  // 'delayEstimation:15' Cxy2=xcorr(farEndSound,nearEndSound);
  ixstart = farEndSound->size[0];
  n = nearEndSound->size[0];
  if (ixstart > n) {
    n = ixstart;
  }

  emxInit_real32_T(&Cxy2, 1);
  emxInit_real32_T(&c1, 1);
  b_nearEndSound[0] = nearEndSound->size[0];
  c_nearEndSound = *nearEndSound;
  c_nearEndSound.size = (int *)&b_nearEndSound;
  c_nearEndSound.numDimensions = 1;
  crosscorr(farEndSound, &c_nearEndSound, (double)n - 1.0, c1);
  itmp = Cxy2->size[0];
  Cxy2->size[0] = (int)(2.0 * ((double)n - 1.0) + 1.0);
  emxEnsureCapacity((emxArray__common *)Cxy2, itmp, sizeof(float));
  ixstart = (int)(2.0 * ((double)n - 1.0) + 1.0);
  for (itmp = 0; itmp < ixstart; itmp++) {
    Cxy2->data[itmp] = 0.0F;
  }

  ixstart = c1->size[0] - 1;
  for (itmp = 0; itmp <= ixstart; itmp++) {
    Cxy2->data[itmp] = c1->data[itmp];
  }

  emxFree_real32_T(&c1);

  // 'delayEstimation:16' [max1,location1]=max(Cxy2);
  ixstart = 1;
  n = Cxy2->size[0];
  mtmp = Cxy2->data[0];
  itmp = 1;
  if (Cxy2->size[0] > 1) {
    if (rtIsNaNF(Cxy2->data[0])) {
      ix = 2;
      exitg1 = false;
      while ((!exitg1) && (ix <= n)) {
        ixstart = ix;
        if (!rtIsNaNF(Cxy2->data[ix - 1])) {
          mtmp = Cxy2->data[ix - 1];
          itmp = ix;
          exitg1 = true;
        } else {
          ix++;
        }
      }
    }

    if (ixstart < Cxy2->size[0]) {
      while (ixstart + 1 <= n) {
        if (Cxy2->data[ixstart] > mtmp) {
          mtmp = Cxy2->data[ixstart];
          itmp = ixstart + 1;
        }

        ixstart++;
      }
    }
  }

  emxFree_real32_T(&Cxy2);

  // 'delayEstimation:17' delay = N - location1;
  return (float)farEndSound->size[0] - (float)itmp;
}

//
// Arguments    : void
// Return Type  : void
//
void delayEstimation_initialize()
{
  rt_InitInfAndNaN(8U);
}

//
// Arguments    : void
// Return Type  : void
//
void delayEstimation_terminate()
{
  // (no terminate code required)
}

//
// File trailer for delayEstimation.cpp
//
// [EOF]
//
