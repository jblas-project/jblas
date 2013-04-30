/// --- BEGIN LICENSE BLOCK ---
// Copyright (c) 2009, Mikio L. Braun
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
// 
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
// 
//     * Neither the name of the Technische Universität Berlin nor the
//       names of its contributors may be used to endorse or promote
//       products derived from this software without specific prior
//       written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/// --- END LICENSE BLOCK ---

#include "org_jblas_NativeBlas.h"

#define CORE_PACKAGE "org/jblas/"

/*
 * For convenience, we define some typedefs here which have the
 * same name as certain Java types, but their implementation differs (of course)
 */
typedef char *String;

typedef char ByteBuffer;
typedef short ShortBuffer;
typedef int IntBuffer;
typedef long LongBuffer;
typedef float FloatBuffer;
typedef double DoubleBuffer;

/* a function to create new objects */
static jobject createObject(JNIEnv *env, const char *className, const char *signature, ...)
{
  va_list args;
  jclass klass = (*env)->FindClass(env, className);
  jmethodID init = (*env)->GetMethodID(env, klass, "<init>", signature);
  jobject newObject;

  va_start(args, signature);
  newObject = (*env)->NewObjectV(env, klass, init, args);
  va_end(args);

  return newObject;
}


#include <complex.h>
typedef float complex ComplexFloat;
typedef double complex ComplexDouble;

static jobject createComplexFloat(JNIEnv *env, ComplexFloat fc) {
  return createObject(env, CORE_PACKAGE "ComplexFloat", "(FF)V", crealf(fc), cimagf(fc));
}

static jobject createComplexDouble(JNIEnv *env, ComplexDouble dc)
{
  return createObject(env, CORE_PACKAGE "ComplexDouble", "(DD)V", creal(dc), cimag(dc));
}

static ComplexFloat getComplexFloat(JNIEnv *env, jobject fc)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexFloat");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "F");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "F");

  return (*env)->GetFloatField(env, fc, reField) + I*(*env)->GetFloatField(env, fc, imField);
}

static ComplexDouble getComplexDouble(JNIEnv *env, jobject dc)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexDouble");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "D");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "D");

  return (*env)->GetDoubleField(env, dc, reField) + I*(*env)->GetDoubleField(env, dc, imField);
}


static void throwIllegalArgumentException(JNIEnv *env, const char *message)
{
  jclass klass = (*env)->FindClass(env, "java/lang/IllegalArgumentException");

  (*env)->ThrowNew(env, klass, message);
}

/**********************************************************************/
/*                 XERBLA function arguments                          */
/**********************************************************************/

static char *routine_names[] = {
 "CAXPY",  "CCOPY",  "CDOTC",  "CDOTU",  "CGEEV",  "CGEMM",  "CGEMV",  "CGERC",  "CGERU",  "CGESVD",  "CSCAL",  "CSSCAL",  "CSWAP",  "DASUM",  "DAXPY",  "DCOPY",  "DDOT",  "DGEEV",  "DGELSD",  "DGEMM",  "DGEMV",  "DGEQRF",  "DGER",  "DGESV",  "DGESVD",  "DGETRF",  "DNRM2",  "DORMQR",  "DPOSV",  "DPOTRF",  "DSCAL",  "DSWAP",  "DSYEV",  "DSYEVD",  "DSYEVR",  "DSYEVX",  "DSYGVD",  "DSYGVX",  "DSYSV",  "DZASUM",  "DZNRM2",  "ICAMAX",  "IDAMAX",  "ILAENV",  "ISAMAX",  "IZAMAX",  "SASUM",  "SAXPY",  "SCASUM",  "SCNRM2",  "SCOPY",  "SDOT",  "SGEEV",  "SGELSD",  "SGEMM",  "SGEMV",  "SGEQRF",  "SGER",  "SGESV",  "SGESVD",  "SGETRF",  "SNRM2",  "SORMQR",  "SPOSV",  "SPOTRF",  "SSCAL",  "SSWAP",  "SSYEV",  "SSYEVD",  "SSYEVR",  "SSYEVX",  "SSYGVD",  "SSYGVX",  "SSYSV",  "ZAXPY",  "ZCOPY",  "ZDOTC",  "ZDOTU",  "ZDSCAL",  "ZGEEV",  "ZGEMM",  "ZGEMV",  "ZGERC",  "ZGERU",  "ZGESVD",  "ZSCAL",  "ZSWAP", 	0
};

static char *routine_arguments[][23] = {
   { "N", "CA", "CX", "INCX", "CY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "JOBVL", "JOBVR", "N", "A", "LDA", "W", "VL", "LDVL", "VR", "LDVR", "WORK", "LWORK", "RWORK", "INFO" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "JOBU", "JOBVT", "M", "N", "A", "LDA", "S", "U", "LDU", "VT", "LDVT", "WORK", "LWORK", "RWORK", "INFO" }, 
   { "N", "CA", "CX", "INCX" }, 
   { "N", "SA", "CX", "INCX" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "N", "DX", "INCX" }, 
   { "N", "DA", "DX", "INCX", "DY", "INCY" }, 
   { "N", "DX", "INCX", "DY", "INCY" }, 
   { "N", "DX", "INCX", "DY", "INCY" }, 
   { "JOBVL", "JOBVR", "N", "A", "LDA", "WR", "WI", "VL", "LDVL", "VR", "LDVR", "WORK", "LWORK", "INFO" }, 
   { "M", "N", "NRHS", "A", "LDA", "B", "LDB", "S", "RCOND", "RANK", "WORK", "LWORK", "IWORK", "INFO" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "A", "LDA", "TAU", "WORK", "LWORK", "INFO" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "INFO" }, 
   { "JOBU", "JOBVT", "M", "N", "A", "LDA", "S", "U", "LDU", "VT", "LDVT", "WORK", "LWORK", "INFO" }, 
   { "M", "N", "A", "LDA", "IPIV", "INFO" }, 
   { "N", "X", "INCX" }, 
   { "SIDE", "TRANS", "M", "N", "K", "A", "LDA", "TAU", "C", "LDC", "WORK", "LWORK", "INFO" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "B", "LDB", "INFO" }, 
   { "UPLO", "N", "A", "LDA", "INFO" }, 
   { "N", "DA", "DX", "INCX" }, 
   { "N", "DX", "INCX", "DY", "INCY" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "INFO" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "ISUPPZ", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "WORK", "LWORK", "IWORK", "IFAIL", "INFO" }, 
   { "ITYPE", "JOBZ", "UPLO", "N", "A", "LDA", "B", "LDB", "W", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "ITYPE", "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "B", "LDB", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "WORK", "LWORK", "IWORK", "IFAIL", "INFO" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "WORK", "LWORK", "INFO" }, 
   { "N", "ZX", "INCX" }, 
   { "N", "X", "INCX" }, 
   { "N", "CX", "INCX" }, 
   { "N", "DX", "INCX" }, 
   { "ISPEC", "NAME", "OPTS", "N1", "N2", "N3", "N4" }, 
   { "N", "SX", "INCX" }, 
   { "N", "ZX", "INCX" }, 
   { "N", "SX", "INCX" }, 
   { "N", "SA", "SX", "INCX", "SY", "INCY" }, 
   { "N", "CX", "INCX" }, 
   { "N", "X", "INCX" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "JOBVL", "JOBVR", "N", "A", "LDA", "WR", "WI", "VL", "LDVL", "VR", "LDVR", "WORK", "LWORK", "INFO" }, 
   { "M", "N", "NRHS", "A", "LDA", "B", "LDB", "S", "RCOND", "RANK", "WORK", "LWORK", "IWORK", "INFO" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "A", "LDA", "TAU", "WORK", "LWORK", "INFO" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "INFO" }, 
   { "JOBU", "JOBVT", "M", "N", "A", "LDA", "S", "U", "LDU", "VT", "LDVT", "WORK", "LWORK", "INFO" }, 
   { "M", "N", "A", "LDA", "IPIV", "INFO" }, 
   { "N", "X", "INCX" }, 
   { "SIDE", "TRANS", "M", "N", "K", "A", "LDA", "TAU", "C", "LDC", "WORK", "LWORK", "INFO" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "B", "LDB", "INFO" }, 
   { "UPLO", "N", "A", "LDA", "INFO" }, 
   { "N", "SA", "SX", "INCX" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "INFO" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "ISUPPZ", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "WORK", "LWORK", "IWORK", "IFAIL", "INFO" }, 
   { "ITYPE", "JOBZ", "UPLO", "N", "A", "LDA", "B", "LDB", "W", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "ITYPE", "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "B", "LDB", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "WORK", "LWORK", "IWORK", "IFAIL", "INFO" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "WORK", "LWORK", "INFO" }, 
   { "N", "ZA", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "DA", "ZX", "INCX" }, 
   { "JOBVL", "JOBVR", "N", "A", "LDA", "W", "VL", "LDVL", "VR", "LDVR", "WORK", "LWORK", "RWORK", "INFO" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "JOBU", "JOBVT", "M", "N", "A", "LDA", "S", "U", "LDU", "VT", "LDVT", "WORK", "LWORK", "RWORK", "INFO" }, 
   { "N", "ZA", "ZX", "INCX" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
};

/**********************************************************************/
/*                 Our implementation of XERBLA                       */
/**********************************************************************/
static JNIEnv *savedEnv = 0;


void xerbla_(char *fct, int *info)
{
	static char name[7];
	static char buffer[256];
	int i;
	char **p;
	char **arguments = 0;
	
	for (i = 0; i < 6; i++) {
		if (fct[i] == ' ')
			break;	
		name[i] = fct[i];
	}
	name[i] = '\0';
	
	//fprintf(stderr, "Searching for function \"%s\"\n", name);
	
	for (p = routine_names, i = 0; *p; p++, i++)
		if (!strcmp(*p, name))
			arguments = routine_arguments[i];
			
	if (!arguments) {
		sprintf(buffer, "XERBLA: Error on argument %d for *unknown function* %s (how odd!)\n", *info, name);
	}
	else {
		sprintf(buffer, "XERBLA: Error on argument %d (%s) in %s", *info, arguments[*info-1], name);
	}
	throwIllegalArgumentException(savedEnv, buffer);
}

/**********************************************************************/
/*                 generated functions below                          */
/**********************************************************************/


JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_ccopy(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern void ccopy_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }
  jfloat *cyPtrBase = 0, *cyPtr = 0;
  if (cy) {
    if((*env)->IsSameObject(env, cy, cx) == JNI_TRUE)
      cyPtrBase = cxPtrBase;
    else
      cyPtrBase = (*env)->GetFloatArrayElements(env, cy, NULL);
    cyPtr = cyPtrBase + 2*cyIdx;
  }

  savedEnv = env;
  ccopy_(&n, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
    if (cyPtrBase == cxPtrBase)
      cxPtrBase = 0;
    cyPtrBase = 0;
  }
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_dcopy(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
{
  extern void dcopy_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }
  jdouble *dyPtrBase = 0, *dyPtr = 0;
  if (dy) {
    if((*env)->IsSameObject(env, dy, dx) == JNI_TRUE)
      dyPtrBase = dxPtrBase;
    else
      dyPtrBase = (*env)->GetDoubleArrayElements(env, dy, NULL);
    dyPtr = dyPtrBase + dyIdx;
  }

  savedEnv = env;
  dcopy_(&n, dxPtr, &incx, dyPtr, &incy);
  if(dyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
    if (dyPtrBase == dxPtrBase)
      dxPtrBase = 0;
    dyPtrBase = 0;
  }
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_scopy(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern void scopy_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }
  jfloat *syPtrBase = 0, *syPtr = 0;
  if (sy) {
    if((*env)->IsSameObject(env, sy, sx) == JNI_TRUE)
      syPtrBase = sxPtrBase;
    else
      syPtrBase = (*env)->GetFloatArrayElements(env, sy, NULL);
    syPtr = syPtrBase + syIdx;
  }

  savedEnv = env;
  scopy_(&n, sxPtr, &incx, syPtr, &incy);
  if(syPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
    if (syPtrBase == sxPtrBase)
      sxPtrBase = 0;
    syPtrBase = 0;
  }
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zcopy(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern void zcopy_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }
  jdouble *zyPtrBase = 0, *zyPtr = 0;
  if (zy) {
    if((*env)->IsSameObject(env, zy, zx) == JNI_TRUE)
      zyPtrBase = zxPtrBase;
    else
      zyPtrBase = (*env)->GetDoubleArrayElements(env, zy, NULL);
    zyPtr = zyPtrBase + 2*zyIdx;
  }

  savedEnv = env;
  zcopy_(&n, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
    if (zyPtrBase == zxPtrBase)
      zxPtrBase = 0;
    zyPtrBase = 0;
  }
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_cswap(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern void cswap_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }
  jfloat *cyPtrBase = 0, *cyPtr = 0;
  if (cy) {
    if((*env)->IsSameObject(env, cy, cx) == JNI_TRUE)
      cyPtrBase = cxPtrBase;
    else
      cyPtrBase = (*env)->GetFloatArrayElements(env, cy, NULL);
    cyPtr = cyPtrBase + 2*cyIdx;
  }

  savedEnv = env;
  cswap_(&n, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
    if (cyPtrBase == cxPtrBase)
      cxPtrBase = 0;
    cyPtrBase = 0;
  }
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_dswap(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
{
  extern void dswap_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }
  jdouble *dyPtrBase = 0, *dyPtr = 0;
  if (dy) {
    if((*env)->IsSameObject(env, dy, dx) == JNI_TRUE)
      dyPtrBase = dxPtrBase;
    else
      dyPtrBase = (*env)->GetDoubleArrayElements(env, dy, NULL);
    dyPtr = dyPtrBase + dyIdx;
  }

  savedEnv = env;
  dswap_(&n, dxPtr, &incx, dyPtr, &incy);
  if(dyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
    if (dyPtrBase == dxPtrBase)
      dxPtrBase = 0;
    dyPtrBase = 0;
  }
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_sswap(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern void sswap_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }
  jfloat *syPtrBase = 0, *syPtr = 0;
  if (sy) {
    if((*env)->IsSameObject(env, sy, sx) == JNI_TRUE)
      syPtrBase = sxPtrBase;
    else
      syPtrBase = (*env)->GetFloatArrayElements(env, sy, NULL);
    syPtr = syPtrBase + syIdx;
  }

  savedEnv = env;
  sswap_(&n, sxPtr, &incx, syPtr, &incy);
  if(syPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
    if (syPtrBase == sxPtrBase)
      sxPtrBase = 0;
    syPtrBase = 0;
  }
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zswap(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern void zswap_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }
  jdouble *zyPtrBase = 0, *zyPtr = 0;
  if (zy) {
    if((*env)->IsSameObject(env, zy, zx) == JNI_TRUE)
      zyPtrBase = zxPtrBase;
    else
      zyPtrBase = (*env)->GetDoubleArrayElements(env, zy, NULL);
    zyPtr = zyPtrBase + 2*zyIdx;
  }

  savedEnv = env;
  zswap_(&n, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
    if (zyPtrBase == zxPtrBase)
      zxPtrBase = 0;
    zyPtrBase = 0;
  }
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_caxpy(JNIEnv *env, jclass this, jint n, jobject ca, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern void caxpy_(jint *, float complex *, jfloat *, jint *, jfloat *, jint *);
  
  float complex caCplx;
  caCplx = getComplexFloat(env, ca);
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }
  jfloat *cyPtrBase = 0, *cyPtr = 0;
  if (cy) {
    if((*env)->IsSameObject(env, cy, cx) == JNI_TRUE)
      cyPtrBase = cxPtrBase;
    else
      cyPtrBase = (*env)->GetFloatArrayElements(env, cy, NULL);
    cyPtr = cyPtrBase + 2*cyIdx;
  }

  savedEnv = env;
  caxpy_(&n, &caCplx, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
    if (cyPtrBase == cxPtrBase)
      cxPtrBase = 0;
    cyPtrBase = 0;
  }
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_daxpy(JNIEnv *env, jclass this, jint n, jdouble da, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
{
  extern void daxpy_(jint *, jdouble *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }
  jdouble *dyPtrBase = 0, *dyPtr = 0;
  if (dy) {
    if((*env)->IsSameObject(env, dy, dx) == JNI_TRUE)
      dyPtrBase = dxPtrBase;
    else
      dyPtrBase = (*env)->GetDoubleArrayElements(env, dy, NULL);
    dyPtr = dyPtrBase + dyIdx;
  }

  savedEnv = env;
  daxpy_(&n, &da, dxPtr, &incx, dyPtr, &incy);
  if(dyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
    if (dyPtrBase == dxPtrBase)
      dxPtrBase = 0;
    dyPtrBase = 0;
  }
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_saxpy(JNIEnv *env, jclass this, jint n, jfloat sa, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern void saxpy_(jint *, jfloat *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }
  jfloat *syPtrBase = 0, *syPtr = 0;
  if (sy) {
    if((*env)->IsSameObject(env, sy, sx) == JNI_TRUE)
      syPtrBase = sxPtrBase;
    else
      syPtrBase = (*env)->GetFloatArrayElements(env, sy, NULL);
    syPtr = syPtrBase + syIdx;
  }

  savedEnv = env;
  saxpy_(&n, &sa, sxPtr, &incx, syPtr, &incy);
  if(syPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
    if (syPtrBase == sxPtrBase)
      sxPtrBase = 0;
    syPtrBase = 0;
  }
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zaxpy(JNIEnv *env, jclass this, jint n, jobject za, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern void zaxpy_(jint *, double complex *, jdouble *, jint *, jdouble *, jint *);
  
  double complex zaCplx;
  zaCplx = getComplexDouble(env, za);
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }
  jdouble *zyPtrBase = 0, *zyPtr = 0;
  if (zy) {
    if((*env)->IsSameObject(env, zy, zx) == JNI_TRUE)
      zyPtrBase = zxPtrBase;
    else
      zyPtrBase = (*env)->GetDoubleArrayElements(env, zy, NULL);
    zyPtr = zyPtrBase + 2*zyIdx;
  }

  savedEnv = env;
  zaxpy_(&n, &zaCplx, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
    if (zyPtrBase == zxPtrBase)
      zxPtrBase = 0;
    zyPtrBase = 0;
  }
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_cscal(JNIEnv *env, jclass this, jint n, jobject ca, jfloatArray cx, jint cxIdx, jint incx)
{
  extern void cscal_(jint *, float complex *, jfloat *, jint *);
  
  float complex caCplx;
  caCplx = getComplexFloat(env, ca);
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }

  savedEnv = env;
  cscal_(&n, &caCplx, cxPtr, &incx);
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_dscal(JNIEnv *env, jclass this, jint n, jdouble da, jdoubleArray dx, jint dxIdx, jint incx)
{
  extern void dscal_(jint *, jdouble *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }

  savedEnv = env;
  dscal_(&n, &da, dxPtr, &incx);
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_sscal(JNIEnv *env, jclass this, jint n, jfloat sa, jfloatArray sx, jint sxIdx, jint incx)
{
  extern void sscal_(jint *, jfloat *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }

  savedEnv = env;
  sscal_(&n, &sa, sxPtr, &incx);
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zscal(JNIEnv *env, jclass this, jint n, jobject za, jdoubleArray zx, jint zxIdx, jint incx)
{
  extern void zscal_(jint *, double complex *, jdouble *, jint *);
  
  double complex zaCplx;
  zaCplx = getComplexDouble(env, za);
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }

  savedEnv = env;
  zscal_(&n, &zaCplx, zxPtr, &incx);
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_csscal(JNIEnv *env, jclass this, jint n, jfloat sa, jfloatArray cx, jint cxIdx, jint incx)
{
  extern void csscal_(jint *, jfloat *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }

  savedEnv = env;
  csscal_(&n, &sa, cxPtr, &incx);
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zdscal(JNIEnv *env, jclass this, jint n, jdouble da, jdoubleArray zx, jint zxIdx, jint incx)
{
  extern void zdscal_(jint *, jdouble *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }

  savedEnv = env;
  zdscal_(&n, &da, zxPtr, &incx);
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

}

JNIEXPORT jobject JNICALL Java_org_jblas_NativeBlas_cdotc(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern float complex cdotc_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }
  jfloat *cyPtrBase = 0, *cyPtr = 0;
  if (cy) {
    if((*env)->IsSameObject(env, cy, cx) == JNI_TRUE)
      cyPtrBase = cxPtrBase;
    else
      cyPtrBase = (*env)->GetFloatArrayElements(env, cy, NULL);
    cyPtr = cyPtrBase + 2*cyIdx;
  }

  savedEnv = env;
  float complex retval = cdotc_(&n, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
    if (cyPtrBase == cxPtrBase)
      cxPtrBase = 0;
    cyPtrBase = 0;
  }
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

  return createComplexFloat(env, retval);
}

JNIEXPORT jobject JNICALL Java_org_jblas_NativeBlas_cdotu(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern float complex cdotu_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }
  jfloat *cyPtrBase = 0, *cyPtr = 0;
  if (cy) {
    if((*env)->IsSameObject(env, cy, cx) == JNI_TRUE)
      cyPtrBase = cxPtrBase;
    else
      cyPtrBase = (*env)->GetFloatArrayElements(env, cy, NULL);
    cyPtr = cyPtrBase + 2*cyIdx;
  }

  savedEnv = env;
  float complex retval = cdotu_(&n, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
    if (cyPtrBase == cxPtrBase)
      cxPtrBase = 0;
    cyPtrBase = 0;
  }
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

  return createComplexFloat(env, retval);
}

JNIEXPORT jdouble JNICALL Java_org_jblas_NativeBlas_ddot(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
{
  extern jdouble ddot_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }
  jdouble *dyPtrBase = 0, *dyPtr = 0;
  if (dy) {
    if((*env)->IsSameObject(env, dy, dx) == JNI_TRUE)
      dyPtrBase = dxPtrBase;
    else
      dyPtrBase = (*env)->GetDoubleArrayElements(env, dy, NULL);
    dyPtr = dyPtrBase + dyIdx;
  }

  savedEnv = env;
  jdouble retval = ddot_(&n, dxPtr, &incx, dyPtr, &incy);
  if(dyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
    if (dyPtrBase == dxPtrBase)
      dxPtrBase = 0;
    dyPtrBase = 0;
  }
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_NativeBlas_sdot(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern jfloat sdot_(jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }
  jfloat *syPtrBase = 0, *syPtr = 0;
  if (sy) {
    if((*env)->IsSameObject(env, sy, sx) == JNI_TRUE)
      syPtrBase = sxPtrBase;
    else
      syPtrBase = (*env)->GetFloatArrayElements(env, sy, NULL);
    syPtr = syPtrBase + syIdx;
  }

  savedEnv = env;
  jfloat retval = sdot_(&n, sxPtr, &incx, syPtr, &incy);
  if(syPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
    if (syPtrBase == sxPtrBase)
      sxPtrBase = 0;
    syPtrBase = 0;
  }
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

  return (jdouble) retval;
}

JNIEXPORT jobject JNICALL Java_org_jblas_NativeBlas_zdotc(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern double complex zdotc_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }
  jdouble *zyPtrBase = 0, *zyPtr = 0;
  if (zy) {
    if((*env)->IsSameObject(env, zy, zx) == JNI_TRUE)
      zyPtrBase = zxPtrBase;
    else
      zyPtrBase = (*env)->GetDoubleArrayElements(env, zy, NULL);
    zyPtr = zyPtrBase + 2*zyIdx;
  }

  savedEnv = env;
  double complex retval = zdotc_(&n, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
    if (zyPtrBase == zxPtrBase)
      zxPtrBase = 0;
    zyPtrBase = 0;
  }
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

  return createComplexDouble(env, retval);
}

JNIEXPORT jobject JNICALL Java_org_jblas_NativeBlas_zdotu(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern double complex zdotu_(jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }
  jdouble *zyPtrBase = 0, *zyPtr = 0;
  if (zy) {
    if((*env)->IsSameObject(env, zy, zx) == JNI_TRUE)
      zyPtrBase = zxPtrBase;
    else
      zyPtrBase = (*env)->GetDoubleArrayElements(env, zy, NULL);
    zyPtr = zyPtrBase + 2*zyIdx;
  }

  savedEnv = env;
  double complex retval = zdotu_(&n, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
    if (zyPtrBase == zxPtrBase)
      zxPtrBase = 0;
    zyPtrBase = 0;
  }
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

  return createComplexDouble(env, retval);
}

JNIEXPORT jdouble JNICALL Java_org_jblas_NativeBlas_dnrm2(JNIEnv *env, jclass this, jint n, jdoubleArray x, jint xIdx, jint incx)
{
  extern jdouble dnrm2_(jint *, jdouble *, jint *);
  
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  jdouble retval = dnrm2_(&n, xPtr, &incx);
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
    xPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jdouble JNICALL Java_org_jblas_NativeBlas_dznrm2(JNIEnv *env, jclass this, jint n, jdoubleArray x, jint xIdx, jint incx)
{
  extern jdouble dznrm2_(jint *, jdouble *, jint *);
  
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  jdouble retval = dznrm2_(&n, xPtr, &incx);
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
    xPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_NativeBlas_scnrm2(JNIEnv *env, jclass this, jint n, jfloatArray x, jint xIdx, jint incx)
{
  extern jfloat scnrm2_(jint *, jfloat *, jint *);
  
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  jfloat retval = scnrm2_(&n, xPtr, &incx);
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
    xPtrBase = 0;
  }

  return (jdouble) retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_NativeBlas_snrm2(JNIEnv *env, jclass this, jint n, jfloatArray x, jint xIdx, jint incx)
{
  extern jfloat snrm2_(jint *, jfloat *, jint *);
  
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  jfloat retval = snrm2_(&n, xPtr, &incx);
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
    xPtrBase = 0;
  }

  return (jdouble) retval;
}

JNIEXPORT jdouble JNICALL Java_org_jblas_NativeBlas_dasum(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx)
{
  extern jdouble dasum_(jint *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }

  savedEnv = env;
  jdouble retval = dasum_(&n, dxPtr, &incx);
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jdouble JNICALL Java_org_jblas_NativeBlas_dzasum(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx)
{
  extern jdouble dzasum_(jint *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }

  savedEnv = env;
  jdouble retval = dzasum_(&n, zxPtr, &incx);
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_NativeBlas_sasum(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx)
{
  extern jfloat sasum_(jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }

  savedEnv = env;
  jfloat retval = sasum_(&n, sxPtr, &incx);
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

  return (jdouble) retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_NativeBlas_scasum(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx)
{
  extern jfloat scasum_(jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }

  savedEnv = env;
  jfloat retval = scasum_(&n, cxPtr, &incx);
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

  return (jdouble) retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_icamax(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx)
{
  extern jint icamax_(jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }

  savedEnv = env;
  jint retval = icamax_(&n, cxPtr, &incx);
  if(cxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
    cxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_idamax(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx)
{
  extern jint idamax_(jint *, jdouble *, jint *);
  
  jdouble *dxPtrBase = 0, *dxPtr = 0;
  if (dx) {
    dxPtrBase = (*env)->GetDoubleArrayElements(env, dx, NULL);
    dxPtr = dxPtrBase + dxIdx;
  }

  savedEnv = env;
  jint retval = idamax_(&n, dxPtr, &incx);
  if(dxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
    dxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_isamax(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx)
{
  extern jint isamax_(jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }

  savedEnv = env;
  jint retval = isamax_(&n, sxPtr, &incx);
  if(sxPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
    sxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_izamax(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx)
{
  extern jint izamax_(jint *, jdouble *, jint *);
  
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }

  savedEnv = env;
  jint retval = izamax_(&n, zxPtr, &incx);
  if(zxPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
    zxPtrBase = 0;
  }

  return retval;
}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_cgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jobject beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void cgemv_(char *, jint *, jint *, float complex *, jfloat *, jint *, jfloat *, jint *, float complex *, jfloat *, jint *);
  
  char transChr = (char) trans;
  float complex alphaCplx;
  alphaCplx = getComplexFloat(env, alpha);
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, a) == JNI_TRUE)
      xPtrBase = aPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  float complex betaCplx;
  betaCplx = getComplexFloat(env, beta);
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, a) == JNI_TRUE)
      yPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }

  savedEnv = env;
  cgemv_(&transChr, &m, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
    if (yPtrBase == aPtrBase)
      aPtrBase = 0;
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, JNI_ABORT);
    if (xPtrBase == aPtrBase)
      aPtrBase = 0;
    xPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_dgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jdouble beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void dgemv_(char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char transChr = (char) trans;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, a) == JNI_TRUE)
      xPtrBase = aPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, a) == JNI_TRUE)
      yPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + yIdx;
  }

  savedEnv = env;
  dgemv_(&transChr, &m, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
    if (yPtrBase == aPtrBase)
      aPtrBase = 0;
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, JNI_ABORT);
    if (xPtrBase == aPtrBase)
      aPtrBase = 0;
    xPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_sgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jfloat beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void sgemv_(char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char transChr = (char) trans;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, a) == JNI_TRUE)
      xPtrBase = aPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, a) == JNI_TRUE)
      yPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + yIdx;
  }

  savedEnv = env;
  sgemv_(&transChr, &m, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
    if (yPtrBase == aPtrBase)
      aPtrBase = 0;
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, JNI_ABORT);
    if (xPtrBase == aPtrBase)
      aPtrBase = 0;
    xPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jobject beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void zgemv_(char *, jint *, jint *, double complex *, jdouble *, jint *, jdouble *, jint *, double complex *, jdouble *, jint *);
  
  char transChr = (char) trans;
  double complex alphaCplx;
  alphaCplx = getComplexDouble(env, alpha);
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, a) == JNI_TRUE)
      xPtrBase = aPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  double complex betaCplx;
  betaCplx = getComplexDouble(env, beta);
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, a) == JNI_TRUE)
      yPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }

  savedEnv = env;
  zgemv_(&transChr, &m, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
    if (yPtrBase == aPtrBase)
      aPtrBase = 0;
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, JNI_ABORT);
    if (xPtrBase == aPtrBase)
      aPtrBase = 0;
    xPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_cgerc(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void cgerc_(jint *, jint *, float complex *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  float complex alphaCplx;
  alphaCplx = getComplexFloat(env, alpha);
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, a, y) == JNI_TRUE)
      aPtrBase = yPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }

  savedEnv = env;
  cgerc_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == xPtrBase)
      xPtrBase = 0;
    if (aPtrBase == yPtrBase)
      yPtrBase = 0;
    aPtrBase = 0;
  }
  if(yPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, JNI_ABORT);
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, JNI_ABORT);
    xPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_cgeru(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void cgeru_(jint *, jint *, float complex *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  float complex alphaCplx;
  alphaCplx = getComplexFloat(env, alpha);
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, a, y) == JNI_TRUE)
      aPtrBase = yPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }

  savedEnv = env;
  cgeru_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == xPtrBase)
      xPtrBase = 0;
    if (aPtrBase == yPtrBase)
      yPtrBase = 0;
    aPtrBase = 0;
  }
  if(yPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, JNI_ABORT);
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, JNI_ABORT);
    xPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_dger(JNIEnv *env, jclass this, jint m, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void dger_(jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + yIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, a, y) == JNI_TRUE)
      aPtrBase = yPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }

  savedEnv = env;
  dger_(&m, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == xPtrBase)
      xPtrBase = 0;
    if (aPtrBase == yPtrBase)
      yPtrBase = 0;
    aPtrBase = 0;
  }
  if(yPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, JNI_ABORT);
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, JNI_ABORT);
    xPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_sger(JNIEnv *env, jclass this, jint m, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void sger_(jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + yIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, a, y) == JNI_TRUE)
      aPtrBase = yPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }

  savedEnv = env;
  sger_(&m, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == xPtrBase)
      xPtrBase = 0;
    if (aPtrBase == yPtrBase)
      yPtrBase = 0;
    aPtrBase = 0;
  }
  if(yPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, JNI_ABORT);
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, JNI_ABORT);
    xPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zgerc(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void zgerc_(jint *, jint *, double complex *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  double complex alphaCplx;
  alphaCplx = getComplexDouble(env, alpha);
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, a, y) == JNI_TRUE)
      aPtrBase = yPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }

  savedEnv = env;
  zgerc_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == xPtrBase)
      xPtrBase = 0;
    if (aPtrBase == yPtrBase)
      yPtrBase = 0;
    aPtrBase = 0;
  }
  if(yPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, JNI_ABORT);
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, JNI_ABORT);
    xPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zgeru(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void zgeru_(jint *, jint *, double complex *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  double complex alphaCplx;
  alphaCplx = getComplexDouble(env, alpha);
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, a, y) == JNI_TRUE)
      aPtrBase = yPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }

  savedEnv = env;
  zgeru_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == xPtrBase)
      xPtrBase = 0;
    if (aPtrBase == yPtrBase)
      yPtrBase = 0;
    aPtrBase = 0;
  }
  if(yPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, JNI_ABORT);
    if (yPtrBase == xPtrBase)
      xPtrBase = 0;
    yPtrBase = 0;
  }
  if(xPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, JNI_ABORT);
    xPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_cgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jobject beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void cgemm_(char *, char *, jint *, jint *, jint *, float complex *, jfloat *, jint *, jfloat *, jint *, float complex *, jfloat *, jint *);
  
  char transaChr = (char) transa;
  char transbChr = (char) transb;
  float complex alphaCplx;
  alphaCplx = getComplexFloat(env, alpha);
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + 2*bIdx;
  }
  float complex betaCplx;
  betaCplx = getComplexFloat(env, beta);
  jfloat *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, c, b) == JNI_TRUE)
      cPtrBase = bPtrBase;
    else
      cPtrBase = (*env)->GetFloatArrayElements(env, c, NULL);
    cPtr = cPtrBase + 2*cIdx;
  }

  savedEnv = env;
  cgemm_(&transaChr, &transbChr, &m, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
    if (cPtrBase == aPtrBase)
      aPtrBase = 0;
    if (cPtrBase == bPtrBase)
      bPtrBase = 0;
    cPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, JNI_ABORT);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_dgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void dgemm_(char *, char *, jint *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char transaChr = (char) transa;
  char transbChr = (char) transb;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jdouble *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, c, b) == JNI_TRUE)
      cPtrBase = bPtrBase;
    else
      cPtrBase = (*env)->GetDoubleArrayElements(env, c, NULL);
    cPtr = cPtrBase + cIdx;
  }

  savedEnv = env;
  dgemm_(&transaChr, &transbChr, &m, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
    if (cPtrBase == aPtrBase)
      aPtrBase = 0;
    if (cPtrBase == bPtrBase)
      bPtrBase = 0;
    cPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, JNI_ABORT);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_sgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void sgemm_(char *, char *, jint *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char transaChr = (char) transa;
  char transbChr = (char) transb;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jfloat *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, c, b) == JNI_TRUE)
      cPtrBase = bPtrBase;
    else
      cPtrBase = (*env)->GetFloatArrayElements(env, c, NULL);
    cPtr = cPtrBase + cIdx;
  }

  savedEnv = env;
  sgemm_(&transaChr, &transbChr, &m, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
    if (cPtrBase == aPtrBase)
      aPtrBase = 0;
    if (cPtrBase == bPtrBase)
      bPtrBase = 0;
    cPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, JNI_ABORT);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT void JNICALL Java_org_jblas_NativeBlas_zgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jobject beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zgemm_(char *, char *, jint *, jint *, jint *, double complex *, jdouble *, jint *, jdouble *, jint *, double complex *, jdouble *, jint *);
  
  char transaChr = (char) transa;
  char transbChr = (char) transb;
  double complex alphaCplx;
  alphaCplx = getComplexDouble(env, alpha);
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + 2*bIdx;
  }
  double complex betaCplx;
  betaCplx = getComplexDouble(env, beta);
  jdouble *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, c, b) == JNI_TRUE)
      cPtrBase = bPtrBase;
    else
      cPtrBase = (*env)->GetDoubleArrayElements(env, c, NULL);
    cPtr = cPtrBase + 2*cIdx;
  }

  savedEnv = env;
  zgemm_(&transaChr, &transbChr, &m, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
    if (cPtrBase == aPtrBase)
      aPtrBase = 0;
    if (cPtrBase == bPtrBase)
      bPtrBase = 0;
    cPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, JNI_ABORT);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dgesv(JNIEnv *env, jclass this, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jdoubleArray b, jint bIdx, jint ldb)
{
  extern void dgesv_(jint *, jint *, jdouble *, jint *, jint *, jdouble *, jint *, int *);
  
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *ipivPtrBase = 0, *ipivPtr = 0;
  if (ipiv) {
    ipivPtrBase = (*env)->GetIntArrayElements(env, ipiv, NULL);
    ipivPtr = ipivPtrBase + ipivIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  int info;

  savedEnv = env;
  dgesv_(&n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, &info);
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(ipivPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
    ipivPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sgesv(JNIEnv *env, jclass this, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jfloatArray b, jint bIdx, jint ldb)
{
  extern void sgesv_(jint *, jint *, jfloat *, jint *, jint *, jfloat *, jint *, int *);
  
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *ipivPtrBase = 0, *ipivPtr = 0;
  if (ipiv) {
    ipivPtrBase = (*env)->GetIntArrayElements(env, ipiv, NULL);
    ipivPtr = ipivPtrBase + ipivIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  int info;

  savedEnv = env;
  sgesv_(&n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, &info);
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(ipivPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
    ipivPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsysv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jdoubleArray b, jint bIdx, jint ldb, jdoubleArray work, jint workIdx, jint lwork)
{
  extern void dsysv_(char *, jint *, jint *, jdouble *, jint *, jint *, jdouble *, jint *, jdouble *, jint *, int *);
  
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *ipivPtrBase = 0, *ipivPtr = 0;
  if (ipiv) {
    ipivPtrBase = (*env)->GetIntArrayElements(env, ipiv, NULL);
    ipivPtr = ipivPtrBase + ipivIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dsysv_(&uploChr, &n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    workPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(ipivPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
    ipivPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssysv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jfloatArray b, jint bIdx, jint ldb, jfloatArray work, jint workIdx, jint lwork)
{
  extern void ssysv_(char *, jint *, jint *, jfloat *, jint *, jint *, jfloat *, jint *, jfloat *, jint *, int *);
  
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *ipivPtrBase = 0, *ipivPtr = 0;
  if (ipiv) {
    ipivPtrBase = (*env)->GetIntArrayElements(env, ipiv, NULL);
    ipivPtr = ipivPtrBase + ipivIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  ssysv_(&uploChr, &n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    workPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(ipivPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
    ipivPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsyev(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray w, jint wIdx, jdoubleArray work, jint workIdx, jint lwork)
{
  extern void dsyev_(char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dsyev_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    workPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssyev(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray w, jint wIdx, jfloatArray work, jint workIdx, jint lwork)
{
  extern void ssyev_(char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  ssyev_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    workPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsyevd(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray w, jint wIdx, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
{
  extern void dsyevd_(char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  int info;

  savedEnv = env;
  dsyevd_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
    iworkPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    workPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsyevr(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jintArray m, jint mIdx, jdoubleArray w, jint wIdx, jdoubleArray z, jint zIdx, jint ldz, jintArray isuppz, jint isuppzIdx, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
{
  extern void dsyevr_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jdouble *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *mPtrBase = 0, *mPtr = 0;
  if (m) {
    mPtrBase = (*env)->GetIntArrayElements(env, m, NULL);
    mPtr = mPtrBase + mIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jdouble *zPtrBase = 0, *zPtr = 0;
  if (z) {
    if((*env)->IsSameObject(env, z, a) == JNI_TRUE)
      zPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, z, w) == JNI_TRUE)
      zPtrBase = wPtrBase;
    else
      zPtrBase = (*env)->GetDoubleArrayElements(env, z, NULL);
    zPtr = zPtrBase + zIdx;
  }
  jint *isuppzPtrBase = 0, *isuppzPtr = 0;
  if (isuppz) {
    if((*env)->IsSameObject(env, isuppz, m) == JNI_TRUE)
      isuppzPtrBase = mPtrBase;
    else
      isuppzPtrBase = (*env)->GetIntArrayElements(env, isuppz, NULL);
    isuppzPtr = isuppzPtrBase + isuppzIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, z) == JNI_TRUE)
      workPtrBase = zPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    if((*env)->IsSameObject(env, iwork, m) == JNI_TRUE)
      iworkPtrBase = mPtrBase;
    else
      if((*env)->IsSameObject(env, iwork, isuppz) == JNI_TRUE)
      iworkPtrBase = isuppzPtrBase;
    else
      iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  int info;

  savedEnv = env;
  dsyevr_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, isuppzPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
    if (iworkPtrBase == mPtrBase)
      mPtrBase = 0;
    if (iworkPtrBase == isuppzPtrBase)
      isuppzPtrBase = 0;
    iworkPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == zPtrBase)
      zPtrBase = 0;
    workPtrBase = 0;
  }
  if(isuppzPtrBase) {
    (*env)->ReleaseIntArrayElements(env, isuppz, isuppzPtrBase, 0);
    if (isuppzPtrBase == mPtrBase)
      mPtrBase = 0;
    isuppzPtrBase = 0;
  }
  if(zPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, z, zPtrBase, 0);
    if (zPtrBase == aPtrBase)
      aPtrBase = 0;
    if (zPtrBase == wPtrBase)
      wPtrBase = 0;
    zPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(mPtrBase) {
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
    mPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsyevx(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jintArray m, jint mIdx, jdoubleArray w, jint wIdx, jdoubleArray z, jint zIdx, jint ldz, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jintArray ifail, jint ifailIdx)
{
  extern void dsyevx_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *mPtrBase = 0, *mPtr = 0;
  if (m) {
    if((*env)->IsSameObject(env, m, iwork) == JNI_TRUE)
      mPtrBase = iworkPtrBase;
    else
      mPtrBase = (*env)->GetIntArrayElements(env, m, NULL);
    mPtr = mPtrBase + mIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jdouble *zPtrBase = 0, *zPtr = 0;
  if (z) {
    if((*env)->IsSameObject(env, z, a) == JNI_TRUE)
      zPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, z, w) == JNI_TRUE)
      zPtrBase = wPtrBase;
    else
      zPtrBase = (*env)->GetDoubleArrayElements(env, z, NULL);
    zPtr = zPtrBase + zIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, z) == JNI_TRUE)
      workPtrBase = zPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *ifailPtrBase = 0, *ifailPtr = 0;
  if (ifail) {
    if((*env)->IsSameObject(env, ifail, iwork) == JNI_TRUE)
      ifailPtrBase = iworkPtrBase;
    else
      if((*env)->IsSameObject(env, ifail, m) == JNI_TRUE)
      ifailPtrBase = mPtrBase;
    else
      ifailPtrBase = (*env)->GetIntArrayElements(env, ifail, NULL);
    ifailPtr = ifailPtrBase + ifailIdx;
  }
  int info;

  savedEnv = env;
  dsyevx_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  if(ifailPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ifail, ifailPtrBase, 0);
    if (ifailPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    if (ifailPtrBase == mPtrBase)
      mPtrBase = 0;
    ifailPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == zPtrBase)
      zPtrBase = 0;
    workPtrBase = 0;
  }
  if(zPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, z, zPtrBase, 0);
    if (zPtrBase == aPtrBase)
      aPtrBase = 0;
    if (zPtrBase == wPtrBase)
      wPtrBase = 0;
    zPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(mPtrBase) {
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
    if (mPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    mPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, JNI_ABORT);
    iworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssyevd(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray w, jint wIdx, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
{
  extern void ssyevd_(char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  int info;

  savedEnv = env;
  ssyevd_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
    iworkPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    workPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssyevr(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jintArray m, jint mIdx, jfloatArray w, jint wIdx, jfloatArray z, jint zIdx, jint ldz, jintArray isuppz, jint isuppzIdx, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
{
  extern void ssyevr_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jfloat *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *mPtrBase = 0, *mPtr = 0;
  if (m) {
    mPtrBase = (*env)->GetIntArrayElements(env, m, NULL);
    mPtr = mPtrBase + mIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jfloat *zPtrBase = 0, *zPtr = 0;
  if (z) {
    if((*env)->IsSameObject(env, z, a) == JNI_TRUE)
      zPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, z, w) == JNI_TRUE)
      zPtrBase = wPtrBase;
    else
      zPtrBase = (*env)->GetFloatArrayElements(env, z, NULL);
    zPtr = zPtrBase + zIdx;
  }
  jint *isuppzPtrBase = 0, *isuppzPtr = 0;
  if (isuppz) {
    if((*env)->IsSameObject(env, isuppz, m) == JNI_TRUE)
      isuppzPtrBase = mPtrBase;
    else
      isuppzPtrBase = (*env)->GetIntArrayElements(env, isuppz, NULL);
    isuppzPtr = isuppzPtrBase + isuppzIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, z) == JNI_TRUE)
      workPtrBase = zPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    if((*env)->IsSameObject(env, iwork, m) == JNI_TRUE)
      iworkPtrBase = mPtrBase;
    else
      if((*env)->IsSameObject(env, iwork, isuppz) == JNI_TRUE)
      iworkPtrBase = isuppzPtrBase;
    else
      iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  int info;

  savedEnv = env;
  ssyevr_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, isuppzPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
    if (iworkPtrBase == mPtrBase)
      mPtrBase = 0;
    if (iworkPtrBase == isuppzPtrBase)
      isuppzPtrBase = 0;
    iworkPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == zPtrBase)
      zPtrBase = 0;
    workPtrBase = 0;
  }
  if(isuppzPtrBase) {
    (*env)->ReleaseIntArrayElements(env, isuppz, isuppzPtrBase, 0);
    if (isuppzPtrBase == mPtrBase)
      mPtrBase = 0;
    isuppzPtrBase = 0;
  }
  if(zPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, z, zPtrBase, 0);
    if (zPtrBase == aPtrBase)
      aPtrBase = 0;
    if (zPtrBase == wPtrBase)
      wPtrBase = 0;
    zPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(mPtrBase) {
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
    mPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssyevx(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jintArray m, jint mIdx, jfloatArray w, jint wIdx, jfloatArray z, jint zIdx, jint ldz, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jintArray ifail, jint ifailIdx)
{
  extern void ssyevx_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *mPtrBase = 0, *mPtr = 0;
  if (m) {
    if((*env)->IsSameObject(env, m, iwork) == JNI_TRUE)
      mPtrBase = iworkPtrBase;
    else
      mPtrBase = (*env)->GetIntArrayElements(env, m, NULL);
    mPtr = mPtrBase + mIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jfloat *zPtrBase = 0, *zPtr = 0;
  if (z) {
    if((*env)->IsSameObject(env, z, a) == JNI_TRUE)
      zPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, z, w) == JNI_TRUE)
      zPtrBase = wPtrBase;
    else
      zPtrBase = (*env)->GetFloatArrayElements(env, z, NULL);
    zPtr = zPtrBase + zIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, z) == JNI_TRUE)
      workPtrBase = zPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *ifailPtrBase = 0, *ifailPtr = 0;
  if (ifail) {
    if((*env)->IsSameObject(env, ifail, iwork) == JNI_TRUE)
      ifailPtrBase = iworkPtrBase;
    else
      if((*env)->IsSameObject(env, ifail, m) == JNI_TRUE)
      ifailPtrBase = mPtrBase;
    else
      ifailPtrBase = (*env)->GetIntArrayElements(env, ifail, NULL);
    ifailPtr = ifailPtrBase + ifailIdx;
  }
  int info;

  savedEnv = env;
  ssyevx_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  if(ifailPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ifail, ifailPtrBase, 0);
    if (ifailPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    if (ifailPtrBase == mPtrBase)
      mPtrBase = 0;
    ifailPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == zPtrBase)
      zPtrBase = 0;
    workPtrBase = 0;
  }
  if(zPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, z, zPtrBase, 0);
    if (zPtrBase == aPtrBase)
      aPtrBase = 0;
    if (zPtrBase == wPtrBase)
      wPtrBase = 0;
    zPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(mPtrBase) {
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
    if (mPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    mPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, JNI_ABORT);
    iworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dposv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb)
{
  extern void dposv_(char *, jint *, jint *, jdouble *, jint *, jdouble *, jint *, int *);
  
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  int info;

  savedEnv = env;
  dposv_(&uploChr, &n, &nrhs, aPtr, &lda, bPtr, &ldb, &info);
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sposv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb)
{
  extern void sposv_(char *, jint *, jint *, jfloat *, jint *, jfloat *, jint *, int *);
  
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  int info;

  savedEnv = env;
  sposv_(&uploChr, &n, &nrhs, aPtr, &lda, bPtr, &ldb, &info);
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_cgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray w, jint wIdx, jfloatArray vl, jint vlIdx, jint ldvl, jfloatArray vr, jint vrIdx, jint ldvr, jfloatArray work, jint workIdx, jint lwork, jfloatArray rwork, jint rworkIdx)
{
  extern void cgeev_(char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, int *);
  
  char jobvlChr = (char) jobvl;
  char jobvrChr = (char) jobvr;
  jfloat *rworkPtrBase = 0, *rworkPtr = 0;
  if (rwork) {
    rworkPtrBase = (*env)->GetFloatArrayElements(env, rwork, NULL);
    rworkPtr = rworkPtrBase + rworkIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, rwork) == JNI_TRUE)
      aPtrBase = rworkPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, rwork) == JNI_TRUE)
      wPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + 2*wIdx;
  }
  jfloat *vlPtrBase = 0, *vlPtr = 0;
  if (vl) {
    if((*env)->IsSameObject(env, vl, rwork) == JNI_TRUE)
      vlPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, vl, a) == JNI_TRUE)
      vlPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vl, w) == JNI_TRUE)
      vlPtrBase = wPtrBase;
    else
      vlPtrBase = (*env)->GetFloatArrayElements(env, vl, NULL);
    vlPtr = vlPtrBase + 2*vlIdx;
  }
  jfloat *vrPtrBase = 0, *vrPtr = 0;
  if (vr) {
    if((*env)->IsSameObject(env, vr, rwork) == JNI_TRUE)
      vrPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, vr, a) == JNI_TRUE)
      vrPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vr, w) == JNI_TRUE)
      vrPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, vr, vl) == JNI_TRUE)
      vrPtrBase = vlPtrBase;
    else
      vrPtrBase = (*env)->GetFloatArrayElements(env, vr, NULL);
    vrPtr = vrPtrBase + 2*vrIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, rwork) == JNI_TRUE)
      workPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, vl) == JNI_TRUE)
      workPtrBase = vlPtrBase;
    else
      if((*env)->IsSameObject(env, work, vr) == JNI_TRUE)
      workPtrBase = vrPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + 2*workIdx;
  }
  int info;

  savedEnv = env;
  cgeev_(&jobvlChr, &jobvrChr, &n, aPtr, &lda, wPtr, vlPtr, &ldvl, vrPtr, &ldvr, workPtr, &lwork, rworkPtr, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == vlPtrBase)
      vlPtrBase = 0;
    if (workPtrBase == vrPtrBase)
      vrPtrBase = 0;
    workPtrBase = 0;
  }
  if(vrPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, vr, vrPtrBase, 0);
    if (vrPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (vrPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vrPtrBase == wPtrBase)
      wPtrBase = 0;
    if (vrPtrBase == vlPtrBase)
      vlPtrBase = 0;
    vrPtrBase = 0;
  }
  if(vlPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, vl, vlPtrBase, 0);
    if (vlPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (vlPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vlPtrBase == wPtrBase)
      wPtrBase = 0;
    vlPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    aPtrBase = 0;
  }
  if(rworkPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, rwork, rworkPtrBase, JNI_ABORT);
    rworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray wr, jint wrIdx, jdoubleArray wi, jint wiIdx, jdoubleArray vl, jint vlIdx, jint ldvl, jdoubleArray vr, jint vrIdx, jint ldvr, jdoubleArray work, jint workIdx, jint lwork)
{
  extern void dgeev_(char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *, int *);
  
  char jobvlChr = (char) jobvl;
  char jobvrChr = (char) jobvr;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *wrPtrBase = 0, *wrPtr = 0;
  if (wr) {
    if((*env)->IsSameObject(env, wr, a) == JNI_TRUE)
      wrPtrBase = aPtrBase;
    else
      wrPtrBase = (*env)->GetDoubleArrayElements(env, wr, NULL);
    wrPtr = wrPtrBase + wrIdx;
  }
  jdouble *wiPtrBase = 0, *wiPtr = 0;
  if (wi) {
    if((*env)->IsSameObject(env, wi, a) == JNI_TRUE)
      wiPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, wi, wr) == JNI_TRUE)
      wiPtrBase = wrPtrBase;
    else
      wiPtrBase = (*env)->GetDoubleArrayElements(env, wi, NULL);
    wiPtr = wiPtrBase + wiIdx;
  }
  jdouble *vlPtrBase = 0, *vlPtr = 0;
  if (vl) {
    if((*env)->IsSameObject(env, vl, a) == JNI_TRUE)
      vlPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vl, wr) == JNI_TRUE)
      vlPtrBase = wrPtrBase;
    else
      if((*env)->IsSameObject(env, vl, wi) == JNI_TRUE)
      vlPtrBase = wiPtrBase;
    else
      vlPtrBase = (*env)->GetDoubleArrayElements(env, vl, NULL);
    vlPtr = vlPtrBase + vlIdx;
  }
  jdouble *vrPtrBase = 0, *vrPtr = 0;
  if (vr) {
    if((*env)->IsSameObject(env, vr, a) == JNI_TRUE)
      vrPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vr, wr) == JNI_TRUE)
      vrPtrBase = wrPtrBase;
    else
      if((*env)->IsSameObject(env, vr, wi) == JNI_TRUE)
      vrPtrBase = wiPtrBase;
    else
      if((*env)->IsSameObject(env, vr, vl) == JNI_TRUE)
      vrPtrBase = vlPtrBase;
    else
      vrPtrBase = (*env)->GetDoubleArrayElements(env, vr, NULL);
    vrPtr = vrPtrBase + vrIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, wr) == JNI_TRUE)
      workPtrBase = wrPtrBase;
    else
      if((*env)->IsSameObject(env, work, wi) == JNI_TRUE)
      workPtrBase = wiPtrBase;
    else
      if((*env)->IsSameObject(env, work, vl) == JNI_TRUE)
      workPtrBase = vlPtrBase;
    else
      if((*env)->IsSameObject(env, work, vr) == JNI_TRUE)
      workPtrBase = vrPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dgeev_(&jobvlChr, &jobvrChr, &n, aPtr, &lda, wrPtr, wiPtr, vlPtr, &ldvl, vrPtr, &ldvr, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wrPtrBase)
      wrPtrBase = 0;
    if (workPtrBase == wiPtrBase)
      wiPtrBase = 0;
    if (workPtrBase == vlPtrBase)
      vlPtrBase = 0;
    if (workPtrBase == vrPtrBase)
      vrPtrBase = 0;
    workPtrBase = 0;
  }
  if(vrPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, vr, vrPtrBase, 0);
    if (vrPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vrPtrBase == wrPtrBase)
      wrPtrBase = 0;
    if (vrPtrBase == wiPtrBase)
      wiPtrBase = 0;
    if (vrPtrBase == vlPtrBase)
      vlPtrBase = 0;
    vrPtrBase = 0;
  }
  if(vlPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, vl, vlPtrBase, 0);
    if (vlPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vlPtrBase == wrPtrBase)
      wrPtrBase = 0;
    if (vlPtrBase == wiPtrBase)
      wiPtrBase = 0;
    vlPtrBase = 0;
  }
  if(wiPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, wi, wiPtrBase, 0);
    if (wiPtrBase == aPtrBase)
      aPtrBase = 0;
    if (wiPtrBase == wrPtrBase)
      wrPtrBase = 0;
    wiPtrBase = 0;
  }
  if(wrPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, wr, wrPtrBase, 0);
    if (wrPtrBase == aPtrBase)
      aPtrBase = 0;
    wrPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray wr, jint wrIdx, jfloatArray wi, jint wiIdx, jfloatArray vl, jint vlIdx, jint ldvl, jfloatArray vr, jint vrIdx, jint ldvr, jfloatArray work, jint workIdx, jint lwork)
{
  extern void sgeev_(char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *, int *);
  
  char jobvlChr = (char) jobvl;
  char jobvrChr = (char) jobvr;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *wrPtrBase = 0, *wrPtr = 0;
  if (wr) {
    if((*env)->IsSameObject(env, wr, a) == JNI_TRUE)
      wrPtrBase = aPtrBase;
    else
      wrPtrBase = (*env)->GetFloatArrayElements(env, wr, NULL);
    wrPtr = wrPtrBase + wrIdx;
  }
  jfloat *wiPtrBase = 0, *wiPtr = 0;
  if (wi) {
    if((*env)->IsSameObject(env, wi, a) == JNI_TRUE)
      wiPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, wi, wr) == JNI_TRUE)
      wiPtrBase = wrPtrBase;
    else
      wiPtrBase = (*env)->GetFloatArrayElements(env, wi, NULL);
    wiPtr = wiPtrBase + wiIdx;
  }
  jfloat *vlPtrBase = 0, *vlPtr = 0;
  if (vl) {
    if((*env)->IsSameObject(env, vl, a) == JNI_TRUE)
      vlPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vl, wr) == JNI_TRUE)
      vlPtrBase = wrPtrBase;
    else
      if((*env)->IsSameObject(env, vl, wi) == JNI_TRUE)
      vlPtrBase = wiPtrBase;
    else
      vlPtrBase = (*env)->GetFloatArrayElements(env, vl, NULL);
    vlPtr = vlPtrBase + vlIdx;
  }
  jfloat *vrPtrBase = 0, *vrPtr = 0;
  if (vr) {
    if((*env)->IsSameObject(env, vr, a) == JNI_TRUE)
      vrPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vr, wr) == JNI_TRUE)
      vrPtrBase = wrPtrBase;
    else
      if((*env)->IsSameObject(env, vr, wi) == JNI_TRUE)
      vrPtrBase = wiPtrBase;
    else
      if((*env)->IsSameObject(env, vr, vl) == JNI_TRUE)
      vrPtrBase = vlPtrBase;
    else
      vrPtrBase = (*env)->GetFloatArrayElements(env, vr, NULL);
    vrPtr = vrPtrBase + vrIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, wr) == JNI_TRUE)
      workPtrBase = wrPtrBase;
    else
      if((*env)->IsSameObject(env, work, wi) == JNI_TRUE)
      workPtrBase = wiPtrBase;
    else
      if((*env)->IsSameObject(env, work, vl) == JNI_TRUE)
      workPtrBase = vlPtrBase;
    else
      if((*env)->IsSameObject(env, work, vr) == JNI_TRUE)
      workPtrBase = vrPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  sgeev_(&jobvlChr, &jobvrChr, &n, aPtr, &lda, wrPtr, wiPtr, vlPtr, &ldvl, vrPtr, &ldvr, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wrPtrBase)
      wrPtrBase = 0;
    if (workPtrBase == wiPtrBase)
      wiPtrBase = 0;
    if (workPtrBase == vlPtrBase)
      vlPtrBase = 0;
    if (workPtrBase == vrPtrBase)
      vrPtrBase = 0;
    workPtrBase = 0;
  }
  if(vrPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, vr, vrPtrBase, 0);
    if (vrPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vrPtrBase == wrPtrBase)
      wrPtrBase = 0;
    if (vrPtrBase == wiPtrBase)
      wiPtrBase = 0;
    if (vrPtrBase == vlPtrBase)
      vlPtrBase = 0;
    vrPtrBase = 0;
  }
  if(vlPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, vl, vlPtrBase, 0);
    if (vlPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vlPtrBase == wrPtrBase)
      wrPtrBase = 0;
    if (vlPtrBase == wiPtrBase)
      wiPtrBase = 0;
    vlPtrBase = 0;
  }
  if(wiPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, wi, wiPtrBase, 0);
    if (wiPtrBase == aPtrBase)
      aPtrBase = 0;
    if (wiPtrBase == wrPtrBase)
      wrPtrBase = 0;
    wiPtrBase = 0;
  }
  if(wrPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, wr, wrPtrBase, 0);
    if (wrPtrBase == aPtrBase)
      aPtrBase = 0;
    wrPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_zgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray w, jint wIdx, jdoubleArray vl, jint vlIdx, jint ldvl, jdoubleArray vr, jint vrIdx, jint ldvr, jdoubleArray work, jint workIdx, jint lwork, jdoubleArray rwork, jint rworkIdx)
{
  extern void zgeev_(char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, int *);
  
  char jobvlChr = (char) jobvl;
  char jobvrChr = (char) jobvr;
  jdouble *rworkPtrBase = 0, *rworkPtr = 0;
  if (rwork) {
    rworkPtrBase = (*env)->GetDoubleArrayElements(env, rwork, NULL);
    rworkPtr = rworkPtrBase + rworkIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, rwork) == JNI_TRUE)
      aPtrBase = rworkPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, rwork) == JNI_TRUE)
      wPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + 2*wIdx;
  }
  jdouble *vlPtrBase = 0, *vlPtr = 0;
  if (vl) {
    if((*env)->IsSameObject(env, vl, rwork) == JNI_TRUE)
      vlPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, vl, a) == JNI_TRUE)
      vlPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vl, w) == JNI_TRUE)
      vlPtrBase = wPtrBase;
    else
      vlPtrBase = (*env)->GetDoubleArrayElements(env, vl, NULL);
    vlPtr = vlPtrBase + 2*vlIdx;
  }
  jdouble *vrPtrBase = 0, *vrPtr = 0;
  if (vr) {
    if((*env)->IsSameObject(env, vr, rwork) == JNI_TRUE)
      vrPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, vr, a) == JNI_TRUE)
      vrPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vr, w) == JNI_TRUE)
      vrPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, vr, vl) == JNI_TRUE)
      vrPtrBase = vlPtrBase;
    else
      vrPtrBase = (*env)->GetDoubleArrayElements(env, vr, NULL);
    vrPtr = vrPtrBase + 2*vrIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, rwork) == JNI_TRUE)
      workPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, vl) == JNI_TRUE)
      workPtrBase = vlPtrBase;
    else
      if((*env)->IsSameObject(env, work, vr) == JNI_TRUE)
      workPtrBase = vrPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + 2*workIdx;
  }
  int info;

  savedEnv = env;
  zgeev_(&jobvlChr, &jobvrChr, &n, aPtr, &lda, wPtr, vlPtr, &ldvl, vrPtr, &ldvr, workPtr, &lwork, rworkPtr, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == vlPtrBase)
      vlPtrBase = 0;
    if (workPtrBase == vrPtrBase)
      vrPtrBase = 0;
    workPtrBase = 0;
  }
  if(vrPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, vr, vrPtrBase, 0);
    if (vrPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (vrPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vrPtrBase == wPtrBase)
      wPtrBase = 0;
    if (vrPtrBase == vlPtrBase)
      vlPtrBase = 0;
    vrPtrBase = 0;
  }
  if(vlPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, vl, vlPtrBase, 0);
    if (vlPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (vlPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vlPtrBase == wPtrBase)
      wPtrBase = 0;
    vlPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    wPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    aPtrBase = 0;
  }
  if(rworkPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, rwork, rworkPtrBase, JNI_ABORT);
    rworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dgetrf(JNIEnv *env, jclass this, jint m, jint n, jdoubleArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx)
{
  extern void dgetrf_(jint *, jint *, jdouble *, jint *, jint *, int *);
  
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *ipivPtrBase = 0, *ipivPtr = 0;
  if (ipiv) {
    ipivPtrBase = (*env)->GetIntArrayElements(env, ipiv, NULL);
    ipivPtr = ipivPtrBase + ipivIdx;
  }
  int info;

  savedEnv = env;
  dgetrf_(&m, &n, aPtr, &lda, ipivPtr, &info);
  if(ipivPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
    ipivPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sgetrf(JNIEnv *env, jclass this, jint m, jint n, jfloatArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx)
{
  extern void sgetrf_(jint *, jint *, jfloat *, jint *, jint *, int *);
  
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *ipivPtrBase = 0, *ipivPtr = 0;
  if (ipiv) {
    ipivPtrBase = (*env)->GetIntArrayElements(env, ipiv, NULL);
    ipivPtr = ipivPtrBase + ipivIdx;
  }
  int info;

  savedEnv = env;
  sgetrf_(&m, &n, aPtr, &lda, ipivPtr, &info);
  if(ipivPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
    ipivPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dpotrf(JNIEnv *env, jclass this, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda)
{
  extern void dpotrf_(char *, jint *, jdouble *, jint *, int *);
  
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  int info;

  savedEnv = env;
  dpotrf_(&uploChr, &n, aPtr, &lda, &info);
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_spotrf(JNIEnv *env, jclass this, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda)
{
  extern void spotrf_(char *, jint *, jfloat *, jint *, int *);
  
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  int info;

  savedEnv = env;
  spotrf_(&uploChr, &n, aPtr, &lda, &info);
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_cgesvd(JNIEnv *env, jclass this, jchar jobu, jchar jobvt, jint m, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray s, jint sIdx, jfloatArray u, jint uIdx, jint ldu, jfloatArray vt, jint vtIdx, jint ldvt, jfloatArray work, jint workIdx, jint lwork, jfloatArray rwork, jint rworkIdx)
{
  extern void cgesvd_(char *, char *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, int *);
  
  char jobuChr = (char) jobu;
  char jobvtChr = (char) jobvt;
  jfloat *rworkPtrBase = 0, *rworkPtr = 0;
  if (rwork) {
    rworkPtrBase = (*env)->GetFloatArrayElements(env, rwork, NULL);
    rworkPtr = rworkPtrBase + rworkIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, rwork) == JNI_TRUE)
      aPtrBase = rworkPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jfloat *sPtrBase = 0, *sPtr = 0;
  if (s) {
    if((*env)->IsSameObject(env, s, rwork) == JNI_TRUE)
      sPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, s, a) == JNI_TRUE)
      sPtrBase = aPtrBase;
    else
      sPtrBase = (*env)->GetFloatArrayElements(env, s, NULL);
    sPtr = sPtrBase + sIdx;
  }
  jfloat *uPtrBase = 0, *uPtr = 0;
  if (u) {
    if((*env)->IsSameObject(env, u, rwork) == JNI_TRUE)
      uPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, u, a) == JNI_TRUE)
      uPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, u, s) == JNI_TRUE)
      uPtrBase = sPtrBase;
    else
      uPtrBase = (*env)->GetFloatArrayElements(env, u, NULL);
    uPtr = uPtrBase + 2*uIdx;
  }
  jfloat *vtPtrBase = 0, *vtPtr = 0;
  if (vt) {
    if((*env)->IsSameObject(env, vt, rwork) == JNI_TRUE)
      vtPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, vt, a) == JNI_TRUE)
      vtPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vt, s) == JNI_TRUE)
      vtPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, vt, u) == JNI_TRUE)
      vtPtrBase = uPtrBase;
    else
      vtPtrBase = (*env)->GetFloatArrayElements(env, vt, NULL);
    vtPtr = vtPtrBase + 2*vtIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, rwork) == JNI_TRUE)
      workPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, s) == JNI_TRUE)
      workPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, work, u) == JNI_TRUE)
      workPtrBase = uPtrBase;
    else
      if((*env)->IsSameObject(env, work, vt) == JNI_TRUE)
      workPtrBase = vtPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + 2*workIdx;
  }
  int info;

  savedEnv = env;
  cgesvd_(&jobuChr, &jobvtChr, &m, &n, aPtr, &lda, sPtr, uPtr, &ldu, vtPtr, &ldvt, workPtr, &lwork, rworkPtr, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == sPtrBase)
      sPtrBase = 0;
    if (workPtrBase == uPtrBase)
      uPtrBase = 0;
    if (workPtrBase == vtPtrBase)
      vtPtrBase = 0;
    workPtrBase = 0;
  }
  if(vtPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, vt, vtPtrBase, 0);
    if (vtPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (vtPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vtPtrBase == sPtrBase)
      sPtrBase = 0;
    if (vtPtrBase == uPtrBase)
      uPtrBase = 0;
    vtPtrBase = 0;
  }
  if(uPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, u, uPtrBase, 0);
    if (uPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (uPtrBase == aPtrBase)
      aPtrBase = 0;
    if (uPtrBase == sPtrBase)
      sPtrBase = 0;
    uPtrBase = 0;
  }
  if(sPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, s, sPtrBase, 0);
    if (sPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (sPtrBase == aPtrBase)
      aPtrBase = 0;
    sPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    aPtrBase = 0;
  }
  if(rworkPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, rwork, rworkPtrBase, JNI_ABORT);
    rworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dgesvd(JNIEnv *env, jclass this, jchar jobu, jchar jobvt, jint m, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray s, jint sIdx, jdoubleArray u, jint uIdx, jint ldu, jdoubleArray vt, jint vtIdx, jint ldvt, jdoubleArray work, jint workIdx, jint lwork)
{
  extern void dgesvd_(char *, char *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *, int *);
  
  char jobuChr = (char) jobu;
  char jobvtChr = (char) jobvt;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *sPtrBase = 0, *sPtr = 0;
  if (s) {
    if((*env)->IsSameObject(env, s, a) == JNI_TRUE)
      sPtrBase = aPtrBase;
    else
      sPtrBase = (*env)->GetDoubleArrayElements(env, s, NULL);
    sPtr = sPtrBase + sIdx;
  }
  jdouble *uPtrBase = 0, *uPtr = 0;
  if (u) {
    if((*env)->IsSameObject(env, u, a) == JNI_TRUE)
      uPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, u, s) == JNI_TRUE)
      uPtrBase = sPtrBase;
    else
      uPtrBase = (*env)->GetDoubleArrayElements(env, u, NULL);
    uPtr = uPtrBase + uIdx;
  }
  jdouble *vtPtrBase = 0, *vtPtr = 0;
  if (vt) {
    if((*env)->IsSameObject(env, vt, a) == JNI_TRUE)
      vtPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vt, s) == JNI_TRUE)
      vtPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, vt, u) == JNI_TRUE)
      vtPtrBase = uPtrBase;
    else
      vtPtrBase = (*env)->GetDoubleArrayElements(env, vt, NULL);
    vtPtr = vtPtrBase + vtIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, s) == JNI_TRUE)
      workPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, work, u) == JNI_TRUE)
      workPtrBase = uPtrBase;
    else
      if((*env)->IsSameObject(env, work, vt) == JNI_TRUE)
      workPtrBase = vtPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dgesvd_(&jobuChr, &jobvtChr, &m, &n, aPtr, &lda, sPtr, uPtr, &ldu, vtPtr, &ldvt, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == sPtrBase)
      sPtrBase = 0;
    if (workPtrBase == uPtrBase)
      uPtrBase = 0;
    if (workPtrBase == vtPtrBase)
      vtPtrBase = 0;
    workPtrBase = 0;
  }
  if(vtPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, vt, vtPtrBase, 0);
    if (vtPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vtPtrBase == sPtrBase)
      sPtrBase = 0;
    if (vtPtrBase == uPtrBase)
      uPtrBase = 0;
    vtPtrBase = 0;
  }
  if(uPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, u, uPtrBase, 0);
    if (uPtrBase == aPtrBase)
      aPtrBase = 0;
    if (uPtrBase == sPtrBase)
      sPtrBase = 0;
    uPtrBase = 0;
  }
  if(sPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, s, sPtrBase, 0);
    if (sPtrBase == aPtrBase)
      aPtrBase = 0;
    sPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sgesvd(JNIEnv *env, jclass this, jchar jobu, jchar jobvt, jint m, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray s, jint sIdx, jfloatArray u, jint uIdx, jint ldu, jfloatArray vt, jint vtIdx, jint ldvt, jfloatArray work, jint workIdx, jint lwork)
{
  extern void sgesvd_(char *, char *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *, int *);
  
  char jobuChr = (char) jobu;
  char jobvtChr = (char) jobvt;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *sPtrBase = 0, *sPtr = 0;
  if (s) {
    if((*env)->IsSameObject(env, s, a) == JNI_TRUE)
      sPtrBase = aPtrBase;
    else
      sPtrBase = (*env)->GetFloatArrayElements(env, s, NULL);
    sPtr = sPtrBase + sIdx;
  }
  jfloat *uPtrBase = 0, *uPtr = 0;
  if (u) {
    if((*env)->IsSameObject(env, u, a) == JNI_TRUE)
      uPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, u, s) == JNI_TRUE)
      uPtrBase = sPtrBase;
    else
      uPtrBase = (*env)->GetFloatArrayElements(env, u, NULL);
    uPtr = uPtrBase + uIdx;
  }
  jfloat *vtPtrBase = 0, *vtPtr = 0;
  if (vt) {
    if((*env)->IsSameObject(env, vt, a) == JNI_TRUE)
      vtPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vt, s) == JNI_TRUE)
      vtPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, vt, u) == JNI_TRUE)
      vtPtrBase = uPtrBase;
    else
      vtPtrBase = (*env)->GetFloatArrayElements(env, vt, NULL);
    vtPtr = vtPtrBase + vtIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, s) == JNI_TRUE)
      workPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, work, u) == JNI_TRUE)
      workPtrBase = uPtrBase;
    else
      if((*env)->IsSameObject(env, work, vt) == JNI_TRUE)
      workPtrBase = vtPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  sgesvd_(&jobuChr, &jobvtChr, &m, &n, aPtr, &lda, sPtr, uPtr, &ldu, vtPtr, &ldvt, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == sPtrBase)
      sPtrBase = 0;
    if (workPtrBase == uPtrBase)
      uPtrBase = 0;
    if (workPtrBase == vtPtrBase)
      vtPtrBase = 0;
    workPtrBase = 0;
  }
  if(vtPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, vt, vtPtrBase, 0);
    if (vtPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vtPtrBase == sPtrBase)
      sPtrBase = 0;
    if (vtPtrBase == uPtrBase)
      uPtrBase = 0;
    vtPtrBase = 0;
  }
  if(uPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, u, uPtrBase, 0);
    if (uPtrBase == aPtrBase)
      aPtrBase = 0;
    if (uPtrBase == sPtrBase)
      sPtrBase = 0;
    uPtrBase = 0;
  }
  if(sPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, s, sPtrBase, 0);
    if (sPtrBase == aPtrBase)
      aPtrBase = 0;
    sPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_zgesvd(JNIEnv *env, jclass this, jchar jobu, jchar jobvt, jint m, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray s, jint sIdx, jdoubleArray u, jint uIdx, jint ldu, jdoubleArray vt, jint vtIdx, jint ldvt, jdoubleArray work, jint workIdx, jint lwork, jdoubleArray rwork, jint rworkIdx)
{
  extern void zgesvd_(char *, char *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, int *);
  
  char jobuChr = (char) jobu;
  char jobvtChr = (char) jobvt;
  jdouble *rworkPtrBase = 0, *rworkPtr = 0;
  if (rwork) {
    rworkPtrBase = (*env)->GetDoubleArrayElements(env, rwork, NULL);
    rworkPtr = rworkPtrBase + rworkIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, rwork) == JNI_TRUE)
      aPtrBase = rworkPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jdouble *sPtrBase = 0, *sPtr = 0;
  if (s) {
    if((*env)->IsSameObject(env, s, rwork) == JNI_TRUE)
      sPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, s, a) == JNI_TRUE)
      sPtrBase = aPtrBase;
    else
      sPtrBase = (*env)->GetDoubleArrayElements(env, s, NULL);
    sPtr = sPtrBase + sIdx;
  }
  jdouble *uPtrBase = 0, *uPtr = 0;
  if (u) {
    if((*env)->IsSameObject(env, u, rwork) == JNI_TRUE)
      uPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, u, a) == JNI_TRUE)
      uPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, u, s) == JNI_TRUE)
      uPtrBase = sPtrBase;
    else
      uPtrBase = (*env)->GetDoubleArrayElements(env, u, NULL);
    uPtr = uPtrBase + 2*uIdx;
  }
  jdouble *vtPtrBase = 0, *vtPtr = 0;
  if (vt) {
    if((*env)->IsSameObject(env, vt, rwork) == JNI_TRUE)
      vtPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, vt, a) == JNI_TRUE)
      vtPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, vt, s) == JNI_TRUE)
      vtPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, vt, u) == JNI_TRUE)
      vtPtrBase = uPtrBase;
    else
      vtPtrBase = (*env)->GetDoubleArrayElements(env, vt, NULL);
    vtPtr = vtPtrBase + 2*vtIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, rwork) == JNI_TRUE)
      workPtrBase = rworkPtrBase;
    else
      if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, s) == JNI_TRUE)
      workPtrBase = sPtrBase;
    else
      if((*env)->IsSameObject(env, work, u) == JNI_TRUE)
      workPtrBase = uPtrBase;
    else
      if((*env)->IsSameObject(env, work, vt) == JNI_TRUE)
      workPtrBase = vtPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + 2*workIdx;
  }
  int info;

  savedEnv = env;
  zgesvd_(&jobuChr, &jobvtChr, &m, &n, aPtr, &lda, sPtr, uPtr, &ldu, vtPtr, &ldvt, workPtr, &lwork, rworkPtr, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == sPtrBase)
      sPtrBase = 0;
    if (workPtrBase == uPtrBase)
      uPtrBase = 0;
    if (workPtrBase == vtPtrBase)
      vtPtrBase = 0;
    workPtrBase = 0;
  }
  if(vtPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, vt, vtPtrBase, 0);
    if (vtPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (vtPtrBase == aPtrBase)
      aPtrBase = 0;
    if (vtPtrBase == sPtrBase)
      sPtrBase = 0;
    if (vtPtrBase == uPtrBase)
      uPtrBase = 0;
    vtPtrBase = 0;
  }
  if(uPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, u, uPtrBase, 0);
    if (uPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (uPtrBase == aPtrBase)
      aPtrBase = 0;
    if (uPtrBase == sPtrBase)
      sPtrBase = 0;
    uPtrBase = 0;
  }
  if(sPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, s, sPtrBase, 0);
    if (sPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    if (sPtrBase == aPtrBase)
      aPtrBase = 0;
    sPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    if (aPtrBase == rworkPtrBase)
      rworkPtrBase = 0;
    aPtrBase = 0;
  }
  if(rworkPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, rwork, rworkPtrBase, JNI_ABORT);
    rworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsygvd(JNIEnv *env, jclass this, jint itype, jchar jobz, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdoubleArray w, jint wIdx, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
{
  extern void dsygvd_(jint *, char *, char *, jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, w, b) == JNI_TRUE)
      wPtrBase = bPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  int info;

  savedEnv = env;
  dsygvd_(&itype, &jobzChr, &uploChr, &n, aPtr, &lda, bPtr, &ldb, wPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
    iworkPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    workPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    if (wPtrBase == bPtrBase)
      bPtrBase = 0;
    wPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssygvd(JNIEnv *env, jclass this, jint itype, jchar jobz, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloatArray w, jint wIdx, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
{
  extern void ssygvd_(jint *, char *, char *, jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, w, b) == JNI_TRUE)
      wPtrBase = bPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  int info;

  savedEnv = env;
  ssygvd_(&itype, &jobzChr, &uploChr, &n, aPtr, &lda, bPtr, &ldb, wPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
    iworkPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    workPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    if (wPtrBase == bPtrBase)
      bPtrBase = 0;
    wPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dgelsd(JNIEnv *env, jclass this, jint m, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdoubleArray s, jint sIdx, jdouble rcond, jintArray rank, jint rankIdx, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx)
{
  extern void dgelsd_(jint *, jint *, jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jint *, int *);
  
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jdouble *sPtrBase = 0, *sPtr = 0;
  if (s) {
    if((*env)->IsSameObject(env, s, a) == JNI_TRUE)
      sPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, s, b) == JNI_TRUE)
      sPtrBase = bPtrBase;
    else
      sPtrBase = (*env)->GetDoubleArrayElements(env, s, NULL);
    sPtr = sPtrBase + sIdx;
  }
  jint *rankPtrBase = 0, *rankPtr = 0;
  if (rank) {
    if((*env)->IsSameObject(env, rank, iwork) == JNI_TRUE)
      rankPtrBase = iworkPtrBase;
    else
      rankPtrBase = (*env)->GetIntArrayElements(env, rank, NULL);
    rankPtr = rankPtrBase + rankIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, work, s) == JNI_TRUE)
      workPtrBase = sPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dgelsd_(&m, &n, &nrhs, aPtr, &lda, bPtr, &ldb, sPtr, &rcond, rankPtr, workPtr, &lwork, iworkPtr, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    if (workPtrBase == sPtrBase)
      sPtrBase = 0;
    workPtrBase = 0;
  }
  if(rankPtrBase) {
    (*env)->ReleaseIntArrayElements(env, rank, rankPtrBase, 0);
    if (rankPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    rankPtrBase = 0;
  }
  if(sPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, s, sPtrBase, 0);
    if (sPtrBase == aPtrBase)
      aPtrBase = 0;
    if (sPtrBase == bPtrBase)
      bPtrBase = 0;
    sPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, JNI_ABORT);
    iworkPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sgelsd(JNIEnv *env, jclass this, jint m, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloatArray s, jint sIdx, jfloat rcond, jintArray rank, jint rankIdx, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx)
{
  extern void sgelsd_(jint *, jint *, jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jint *, int *);
  
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jfloat *sPtrBase = 0, *sPtr = 0;
  if (s) {
    if((*env)->IsSameObject(env, s, a) == JNI_TRUE)
      sPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, s, b) == JNI_TRUE)
      sPtrBase = bPtrBase;
    else
      sPtrBase = (*env)->GetFloatArrayElements(env, s, NULL);
    sPtr = sPtrBase + sIdx;
  }
  jint *rankPtrBase = 0, *rankPtr = 0;
  if (rank) {
    if((*env)->IsSameObject(env, rank, iwork) == JNI_TRUE)
      rankPtrBase = iworkPtrBase;
    else
      rankPtrBase = (*env)->GetIntArrayElements(env, rank, NULL);
    rankPtr = rankPtrBase + rankIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, work, s) == JNI_TRUE)
      workPtrBase = sPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  sgelsd_(&m, &n, &nrhs, aPtr, &lda, bPtr, &ldb, sPtr, &rcond, rankPtr, workPtr, &lwork, iworkPtr, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    if (workPtrBase == sPtrBase)
      sPtrBase = 0;
    workPtrBase = 0;
  }
  if(rankPtrBase) {
    (*env)->ReleaseIntArrayElements(env, rank, rankPtrBase, 0);
    if (rankPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    rankPtrBase = 0;
  }
  if(sPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, s, sPtrBase, 0);
    if (sPtrBase == aPtrBase)
      aPtrBase = 0;
    if (sPtrBase == bPtrBase)
      bPtrBase = 0;
    sPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, JNI_ABORT);
    iworkPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ilaenv(JNIEnv *env, jclass this, jint ispec, jstring name, jstring opts, jint n1, jint n2, jint n3, jint n4)
{
  extern jint ilaenv_(jint *, char *, char *, jint *, jint *, jint *, jint *);
  
  char *nameStr = (*env)->GetStringChars(env, name, NULL);
  char *optsStr = (*env)->GetStringChars(env, opts, NULL);

  savedEnv = env;
  jint retval = ilaenv_(&ispec, nameStr, optsStr, &n1, &n2, &n3, &n4);
  (*env)->ReleaseStringChars(env, name, nameStr);
  (*env)->ReleaseStringChars(env, opts, optsStr);

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dgeqrf(JNIEnv *env, jclass this, jint m, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray tau, jint tauIdx, jdoubleArray work, jint workIdx, jint lwork)
{
  extern void dgeqrf_(jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, int *);
  
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *tauPtrBase = 0, *tauPtr = 0;
  if (tau) {
    if((*env)->IsSameObject(env, tau, a) == JNI_TRUE)
      tauPtrBase = aPtrBase;
    else
      tauPtrBase = (*env)->GetDoubleArrayElements(env, tau, NULL);
    tauPtr = tauPtrBase + tauIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, tau) == JNI_TRUE)
      workPtrBase = tauPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dgeqrf_(&m, &n, aPtr, &lda, tauPtr, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == tauPtrBase)
      tauPtrBase = 0;
    workPtrBase = 0;
  }
  if(tauPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, tau, tauPtrBase, 0);
    if (tauPtrBase == aPtrBase)
      aPtrBase = 0;
    tauPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sgeqrf(JNIEnv *env, jclass this, jint m, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray tau, jint tauIdx, jfloatArray work, jint workIdx, jint lwork)
{
  extern void sgeqrf_(jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, int *);
  
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *tauPtrBase = 0, *tauPtr = 0;
  if (tau) {
    if((*env)->IsSameObject(env, tau, a) == JNI_TRUE)
      tauPtrBase = aPtrBase;
    else
      tauPtrBase = (*env)->GetFloatArrayElements(env, tau, NULL);
    tauPtr = tauPtrBase + tauIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, tau) == JNI_TRUE)
      workPtrBase = tauPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  sgeqrf_(&m, &n, aPtr, &lda, tauPtr, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == tauPtrBase)
      tauPtrBase = 0;
    workPtrBase = 0;
  }
  if(tauPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, tau, tauPtrBase, 0);
    if (tauPtrBase == aPtrBase)
      aPtrBase = 0;
    tauPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dormqr(JNIEnv *env, jclass this, jchar side, jchar trans, jint m, jint n, jint k, jdoubleArray a, jint aIdx, jint lda, jdoubleArray tau, jint tauIdx, jdoubleArray c, jint cIdx, jint ldc, jdoubleArray work, jint workIdx, jint lwork)
{
  extern void dormqr_(char *, char *, jint *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, int *);
  
  char sideChr = (char) side;
  char transChr = (char) trans;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *tauPtrBase = 0, *tauPtr = 0;
  if (tau) {
    if((*env)->IsSameObject(env, tau, a) == JNI_TRUE)
      tauPtrBase = aPtrBase;
    else
      tauPtrBase = (*env)->GetDoubleArrayElements(env, tau, NULL);
    tauPtr = tauPtrBase + tauIdx;
  }
  jdouble *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, c, tau) == JNI_TRUE)
      cPtrBase = tauPtrBase;
    else
      cPtrBase = (*env)->GetDoubleArrayElements(env, c, NULL);
    cPtr = cPtrBase + cIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, tau) == JNI_TRUE)
      workPtrBase = tauPtrBase;
    else
      if((*env)->IsSameObject(env, work, c) == JNI_TRUE)
      workPtrBase = cPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  dormqr_(&sideChr, &transChr, &m, &n, &k, aPtr, &lda, tauPtr, cPtr, &ldc, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == tauPtrBase)
      tauPtrBase = 0;
    if (workPtrBase == cPtrBase)
      cPtrBase = 0;
    workPtrBase = 0;
  }
  if(cPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
    if (cPtrBase == aPtrBase)
      aPtrBase = 0;
    if (cPtrBase == tauPtrBase)
      tauPtrBase = 0;
    cPtrBase = 0;
  }
  if(tauPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, tau, tauPtrBase, JNI_ABORT);
    if (tauPtrBase == aPtrBase)
      aPtrBase = 0;
    tauPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_sormqr(JNIEnv *env, jclass this, jchar side, jchar trans, jint m, jint n, jint k, jfloatArray a, jint aIdx, jint lda, jfloatArray tau, jint tauIdx, jfloatArray c, jint cIdx, jint ldc, jfloatArray work, jint workIdx, jint lwork)
{
  extern void sormqr_(char *, char *, jint *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, int *);
  
  char sideChr = (char) side;
  char transChr = (char) trans;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *tauPtrBase = 0, *tauPtr = 0;
  if (tau) {
    if((*env)->IsSameObject(env, tau, a) == JNI_TRUE)
      tauPtrBase = aPtrBase;
    else
      tauPtrBase = (*env)->GetFloatArrayElements(env, tau, NULL);
    tauPtr = tauPtrBase + tauIdx;
  }
  jfloat *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, c, tau) == JNI_TRUE)
      cPtrBase = tauPtrBase;
    else
      cPtrBase = (*env)->GetFloatArrayElements(env, c, NULL);
    cPtr = cPtrBase + cIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, tau) == JNI_TRUE)
      workPtrBase = tauPtrBase;
    else
      if((*env)->IsSameObject(env, work, c) == JNI_TRUE)
      workPtrBase = cPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  int info;

  savedEnv = env;
  sormqr_(&sideChr, &transChr, &m, &n, &k, aPtr, &lda, tauPtr, cPtr, &ldc, workPtr, &lwork, &info);
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == tauPtrBase)
      tauPtrBase = 0;
    if (workPtrBase == cPtrBase)
      cPtrBase = 0;
    workPtrBase = 0;
  }
  if(cPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
    if (cPtrBase == aPtrBase)
      aPtrBase = 0;
    if (cPtrBase == tauPtrBase)
      tauPtrBase = 0;
    cPtrBase = 0;
  }
  if(tauPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, tau, tauPtrBase, JNI_ABORT);
    if (tauPtrBase == aPtrBase)
      aPtrBase = 0;
    tauPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, JNI_ABORT);
    aPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_dsygvx(JNIEnv *env, jclass this, jint itype, jchar jobz, jchar range, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jintArray m, jint mIdx, jdoubleArray w, jint wIdx, jdoubleArray z, jint zIdx, jint ldz, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jintArray ifail, jint ifailIdx)
{
  extern void dsygvx_(jint *, char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetDoubleArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jint *mPtrBase = 0, *mPtr = 0;
  if (m) {
    if((*env)->IsSameObject(env, m, iwork) == JNI_TRUE)
      mPtrBase = iworkPtrBase;
    else
      mPtrBase = (*env)->GetIntArrayElements(env, m, NULL);
    mPtr = mPtrBase + mIdx;
  }
  jdouble *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, w, b) == JNI_TRUE)
      wPtrBase = bPtrBase;
    else
      wPtrBase = (*env)->GetDoubleArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jdouble *zPtrBase = 0, *zPtr = 0;
  if (z) {
    if((*env)->IsSameObject(env, z, a) == JNI_TRUE)
      zPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, z, b) == JNI_TRUE)
      zPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, z, w) == JNI_TRUE)
      zPtrBase = wPtrBase;
    else
      zPtrBase = (*env)->GetDoubleArrayElements(env, z, NULL);
    zPtr = zPtrBase + zIdx;
  }
  jdouble *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, z) == JNI_TRUE)
      workPtrBase = zPtrBase;
    else
      workPtrBase = (*env)->GetDoubleArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *ifailPtrBase = 0, *ifailPtr = 0;
  if (ifail) {
    if((*env)->IsSameObject(env, ifail, iwork) == JNI_TRUE)
      ifailPtrBase = iworkPtrBase;
    else
      if((*env)->IsSameObject(env, ifail, m) == JNI_TRUE)
      ifailPtrBase = mPtrBase;
    else
      ifailPtrBase = (*env)->GetIntArrayElements(env, ifail, NULL);
    ifailPtr = ifailPtrBase + ifailIdx;
  }
  int info;

  savedEnv = env;
  dsygvx_(&itype, &jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, bPtr, &ldb, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  if(ifailPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ifail, ifailPtrBase, 0);
    if (ifailPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    if (ifailPtrBase == mPtrBase)
      mPtrBase = 0;
    ifailPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == zPtrBase)
      zPtrBase = 0;
    workPtrBase = 0;
  }
  if(zPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, z, zPtrBase, 0);
    if (zPtrBase == aPtrBase)
      aPtrBase = 0;
    if (zPtrBase == bPtrBase)
      bPtrBase = 0;
    if (zPtrBase == wPtrBase)
      wPtrBase = 0;
    zPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    if (wPtrBase == bPtrBase)
      bPtrBase = 0;
    wPtrBase = 0;
  }
  if(mPtrBase) {
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
    if (mPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    mPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, JNI_ABORT);
    iworkPtrBase = 0;
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_NativeBlas_ssygvx(JNIEnv *env, jclass this, jint itype, jchar jobz, jchar range, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jintArray m, jint mIdx, jfloatArray w, jint wIdx, jfloatArray z, jint zIdx, jint ldz, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jintArray ifail, jint ifailIdx)
{
  extern void ssygvx_(jint *, char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jint *, jint *, int *);
  
  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  jint *iworkPtrBase = 0, *iworkPtr = 0;
  if (iwork) {
    iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *bPtrBase = 0, *bPtr = 0;
  if (b) {
    if((*env)->IsSameObject(env, b, a) == JNI_TRUE)
      bPtrBase = aPtrBase;
    else
      bPtrBase = (*env)->GetFloatArrayElements(env, b, NULL);
    bPtr = bPtrBase + bIdx;
  }
  jint *mPtrBase = 0, *mPtr = 0;
  if (m) {
    if((*env)->IsSameObject(env, m, iwork) == JNI_TRUE)
      mPtrBase = iworkPtrBase;
    else
      mPtrBase = (*env)->GetIntArrayElements(env, m, NULL);
    mPtr = mPtrBase + mIdx;
  }
  jfloat *wPtrBase = 0, *wPtr = 0;
  if (w) {
    if((*env)->IsSameObject(env, w, a) == JNI_TRUE)
      wPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, w, b) == JNI_TRUE)
      wPtrBase = bPtrBase;
    else
      wPtrBase = (*env)->GetFloatArrayElements(env, w, NULL);
    wPtr = wPtrBase + wIdx;
  }
  jfloat *zPtrBase = 0, *zPtr = 0;
  if (z) {
    if((*env)->IsSameObject(env, z, a) == JNI_TRUE)
      zPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, z, b) == JNI_TRUE)
      zPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, z, w) == JNI_TRUE)
      zPtrBase = wPtrBase;
    else
      zPtrBase = (*env)->GetFloatArrayElements(env, z, NULL);
    zPtr = zPtrBase + zIdx;
  }
  jfloat *workPtrBase = 0, *workPtr = 0;
  if (work) {
    if((*env)->IsSameObject(env, work, a) == JNI_TRUE)
      workPtrBase = aPtrBase;
    else
      if((*env)->IsSameObject(env, work, b) == JNI_TRUE)
      workPtrBase = bPtrBase;
    else
      if((*env)->IsSameObject(env, work, w) == JNI_TRUE)
      workPtrBase = wPtrBase;
    else
      if((*env)->IsSameObject(env, work, z) == JNI_TRUE)
      workPtrBase = zPtrBase;
    else
      workPtrBase = (*env)->GetFloatArrayElements(env, work, NULL);
    workPtr = workPtrBase + workIdx;
  }
  jint *ifailPtrBase = 0, *ifailPtr = 0;
  if (ifail) {
    if((*env)->IsSameObject(env, ifail, iwork) == JNI_TRUE)
      ifailPtrBase = iworkPtrBase;
    else
      if((*env)->IsSameObject(env, ifail, m) == JNI_TRUE)
      ifailPtrBase = mPtrBase;
    else
      ifailPtrBase = (*env)->GetIntArrayElements(env, ifail, NULL);
    ifailPtr = ifailPtrBase + ifailIdx;
  }
  int info;

  savedEnv = env;
  ssygvx_(&itype, &jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, bPtr, &ldb, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  if(ifailPtrBase) {
    (*env)->ReleaseIntArrayElements(env, ifail, ifailPtrBase, 0);
    if (ifailPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    if (ifailPtrBase == mPtrBase)
      mPtrBase = 0;
    ifailPtrBase = 0;
  }
  if(workPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
    if (workPtrBase == aPtrBase)
      aPtrBase = 0;
    if (workPtrBase == bPtrBase)
      bPtrBase = 0;
    if (workPtrBase == wPtrBase)
      wPtrBase = 0;
    if (workPtrBase == zPtrBase)
      zPtrBase = 0;
    workPtrBase = 0;
  }
  if(zPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, z, zPtrBase, 0);
    if (zPtrBase == aPtrBase)
      aPtrBase = 0;
    if (zPtrBase == bPtrBase)
      bPtrBase = 0;
    if (zPtrBase == wPtrBase)
      wPtrBase = 0;
    zPtrBase = 0;
  }
  if(wPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
    if (wPtrBase == aPtrBase)
      aPtrBase = 0;
    if (wPtrBase == bPtrBase)
      bPtrBase = 0;
    wPtrBase = 0;
  }
  if(mPtrBase) {
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
    if (mPtrBase == iworkPtrBase)
      iworkPtrBase = 0;
    mPtrBase = 0;
  }
  if(bPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
    if (bPtrBase == aPtrBase)
      aPtrBase = 0;
    bPtrBase = 0;
  }
  if(aPtrBase) {
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
    aPtrBase = 0;
  }
  if(iworkPtrBase) {
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, JNI_ABORT);
    iworkPtrBase = 0;
  }

  return info;
}

