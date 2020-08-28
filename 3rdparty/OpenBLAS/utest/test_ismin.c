/*****************************************************************************
Copyright (c) 2020, The OpenBLAS Project
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
   3. Neither the name of the OpenBLAS project nor the names of
      its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**********************************************************************************/

#include "openblas_utest.h"

#define ELEMENTS 50
#define INCREMENT 2

CTEST(ismin, positive_step_2){
	blasint i;
  blasint N = ELEMENTS, inc = INCREMENT;
  float x[ELEMENTS * INCREMENT];
  for (i = 0; i < N * inc; i ++) {
    x[i] = i + 1000;
  }

  x[8 * inc] = 0;
  blasint index = BLASFUNC(ismin)(&N, x, &inc);
  ASSERT_EQUAL(9, index);
}

CTEST(ismin, negative_step_2){
	blasint i;
  blasint N = ELEMENTS, inc = INCREMENT;
  float x[ELEMENTS * INCREMENT];
  for (i = 0; i < N * inc; i ++) {
    x[i] = - i - 1000;
  }

  x[8 * inc] = -123456.0f;
  blasint index = BLASFUNC(ismin)(&N, x, &inc);
  ASSERT_EQUAL(9, index);
}

CTEST(ismax, positive_step_2){
	blasint i;
  blasint N = ELEMENTS, inc = INCREMENT;
  float x[ELEMENTS * INCREMENT];
  for (i = 0; i < N * inc; i ++) {
    x[i] = i + 1000;
  }

  x[8 * inc] = 123456.0f;
  blasint index = BLASFUNC(ismax)(&N, x, &inc);
  ASSERT_EQUAL(9, index);
}

CTEST(ismax, negative_step_2){
	blasint i;
  blasint N = ELEMENTS, inc = INCREMENT;
  float x[ELEMENTS * INCREMENT];
  for (i = 0; i < N * inc; i ++) {
    x[i] = - i - 1000;
  }

  x[8 * inc] = 0;
  blasint index = BLASFUNC(ismax)(&N, x, &inc);
  ASSERT_EQUAL(9, index);
}
