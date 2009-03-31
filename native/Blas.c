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

#include "org_jblas_la_Blas.h"

#define CORE_PACKAGE "org/jblas/core/"

/*
 * For convenience, we define some typedefs here which have the
 * same name as certain Java types, but their implementation differs (of course)
 */
typedef struct { float real, imag; } ComplexFloat;
typedef struct { double real, imag; } ComplexDouble;
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
}

static jobject createComplexFloat(JNIEnv *env, ComplexFloat *fc)
{
  return createObject(env, CORE_PACKAGE "ComplexFloat", "(FF)V", fc->real, fc->imag);
}

static jobject createComplexDouble(JNIEnv *env, ComplexDouble *dc)
{
  return createObject(env, CORE_PACKAGE "ComplexDouble", "(DD)V", dc->real, dc->imag);
}

static void getComplexFloat(JNIEnv *env, jobject fc, ComplexFloat *result)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexFloat");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "F");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "F");

  result->real = (*env)->GetFloatField(env, fc, reField);
  result->imag = (*env)->GetFloatField(env, fc, imField);
}

static void getComplexDouble(JNIEnv *env, jobject dc, ComplexDouble *result)
{
  jclass klass = (*env)->FindClass(env, CORE_PACKAGE "ComplexDouble");
  jfieldID reField = (*env)->GetFieldID(env, klass, "r", "D");
  jfieldID imField = (*env)->GetFieldID(env, klass, "i", "D");

  result->real = (*env)->GetDoubleField(env, dc, reField);
  result->imag = (*env)->GetDoubleField(env, dc, imField);
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
 "CAXPY",  "CCOPY",  "CDOTC",  "CDOTU",  "CGBMV",  "CGEMM",  "CGEMV",  "CGERC",  "CGERU",  "CHBMV",  "CHEMM",  "CHEMV",  "CHER",  "CHER2",  "CHER2K",  "CHERK",  "CHPMV",  "CHPR",  "CHPR2",  "CROTG",  "CSCAL",  "CSROT",  "CSSCAL",  "CSWAP",  "CSYMM",  "CSYR2K",  "CSYRK",  "CTBMV",  "CTBSV",  "CTPMV",  "CTPSV",  "CTRMM",  "CTRMV",  "CTRSM",  "CTRSV",  "DASUM",  "DAXPY",  "DCABS1",  "DCOPY",  "DDOT",  "DGBMV",  "DGEEV",  "DGEMM",  "DGEMV",  "DGER",  "DGESV",  "DNRM2",  "DPOSV",  "DROT",  "DROTG",  "DROTM",  "DROTMG",  "DSBMV",  "DSCAL",  "DSDOT",  "DSPMV",  "DSPR",  "DSPR2",  "DSWAP",  "DSYEV",  "DSYEVD",  "DSYEVR",  "DSYEVX",  "DSYMM",  "DSYMV",  "DSYR",  "DSYR2",  "DSYR2K",  "DSYRK",  "DSYSV",  "DTBMV",  "DTBSV",  "DTPMV",  "DTPSV",  "DTRMM",  "DTRMV",  "DTRSM",  "DTRSV",  "DZASUM",  "DZNRM2",  "ICAMAX",  "IDAMAX",  "ISAMAX",  "IZAMAX",  "LSAME",  "SASUM",  "SAXPY",  "SCASUM",  "SCNRM2",  "SCOPY",  "SDOT",  "SDSDOT",  "SGBMV",  "SGEEV",  "SGEMM",  "SGEMV",  "SGER",  "SGESV",  "SNRM2",  "SPOSV",  "SROT",  "SROTG",  "SROTM",  "SROTMG",  "SSBMV",  "SSCAL",  "SSPMV",  "SSPR",  "SSPR2",  "SSWAP",  "SSYEV",  "SSYEVD",  "SSYEVR",  "SSYEVX",  "SSYMM",  "SSYMV",  "SSYR",  "SSYR2",  "SSYR2K",  "SSYRK",  "SSYSV",  "STBMV",  "STBSV",  "STPMV",  "STPSV",  "STRMM",  "STRMV",  "STRSM",  "STRSV",  "ZAXPY",  "ZCOPY",  "ZDOTC",  "ZDOTU",  "ZDROT",  "ZDSCAL",  "ZGBMV",  "ZGEMM",  "ZGEMV",  "ZGERC",  "ZGERU",  "ZHBMV",  "ZHEMM",  "ZHEMV",  "ZHER",  "ZHER2",  "ZHER2K",  "ZHERK",  "ZHPMV",  "ZHPR",  "ZHPR2",  "ZROTG",  "ZSCAL",  "ZSWAP",  "ZSYMM",  "ZSYR2K",  "ZSYRK",  "ZTBMV",  "ZTBSV",  "ZTPMV",  "ZTPSV",  "ZTRMM",  "ZTRMV",  "ZTRSM",  "ZTRSV", 	0
};

static char *routine_arguments[][21] = {
   { "N", "CA", "CX", "INCX", "CY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "TRANS", "M", "N", "KL", "KU", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "UPLO", "N", "K", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "SIDE", "UPLO", "M", "N", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "A", "LDA" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "ALPHA", "AP", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "AP" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "AP" }, 
   { "CA", "CB", "C", "S" }, 
   { "N", "CA", "CX", "INCX" }, 
   { "N", "CX", "INCX", "CY", "INCY", "C", "S" }, 
   { "N", "SA", "CX", "INCX" }, 
   { "N", "CX", "INCX", "CY", "INCY" }, 
   { "SIDE", "UPLO", "M", "N", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "N", "DX", "INCX" }, 
   { "N", "DA", "DX", "INCX", "DY", "INCY" }, 
   { "Z" }, 
   { "N", "DX", "INCX", "DY", "INCY" }, 
   { "N", "DX", "INCX", "DY", "INCY" }, 
   { "TRANS", "M", "N", "KL", "KU", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "JOBVL", "JOBVR", "N", "A", "LDA", "WR", "WI", "VL", "LDVL", "VR", "LDVR", "WORK", "LWORK", "INFO" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "INFO" }, 
   { "N", "X", "INCX" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "B", "LDB", "INFO" }, 
   { "N", "DX", "INCX", "DY", "INCY", "C", "S" }, 
   { "DA", "DB", "C", "S" }, 
   { "N", "DX", "INCX", "DY", "INCY", "DPARAM" }, 
   { "DD1", "DD2", "DX1", "DY1", "DPARAM" }, 
   { "UPLO", "N", "K", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "N", "DA", "DX", "INCX" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "UPLO", "N", "ALPHA", "AP", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "AP" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "AP" }, 
   { "N", "DX", "INCX", "DY", "INCY" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "INFO" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "ISUPPZ", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "WORK", "LWORK", "IWORK", "IFAIL", "INFO" }, 
   { "SIDE", "UPLO", "M", "N", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "A", "LDA" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "WORK", "LWORK", "INFO" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "N", "ZX", "INCX" }, 
   { "N", "X", "INCX" }, 
   { "N", "CX", "INCX" }, 
   { "N", "DX", "INCX" }, 
   { "N", "SX", "INCX" }, 
   { "N", "ZX", "INCX" }, 
   { "CA", "CB" }, 
   { "N", "SX", "INCX" }, 
   { "N", "SA", "SX", "INCX", "SY", "INCY" }, 
   { "N", "CX", "INCX" }, 
   { "N", "X", "INCX" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "N", "SB", "SX", "INCX", "SY", "INCY" }, 
   { "TRANS", "M", "N", "KL", "KU", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "JOBVL", "JOBVR", "N", "A", "LDA", "WR", "WI", "VL", "LDVL", "VR", "LDVR", "WORK", "LWORK", "INFO" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "INFO" }, 
   { "N", "X", "INCX" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "B", "LDB", "INFO" }, 
   { "N", "SX", "INCX", "SY", "INCY", "C", "S" }, 
   { "SA", "SB", "C", "S" }, 
   { "N", "SX", "INCX", "SY", "INCY", "SPARAM" }, 
   { "SD1", "SD2", "SX1", "SY1", "SPARAM" }, 
   { "UPLO", "N", "K", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "N", "SA", "SX", "INCX" }, 
   { "UPLO", "N", "ALPHA", "AP", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "AP" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "AP" }, 
   { "N", "SX", "INCX", "SY", "INCY" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "INFO" }, 
   { "JOBZ", "UPLO", "N", "A", "LDA", "W", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "ISUPPZ", "WORK", "LWORK", "IWORK", "LIWORK", "INFO" }, 
   { "JOBZ", "RANGE", "UPLO", "N", "A", "LDA", "VL", "VU", "IL", "IU", "ABSTOL", "M", "W", "Z", "LDZ", "WORK", "LWORK", "IWORK", "IFAIL", "INFO" }, 
   { "SIDE", "UPLO", "M", "N", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "A", "LDA" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "NRHS", "A", "LDA", "IPIV", "B", "LDB", "WORK", "LWORK", "INFO" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "N", "ZA", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "N", "CX", "INCX", "CY", "INCY", "C", "S" }, 
   { "N", "DA", "ZX", "INCX" }, 
   { "TRANS", "M", "N", "KL", "KU", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "TRANSA", "TRANSB", "M", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "TRANS", "M", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "M", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "UPLO", "N", "K", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "SIDE", "UPLO", "M", "N", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "ALPHA", "A", "LDA", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "A", "LDA" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "A", "LDA" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "BETA", "C", "LDC" }, 
   { "UPLO", "N", "ALPHA", "AP", "X", "INCX", "BETA", "Y", "INCY" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "AP" }, 
   { "UPLO", "N", "ALPHA", "X", "INCX", "Y", "INCY", "AP" }, 
   { "CA", "CB", "C", "S" }, 
   { "N", "ZA", "ZX", "INCX" }, 
   { "N", "ZX", "INCX", "ZY", "INCY" }, 
   { "SIDE", "UPLO", "M", "N", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "B", "LDB", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "N", "K", "ALPHA", "A", "LDA", "BETA", "C", "LDC" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "K", "A", "LDA", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "UPLO", "TRANS", "DIAG", "N", "AP", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
   { "SIDE", "UPLO", "TRANSA", "DIAG", "M", "N", "ALPHA", "A", "LDA", "B", "LDB" }, 
   { "UPLO", "TRANS", "DIAG", "N", "A", "LDA", "X", "INCX" }, 
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


JNIEXPORT void JNICALL Java_org_jblas_la_Blas_caxpy(JNIEnv *env, jclass this, jint n, jobject ca, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern void caxpy_(jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *);
  
  ComplexFloat caCplx;
  getComplexFloat(env, ca, &caCplx);
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
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ccopy(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
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
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

}

JNIEXPORT jobject JNICALL Java_org_jblas_la_Blas_cdotc(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern void cdotc_(ComplexFloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  ComplexFloat retval;
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
  cdotc_(&retval, &n, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

  return createComplexFloat(env, &retval);
}

JNIEXPORT jobject JNICALL Java_org_jblas_la_Blas_cdotu(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
{
  extern void cdotu_(ComplexFloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  ComplexFloat retval;
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
  cdotu_(&retval, &n, cxPtr, &incx, cyPtr, &incy);
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

  return createComplexFloat(env, &retval);
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jobject beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void cgbmv_(char *, jint *, jint *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  cgbmv_(&transChr, &m, &n, &kl, &ku, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jobject beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void cgemm_(char *, char *, jint *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char transaChr = (char) transa;
  char transbChr = (char) transb;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jobject beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void cgemv_(char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cgerc(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void cgerc_(jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cgeru(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void cgeru_(jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_chbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jobject beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void chbmv_(char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  chbmv_(&uploChr, &n, &k, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_chemm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jobject beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void chemm_(char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  chemm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_chemv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jobject beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void chemv_(char *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  chemv_(&uploChr, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cher2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void cher2_(char *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  cher2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cher2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void cher2k_(char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  cher2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cher(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray a, jint aIdx, jint lda)
{
  extern void cher_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }

  savedEnv = env;
  cher_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cherk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void cherk_(char *, char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jfloat *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      cPtrBase = (*env)->GetFloatArrayElements(env, c, NULL);
    cPtr = cPtrBase + 2*cIdx;
  }

  savedEnv = env;
  cherk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_chpmv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jfloatArray ap, jint apIdx, jfloatArray x, jint xIdx, jint incx, jobject beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void chpmv_(char *, jint *, ComplexFloat *, jfloat *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, ap) == JNI_TRUE)
      yPtrBase = apPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }

  savedEnv = env;
  chpmv_(&uploChr, &n, &alphaCplx, apPtr, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != apPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_chpr2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray ap, jint apIdx)
{
  extern void chpr2_(char *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, jfloat *);
  
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, ap, y) == JNI_TRUE)
      apPtrBase = yPtrBase;
    else
      apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }

  savedEnv = env;
  chpr2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase && apPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_chpr(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray ap, jint apIdx)
{
  extern void chpr_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *);
  
  char uploChr = (char) uplo;
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }

  savedEnv = env;
  chpr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_crotg(JNIEnv *env, jclass this, jobject ca, jobject cb, jfloat c, jobject s)
{
  extern void crotg_(ComplexFloat *, ComplexFloat *, jfloat *, ComplexFloat *);
  
  ComplexFloat caCplx;
  getComplexFloat(env, ca, &caCplx);
  ComplexFloat cbCplx;
  getComplexFloat(env, cb, &cbCplx);
  ComplexFloat sCplx;
  getComplexFloat(env, s, &sCplx);

  savedEnv = env;
  crotg_(&caCplx, &cbCplx, &c, &sCplx);

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cscal(JNIEnv *env, jclass this, jint n, jobject ca, jfloatArray cx, jint cxIdx, jint incx)
{
  extern void cscal_(jint *, ComplexFloat *, jfloat *, jint *);
  
  ComplexFloat caCplx;
  getComplexFloat(env, ca, &caCplx);
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }

  savedEnv = env;
  cscal_(&n, &caCplx, cxPtr, &incx);
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_csrot(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy, jfloat c, jfloat s)
{
  extern void csrot_(jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *);
  
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
  csrot_(&n, cxPtr, &incx, cyPtr, &incy, &c, &s);
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_csscal(JNIEnv *env, jclass this, jint n, jfloat sa, jfloatArray cx, jint cxIdx, jint incx)
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
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_cswap(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx, jfloatArray cy, jint cyIdx, jint incy)
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
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_csymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jobject beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void csymm_(char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  csymm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_csyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jobject beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void csyr2k_(char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
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
  csyr2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_csyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jfloatArray a, jint aIdx, jint lda, jobject beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void csyrk_(char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, ComplexFloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  jfloat *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      cPtrBase = (*env)->GetFloatArrayElements(env, c, NULL);
    cPtr = cPtrBase + 2*cIdx;
  }

  savedEnv = env;
  csyrk_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void ctbmv_(char *, char *, char *, jint *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ctbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void ctbsv_(char *, char *, char *, jint *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ctbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray ap, jint apIdx, jfloatArray x, jint xIdx, jint incx)
{
  extern void ctpmv_(char *, char *, char *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  ctpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray ap, jint apIdx, jfloatArray x, jint xIdx, jint incx)
{
  extern void ctpsv_(char *, char *, char *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  ctpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctrmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb)
{
  extern void ctrmm_(char *, char *, char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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

  savedEnv = env;
  ctrmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctrmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void ctrmv_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ctrmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctrsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb)
{
  extern void ctrsm_(char *, char *, char *, char *, jint *, jint *, ComplexFloat *, jfloat *, jint *, jfloat *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
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

  savedEnv = env;
  ctrsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ctrsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void ctrsv_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ctrsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_dasum(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_daxpy(JNIEnv *env, jclass this, jint n, jdouble da, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
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
  if(dyPtrBase && dyPtrBase != dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
  }
  if(dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
  }

}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_dcabs1(JNIEnv *env, jclass this, jobject z)
{
  extern jdouble dcabs1_(ComplexDouble *);
  
  ComplexDouble zCplx;
  getComplexDouble(env, z, &zCplx);

  savedEnv = env;
  jdouble retval = dcabs1_(&zCplx);

  return retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dcopy(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
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
  if(dyPtrBase && dyPtrBase != dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
  }
  if(dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
  }

}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_ddot(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
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
  if(dyPtrBase && dyPtrBase != dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
  }
  if(dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
  }

  return retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jdouble beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void dgbmv_(char *, jint *, jint *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
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
  dgbmv_(&transChr, &m, &n, &kl, &ku, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
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
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jdouble beta, jdoubleArray y, jint yIdx, jint incy)
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
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dger(JNIEnv *env, jclass this, jint m, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
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
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_dnrm2(JNIEnv *env, jclass this, jint n, jdoubleArray x, jint xIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_drot(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy, jdouble c, jdouble s)
{
  extern void drot_(jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *);
  
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
  drot_(&n, dxPtr, &incx, dyPtr, &incy, &c, &s);
  if(dyPtrBase && dyPtrBase != dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
  }
  if(dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_drotg(JNIEnv *env, jclass this, jdouble da, jdouble db, jdouble c, jdouble s)
{
  extern void drotg_(jdouble *, jdouble *, jdouble *, jdouble *);
  

  savedEnv = env;
  drotg_(&da, &db, &c, &s);

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_drotm(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy, jdoubleArray dparam, jint dparamIdx)
{
  extern void drotm_(jint *, jdouble *, jint *, jdouble *, jint *, jdouble *);
  
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
  jdouble *dparamPtrBase = 0, *dparamPtr = 0;
  if (dparam) {
    if((*env)->IsSameObject(env, dparam, dx) == JNI_TRUE)
      dparamPtrBase = dxPtrBase;
    else
      if((*env)->IsSameObject(env, dparam, dy) == JNI_TRUE)
      dparamPtrBase = dyPtrBase;
    else
      dparamPtrBase = (*env)->GetDoubleArrayElements(env, dparam, NULL);
    dparamPtr = dparamPtrBase + dparamIdx;
  }

  savedEnv = env;
  drotm_(&n, dxPtr, &incx, dyPtr, &incy, dparamPtr);
  if(dparamPtrBase && dparamPtrBase != dxPtrBase && dparamPtrBase != dyPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dparam, dparamPtrBase, 0);
  }
  if(dyPtrBase && dyPtrBase != dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
  }
  if(dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_drotmg(JNIEnv *env, jclass this, jdoubleArray dd1, jint dd1Idx, jdoubleArray dd2, jint dd2Idx, jdoubleArray dx1, jint dx1Idx, jdouble dy1, jdoubleArray dparam, jint dparamIdx)
{
  extern void drotmg_(jdouble *, jdouble *, jdouble *, jdouble *, jdouble *);
  
  jdouble *dd1PtrBase = 0, *dd1Ptr = 0;
  if (dd1) {
    dd1PtrBase = (*env)->GetDoubleArrayElements(env, dd1, NULL);
    dd1Ptr = dd1PtrBase + dd1Idx;
  }
  jdouble *dd2PtrBase = 0, *dd2Ptr = 0;
  if (dd2) {
    if((*env)->IsSameObject(env, dd2, dd1) == JNI_TRUE)
      dd2PtrBase = dd1PtrBase;
    else
      dd2PtrBase = (*env)->GetDoubleArrayElements(env, dd2, NULL);
    dd2Ptr = dd2PtrBase + dd2Idx;
  }
  jdouble *dx1PtrBase = 0, *dx1Ptr = 0;
  if (dx1) {
    if((*env)->IsSameObject(env, dx1, dd1) == JNI_TRUE)
      dx1PtrBase = dd1PtrBase;
    else
      if((*env)->IsSameObject(env, dx1, dd2) == JNI_TRUE)
      dx1PtrBase = dd2PtrBase;
    else
      dx1PtrBase = (*env)->GetDoubleArrayElements(env, dx1, NULL);
    dx1Ptr = dx1PtrBase + dx1Idx;
  }
  jdouble *dparamPtrBase = 0, *dparamPtr = 0;
  if (dparam) {
    if((*env)->IsSameObject(env, dparam, dd1) == JNI_TRUE)
      dparamPtrBase = dd1PtrBase;
    else
      if((*env)->IsSameObject(env, dparam, dd2) == JNI_TRUE)
      dparamPtrBase = dd2PtrBase;
    else
      if((*env)->IsSameObject(env, dparam, dx1) == JNI_TRUE)
      dparamPtrBase = dx1PtrBase;
    else
      dparamPtrBase = (*env)->GetDoubleArrayElements(env, dparam, NULL);
    dparamPtr = dparamPtrBase + dparamIdx;
  }

  savedEnv = env;
  drotmg_(dd1Ptr, dd2Ptr, dx1Ptr, &dy1, dparamPtr);
  if(dparamPtrBase && dparamPtrBase != dd1PtrBase && dparamPtrBase != dd2PtrBase && dparamPtrBase != dx1PtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dparam, dparamPtrBase, 0);
  }
  if(dx1PtrBase && dx1PtrBase != dd1PtrBase && dx1PtrBase != dd2PtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx1, dx1PtrBase, 0);
  }
  if(dd2PtrBase && dd2PtrBase != dd1PtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dd2, dd2PtrBase, 0);
  }
  if(dd1PtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dd1, dd1PtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jdouble beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void dsbmv_(char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
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
  dsbmv_(&uploChr, &n, &k, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dscal(JNIEnv *env, jclass this, jint n, jdouble da, jdoubleArray dx, jint dxIdx, jint incx)
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
  }

}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_dsdot(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern jdouble dsdot_(jint *, jfloat *, jint *, jfloat *, jint *);
  
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
  jdouble retval = dsdot_(&n, sxPtr, &incx, syPtr, &incy);
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

  return retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dspmv(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray ap, jint apIdx, jdoubleArray x, jint xIdx, jint incx, jdouble beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void dspmv_(char *, jint *, jdouble *, jdouble *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, ap) == JNI_TRUE)
      yPtrBase = apPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + yIdx;
  }

  savedEnv = env;
  dspmv_(&uploChr, &n, &alpha, apPtr, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != apPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dspr2(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray ap, jint apIdx)
{
  extern void dspr2_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *);
  
  char uploChr = (char) uplo;
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
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, ap, y) == JNI_TRUE)
      apPtrBase = yPtrBase;
    else
      apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }

  savedEnv = env;
  dspr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase && apPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dspr(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray ap, jint apIdx)
{
  extern void dspr_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *);
  
  char uploChr = (char) uplo;
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }

  savedEnv = env;
  dspr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dswap(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx, jdoubleArray dy, jint dyIdx, jint incy)
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
  if(dyPtrBase && dyPtrBase != dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dy, dyPtrBase, 0);
  }
  if(dxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, dx, dxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void dsymm_(char *, char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char sideChr = (char) side;
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
  dsymm_(&sideChr, &uploChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsymv(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jdouble beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void dsymv_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
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
  dsymv_(&uploChr, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsyr2(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void dsyr2_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
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
  dsyr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void dsyr2k_(char *, char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
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
  dsyr2k_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsyr(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray a, jint aIdx, jint lda)
{
  extern void dsyr_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }

  savedEnv = env;
  dsyr_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dsyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void dsyrk_(char *, char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jdouble *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      cPtrBase = (*env)->GetDoubleArrayElements(env, c, NULL);
    cPtr = cPtrBase + cIdx;
  }

  savedEnv = env;
  dsyrk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void dtbmv_(char *, char *, char *, jint *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  dtbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void dtbsv_(char *, char *, char *, jint *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  dtbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray ap, jint apIdx, jdoubleArray x, jint xIdx, jint incx)
{
  extern void dtpmv_(char *, char *, char *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  dtpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray ap, jint apIdx, jdoubleArray x, jint xIdx, jint incx)
{
  extern void dtpsv_(char *, char *, char *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  dtpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtrmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb)
{
  extern void dtrmm_(char *, char *, char *, char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
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

  savedEnv = env;
  dtrmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtrmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void dtrmv_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  dtrmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtrsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb)
{
  extern void dtrsm_(char *, char *, char *, char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
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

  savedEnv = env;
  dtrsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_dtrsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void dtrsv_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  dtrsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_dzasum(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT jdouble JNICALL Java_org_jblas_la_Blas_dznrm2(JNIEnv *env, jclass this, jint n, jdoubleArray x, jint xIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_icamax(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_idamax(JNIEnv *env, jclass this, jint n, jdoubleArray dx, jint dxIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_isamax(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_izamax(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx)
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
  }

  return retval;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_lsame(JNIEnv *env, jclass this, jchar ca, jchar cb)
{
  extern jint lsame_(char *, char *);
  
  char caChr = (char) ca;
  char cbChr = (char) cb;

  savedEnv = env;
  jint retval = lsame_(&caChr, &cbChr);

  return retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_la_Blas_sasum(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx)
{
  extern jdouble sasum_(jint *, jfloat *, jint *);
  
  jfloat *sxPtrBase = 0, *sxPtr = 0;
  if (sx) {
    sxPtrBase = (*env)->GetFloatArrayElements(env, sx, NULL);
    sxPtr = sxPtrBase + sxIdx;
  }

  savedEnv = env;
  jfloat retval = sasum_(&n, sxPtr, &incx);
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

  return (jdouble) retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_saxpy(JNIEnv *env, jclass this, jint n, jfloat sa, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
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
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

}

JNIEXPORT jfloat JNICALL Java_org_jblas_la_Blas_scasum(JNIEnv *env, jclass this, jint n, jfloatArray cx, jint cxIdx, jint incx)
{
  extern jdouble scasum_(jint *, jfloat *, jint *);
  
  jfloat *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetFloatArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }

  savedEnv = env;
  jfloat retval = scasum_(&n, cxPtr, &incx);
  if(cxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, cx, cxPtrBase, 0);
  }

  return (jdouble) retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_la_Blas_scnrm2(JNIEnv *env, jclass this, jint n, jfloatArray x, jint xIdx, jint incx)
{
  extern jdouble scnrm2_(jint *, jfloat *, jint *);
  
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  jfloat retval = scnrm2_(&n, xPtr, &incx);
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

  return (jdouble) retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_scopy(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
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
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

}

JNIEXPORT jfloat JNICALL Java_org_jblas_la_Blas_sdot(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern jdouble sdot_(jint *, jfloat *, jint *, jfloat *, jint *);
  
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
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

  return (jdouble) retval;
}

JNIEXPORT jfloat JNICALL Java_org_jblas_la_Blas_sdsdot(JNIEnv *env, jclass this, jint n, jfloat sb, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
{
  extern jdouble sdsdot_(jint *, jfloat *, jfloat *, jint *, jfloat *, jint *);
  
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
  jfloat retval = sdsdot_(&n, &sb, sxPtr, &incx, syPtr, &incy);
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

  return (jdouble) retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jfloat beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void sgbmv_(char *, jint *, jint *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
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
  sgbmv_(&transChr, &m, &n, &kl, &ku, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
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
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jfloat beta, jfloatArray y, jint yIdx, jint incy)
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
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sger(JNIEnv *env, jclass this, jint m, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
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
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT jfloat JNICALL Java_org_jblas_la_Blas_snrm2(JNIEnv *env, jclass this, jint n, jfloatArray x, jint xIdx, jint incx)
{
  extern jdouble snrm2_(jint *, jfloat *, jint *);
  
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  jfloat retval = snrm2_(&n, xPtr, &incx);
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

  return (jdouble) retval;
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_srot(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy, jfloat c, jfloat s)
{
  extern void srot_(jint *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *);
  
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
  srot_(&n, sxPtr, &incx, syPtr, &incy, &c, &s);
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_srotg(JNIEnv *env, jclass this, jfloat sa, jfloat sb, jfloat c, jfloat s)
{
  extern void srotg_(jfloat *, jfloat *, jfloat *, jfloat *);
  

  savedEnv = env;
  srotg_(&sa, &sb, &c, &s);

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_srotm(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy, jfloatArray sparam, jint sparamIdx)
{
  extern void srotm_(jint *, jfloat *, jint *, jfloat *, jint *, jfloat *);
  
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
  jfloat *sparamPtrBase = 0, *sparamPtr = 0;
  if (sparam) {
    if((*env)->IsSameObject(env, sparam, sx) == JNI_TRUE)
      sparamPtrBase = sxPtrBase;
    else
      if((*env)->IsSameObject(env, sparam, sy) == JNI_TRUE)
      sparamPtrBase = syPtrBase;
    else
      sparamPtrBase = (*env)->GetFloatArrayElements(env, sparam, NULL);
    sparamPtr = sparamPtrBase + sparamIdx;
  }

  savedEnv = env;
  srotm_(&n, sxPtr, &incx, syPtr, &incy, sparamPtr);
  if(sparamPtrBase && sparamPtrBase != sxPtrBase && sparamPtrBase != syPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sparam, sparamPtrBase, 0);
  }
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_srotmg(JNIEnv *env, jclass this, jfloatArray sd1, jint sd1Idx, jfloatArray sd2, jint sd2Idx, jfloatArray sx1, jint sx1Idx, jfloat sy1, jfloatArray sparam, jint sparamIdx)
{
  extern void srotmg_(jfloat *, jfloat *, jfloat *, jfloat *, jfloat *);
  
  jfloat *sd1PtrBase = 0, *sd1Ptr = 0;
  if (sd1) {
    sd1PtrBase = (*env)->GetFloatArrayElements(env, sd1, NULL);
    sd1Ptr = sd1PtrBase + sd1Idx;
  }
  jfloat *sd2PtrBase = 0, *sd2Ptr = 0;
  if (sd2) {
    if((*env)->IsSameObject(env, sd2, sd1) == JNI_TRUE)
      sd2PtrBase = sd1PtrBase;
    else
      sd2PtrBase = (*env)->GetFloatArrayElements(env, sd2, NULL);
    sd2Ptr = sd2PtrBase + sd2Idx;
  }
  jfloat *sx1PtrBase = 0, *sx1Ptr = 0;
  if (sx1) {
    if((*env)->IsSameObject(env, sx1, sd1) == JNI_TRUE)
      sx1PtrBase = sd1PtrBase;
    else
      if((*env)->IsSameObject(env, sx1, sd2) == JNI_TRUE)
      sx1PtrBase = sd2PtrBase;
    else
      sx1PtrBase = (*env)->GetFloatArrayElements(env, sx1, NULL);
    sx1Ptr = sx1PtrBase + sx1Idx;
  }
  jfloat *sparamPtrBase = 0, *sparamPtr = 0;
  if (sparam) {
    if((*env)->IsSameObject(env, sparam, sd1) == JNI_TRUE)
      sparamPtrBase = sd1PtrBase;
    else
      if((*env)->IsSameObject(env, sparam, sd2) == JNI_TRUE)
      sparamPtrBase = sd2PtrBase;
    else
      if((*env)->IsSameObject(env, sparam, sx1) == JNI_TRUE)
      sparamPtrBase = sx1PtrBase;
    else
      sparamPtrBase = (*env)->GetFloatArrayElements(env, sparam, NULL);
    sparamPtr = sparamPtrBase + sparamIdx;
  }

  savedEnv = env;
  srotmg_(sd1Ptr, sd2Ptr, sx1Ptr, &sy1, sparamPtr);
  if(sparamPtrBase && sparamPtrBase != sd1PtrBase && sparamPtrBase != sd2PtrBase && sparamPtrBase != sx1PtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sparam, sparamPtrBase, 0);
  }
  if(sx1PtrBase && sx1PtrBase != sd1PtrBase && sx1PtrBase != sd2PtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx1, sx1PtrBase, 0);
  }
  if(sd2PtrBase && sd2PtrBase != sd1PtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sd2, sd2PtrBase, 0);
  }
  if(sd1PtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sd1, sd1PtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jfloat beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void ssbmv_(char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
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
  ssbmv_(&uploChr, &n, &k, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sscal(JNIEnv *env, jclass this, jint n, jfloat sa, jfloatArray sx, jint sxIdx, jint incx)
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
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sspmv(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray ap, jint apIdx, jfloatArray x, jint xIdx, jint incx, jfloat beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void sspmv_(char *, jint *, jfloat *, jfloat *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jfloat *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, ap) == JNI_TRUE)
      yPtrBase = apPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetFloatArrayElements(env, y, NULL);
    yPtr = yPtrBase + yIdx;
  }

  savedEnv = env;
  sspmv_(&uploChr, &n, &alpha, apPtr, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != apPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sspr2(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray ap, jint apIdx)
{
  extern void sspr2_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *);
  
  char uploChr = (char) uplo;
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
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, ap, y) == JNI_TRUE)
      apPtrBase = yPtrBase;
    else
      apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }

  savedEnv = env;
  sspr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase && apPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sspr(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray ap, jint apIdx)
{
  extern void sspr_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *);
  
  char uploChr = (char) uplo;
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }

  savedEnv = env;
  sspr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_sswap(JNIEnv *env, jclass this, jint n, jfloatArray sx, jint sxIdx, jint incx, jfloatArray sy, jint syIdx, jint incy)
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
  if(syPtrBase && syPtrBase != sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sy, syPtrBase, 0);
  }
  if(sxPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, sx, sxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void ssymm_(char *, char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char sideChr = (char) side;
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
  ssymm_(&sideChr, &uploChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssymv(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx, jfloat beta, jfloatArray y, jint yIdx, jint incy)
{
  extern void ssymv_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
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
  ssymv_(&uploChr, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssyr2(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray y, jint yIdx, jint incy, jfloatArray a, jint aIdx, jint lda)
{
  extern void ssyr2_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
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
  ssyr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void ssyr2k_(char *, char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
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
  ssyr2k_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssyr(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jfloatArray x, jint xIdx, jint incx, jfloatArray a, jint aIdx, jint lda)
{
  extern void ssyr_(char *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }

  savedEnv = env;
  ssyr_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ssyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloat beta, jfloatArray c, jint cIdx, jint ldc)
{
  extern void ssyrk_(char *, char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  jfloat *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetFloatArrayElements(env, a, NULL);
    aPtr = aPtrBase + aIdx;
  }
  jfloat *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      cPtrBase = (*env)->GetFloatArrayElements(env, c, NULL);
    cPtr = cPtrBase + cIdx;
  }

  savedEnv = env;
  ssyrk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, c, cPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_stbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void stbmv_(char *, char *, char *, jint *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  stbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_stbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void stbsv_(char *, char *, char *, jint *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  stbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_stpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray ap, jint apIdx, jfloatArray x, jint xIdx, jint incx)
{
  extern void stpmv_(char *, char *, char *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  stpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_stpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray ap, jint apIdx, jfloatArray x, jint xIdx, jint incx)
{
  extern void stpsv_(char *, char *, char *, jint *, jfloat *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jfloat *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetFloatArrayElements(env, ap, NULL);
    apPtr = apPtrBase + apIdx;
  }
  jfloat *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetFloatArrayElements(env, x, NULL);
    xPtr = xPtrBase + xIdx;
  }

  savedEnv = env;
  stpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_strmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb)
{
  extern void strmm_(char *, char *, char *, char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
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

  savedEnv = env;
  strmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_strmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void strmv_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  strmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_strsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jfloat alpha, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb)
{
  extern void strsm_(char *, char *, char *, char *, jint *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
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

  savedEnv = env;
  strsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_strsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray x, jint xIdx, jint incx)
{
  extern void strsv_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  strsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zaxpy(JNIEnv *env, jclass this, jint n, jobject za, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern void zaxpy_(jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *);
  
  ComplexDouble zaCplx;
  getComplexDouble(env, za, &zaCplx);
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
  if(zyPtrBase && zyPtrBase != zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
  }
  if(zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zcopy(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
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
  if(zyPtrBase && zyPtrBase != zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
  }
  if(zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
  }

}

JNIEXPORT jobject JNICALL Java_org_jblas_la_Blas_zdotc(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern void zdotc_(ComplexDouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  ComplexDouble retval;
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
  zdotc_(&retval, &n, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase && zyPtrBase != zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
  }
  if(zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
  }

  return createComplexDouble(env, &retval);
}

JNIEXPORT jobject JNICALL Java_org_jblas_la_Blas_zdotu(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
{
  extern void zdotu_(ComplexDouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  ComplexDouble retval;
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
  zdotu_(&retval, &n, zxPtr, &incx, zyPtr, &incy);
  if(zyPtrBase && zyPtrBase != zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
  }
  if(zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
  }

  return createComplexDouble(env, &retval);
}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zdrot(JNIEnv *env, jclass this, jint n, jdoubleArray cx, jint cxIdx, jint incx, jdoubleArray cy, jint cyIdx, jint incy, jdouble c, jdouble s)
{
  extern void zdrot_(jint *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *);
  
  jdouble *cxPtrBase = 0, *cxPtr = 0;
  if (cx) {
    cxPtrBase = (*env)->GetDoubleArrayElements(env, cx, NULL);
    cxPtr = cxPtrBase + 2*cxIdx;
  }
  jdouble *cyPtrBase = 0, *cyPtr = 0;
  if (cy) {
    if((*env)->IsSameObject(env, cy, cx) == JNI_TRUE)
      cyPtrBase = cxPtrBase;
    else
      cyPtrBase = (*env)->GetDoubleArrayElements(env, cy, NULL);
    cyPtr = cyPtrBase + 2*cyIdx;
  }

  savedEnv = env;
  zdrot_(&n, cxPtr, &incx, cyPtr, &incy, &c, &s);
  if(cyPtrBase && cyPtrBase != cxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, cy, cyPtrBase, 0);
  }
  if(cxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, cx, cxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zdscal(JNIEnv *env, jclass this, jint n, jdouble da, jdoubleArray zx, jint zxIdx, jint incx)
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
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jobject beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void zgbmv_(char *, jint *, jint *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  zgbmv_(&transChr, &m, &n, &kl, &ku, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jobject beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zgemm_(char *, char *, jint *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char transaChr = (char) transa;
  char transbChr = (char) transb;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jobject beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void zgemv_(char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zgerc(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void zgerc_(jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zgeru(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void zgeru_(jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zhbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jobject beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void zhbmv_(char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  zhbmv_(&uploChr, &n, &k, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zhemm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jobject beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zhemm_(char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  zhemm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zhemv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx, jobject beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void zhemv_(char *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  zhemv_(&uploChr, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != aPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zher2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray a, jint aIdx, jint lda)
{
  extern void zher2_(char *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  zher2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase && aPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zher2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zher2k_(char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  zher2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zher(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray a, jint aIdx, jint lda)
{
  extern void zher_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    if((*env)->IsSameObject(env, a, x) == JNI_TRUE)
      aPtrBase = xPtrBase;
    else
      aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }

  savedEnv = env;
  zher_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
  if(aPtrBase && aPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zherk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jdouble alpha, jdoubleArray a, jint aIdx, jint lda, jdouble beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zherk_(char *, char *, jint *, jint *, jdouble *, jdouble *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  jdouble *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      cPtrBase = (*env)->GetDoubleArrayElements(env, c, NULL);
    cPtr = cPtrBase + 2*cIdx;
  }

  savedEnv = env;
  zherk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zhpmv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jdoubleArray ap, jint apIdx, jdoubleArray x, jint xIdx, jint incx, jobject beta, jdoubleArray y, jint yIdx, jint incy)
{
  extern void zhpmv_(char *, jint *, ComplexDouble *, jdouble *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  jdouble *yPtrBase = 0, *yPtr = 0;
  if (y) {
    if((*env)->IsSameObject(env, y, ap) == JNI_TRUE)
      yPtrBase = apPtrBase;
    else
      if((*env)->IsSameObject(env, y, x) == JNI_TRUE)
      yPtrBase = xPtrBase;
    else
      yPtrBase = (*env)->GetDoubleArrayElements(env, y, NULL);
    yPtr = yPtrBase + 2*yIdx;
  }

  savedEnv = env;
  zhpmv_(&uploChr, &n, &alphaCplx, apPtr, xPtr, &incx, &betaCplx, yPtr, &incy);
  if(yPtrBase && yPtrBase != apPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zhpr2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray y, jint yIdx, jint incy, jdoubleArray ap, jint apIdx)
{
  extern void zhpr2_(char *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, jdouble *);
  
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      if((*env)->IsSameObject(env, ap, y) == JNI_TRUE)
      apPtrBase = yPtrBase;
    else
      apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }

  savedEnv = env;
  zhpr2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase && apPtrBase != yPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }
  if(yPtrBase && yPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, y, yPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zhpr(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jdoubleArray x, jint xIdx, jint incx, jdoubleArray ap, jint apIdx)
{
  extern void zhpr_(char *, jint *, jdouble *, jdouble *, jint *, jdouble *);
  
  char uploChr = (char) uplo;
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    if((*env)->IsSameObject(env, ap, x) == JNI_TRUE)
      apPtrBase = xPtrBase;
    else
      apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }

  savedEnv = env;
  zhpr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
  if(apPtrBase && apPtrBase != xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }
  if(xPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zrotg(JNIEnv *env, jclass this, jobject ca, jobject cb, jdouble c, jobject s)
{
  extern void zrotg_(ComplexDouble *, ComplexDouble *, jdouble *, ComplexDouble *);
  
  ComplexDouble caCplx;
  getComplexDouble(env, ca, &caCplx);
  ComplexDouble cbCplx;
  getComplexDouble(env, cb, &cbCplx);
  ComplexDouble sCplx;
  getComplexDouble(env, s, &sCplx);

  savedEnv = env;
  zrotg_(&caCplx, &cbCplx, &c, &sCplx);

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zscal(JNIEnv *env, jclass this, jint n, jobject za, jdoubleArray zx, jint zxIdx, jint incx)
{
  extern void zscal_(jint *, ComplexDouble *, jdouble *, jint *);
  
  ComplexDouble zaCplx;
  getComplexDouble(env, za, &zaCplx);
  jdouble *zxPtrBase = 0, *zxPtr = 0;
  if (zx) {
    zxPtrBase = (*env)->GetDoubleArrayElements(env, zx, NULL);
    zxPtr = zxPtrBase + 2*zxIdx;
  }

  savedEnv = env;
  zscal_(&n, &zaCplx, zxPtr, &incx);
  if(zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zswap(JNIEnv *env, jclass this, jint n, jdoubleArray zx, jint zxIdx, jint incx, jdoubleArray zy, jint zyIdx, jint incy)
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
  if(zyPtrBase && zyPtrBase != zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zy, zyPtrBase, 0);
  }
  if(zxPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, zx, zxPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zsymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jobject beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zsymm_(char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  zsymm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zsyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb, jobject beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zsyr2k_(char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
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
  zsyr2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase && cPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_zsyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jobject beta, jdoubleArray c, jint cIdx, jint ldc)
{
  extern void zsyrk_(char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, ComplexDouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  jdouble *aPtrBase = 0, *aPtr = 0;
  if (a) {
    aPtrBase = (*env)->GetDoubleArrayElements(env, a, NULL);
    aPtr = aPtrBase + 2*aIdx;
  }
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  jdouble *cPtrBase = 0, *cPtr = 0;
  if (c) {
    if((*env)->IsSameObject(env, c, a) == JNI_TRUE)
      cPtrBase = aPtrBase;
    else
      cPtrBase = (*env)->GetDoubleArrayElements(env, c, NULL);
    cPtr = cPtrBase + 2*cIdx;
  }

  savedEnv = env;
  zsyrk_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, &betaCplx, cPtr, &ldc);
  if(cPtrBase && cPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, c, cPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void ztbmv_(char *, char *, char *, jint *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ztbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void ztbsv_(char *, char *, char *, jint *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ztbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray ap, jint apIdx, jdoubleArray x, jint xIdx, jint incx)
{
  extern void ztpmv_(char *, char *, char *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  ztpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray ap, jint apIdx, jdoubleArray x, jint xIdx, jint incx)
{
  extern void ztpsv_(char *, char *, char *, jint *, jdouble *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  jdouble *apPtrBase = 0, *apPtr = 0;
  if (ap) {
    apPtrBase = (*env)->GetDoubleArrayElements(env, ap, NULL);
    apPtr = apPtrBase + 2*apIdx;
  }
  jdouble *xPtrBase = 0, *xPtr = 0;
  if (x) {
    if((*env)->IsSameObject(env, x, ap) == JNI_TRUE)
      xPtrBase = apPtrBase;
    else
      xPtrBase = (*env)->GetDoubleArrayElements(env, x, NULL);
    xPtr = xPtrBase + 2*xIdx;
  }

  savedEnv = env;
  ztpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
  if(xPtrBase && xPtrBase != apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(apPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, ap, apPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztrmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb)
{
  extern void ztrmm_(char *, char *, char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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

  savedEnv = env;
  ztrmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztrmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void ztrmv_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ztrmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztrsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb)
{
  extern void ztrsm_(char *, char *, char *, char *, jint *, jint *, ComplexDouble *, jdouble *, jint *, jdouble *, jint *);
  
  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
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

  savedEnv = env;
  ztrsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT void JNICALL Java_org_jblas_la_Blas_ztrsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray x, jint xIdx, jint incx)
{
  extern void ztrsv_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jint *);
  
  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
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

  savedEnv = env;
  ztrsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
  if(xPtrBase && xPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, x, xPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dgesv(JNIEnv *env, jclass this, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jdoubleArray b, jint bIdx, jint ldb)
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
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(ipivPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_sgesv(JNIEnv *env, jclass this, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jfloatArray b, jint bIdx, jint ldb)
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
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(ipivPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dsysv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jdoubleArray b, jint bIdx, jint ldb, jdoubleArray work, jint workIdx, jint lwork)
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
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != bPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(ipivPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_ssysv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jintArray ipiv, jint ipivIdx, jfloatArray b, jint bIdx, jint ldb, jfloatArray work, jint workIdx, jint lwork)
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
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != bPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
  }
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(ipivPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, ipiv, ipivPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dsyev(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray w, jint wIdx, jdoubleArray work, jint workIdx, jint lwork)
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
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_ssyev(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray w, jint wIdx, jfloatArray work, jint workIdx, jint lwork)
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
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dsyevd(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray w, jint wIdx, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
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
  }
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dsyevr(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jintArray m, jint mIdx, jdoubleArray w, jint wIdx, jdoubleArray z, jint zIdx, jint ldz, jintArray isuppz, jint isuppzIdx, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
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
  if(iworkPtrBase && iworkPtrBase != mPtrBase && iworkPtrBase != isuppzPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
  }
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase && workPtrBase != zPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
  }
  if(isuppzPtrBase && isuppzPtrBase != mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, isuppz, isuppzPtrBase, 0);
  }
  if(zPtrBase && zPtrBase != aPtrBase && zPtrBase != wPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, z, zPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
  }
  if(mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dsyevx(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jdoubleArray a, jint aIdx, jint lda, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jintArray m, jint mIdx, jdoubleArray w, jint wIdx, jdoubleArray z, jint zIdx, jint ldz, jdoubleArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jintArray ifail, jint ifailIdx)
{
  extern void dsyevx_(char *, char *, char *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jint *, jdouble *, jint *, jdouble *, jdouble *, jint *, jdouble *, jint *, jint *, jint *, int *);
  
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
      iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jint *ifailPtrBase = 0, *ifailPtr = 0;
  if (ifail) {
    if((*env)->IsSameObject(env, ifail, m) == JNI_TRUE)
      ifailPtrBase = mPtrBase;
    else
      if((*env)->IsSameObject(env, ifail, iwork) == JNI_TRUE)
      ifailPtrBase = iworkPtrBase;
    else
      ifailPtrBase = (*env)->GetIntArrayElements(env, ifail, NULL);
    ifailPtr = ifailPtrBase + ifailIdx;
  }
  int info;

  savedEnv = env;
  dsyevx_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  if(ifailPtrBase && ifailPtrBase != mPtrBase && ifailPtrBase != iworkPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, ifail, ifailPtrBase, 0);
  }
  if(iworkPtrBase && iworkPtrBase != mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
  }
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase && workPtrBase != zPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
  }
  if(zPtrBase && zPtrBase != aPtrBase && zPtrBase != wPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, z, zPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, w, wPtrBase, 0);
  }
  if(mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_ssyevd(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray w, jint wIdx, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
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
  }
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_ssyevr(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jintArray m, jint mIdx, jfloatArray w, jint wIdx, jfloatArray z, jint zIdx, jint ldz, jintArray isuppz, jint isuppzIdx, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jint liwork)
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
  if(iworkPtrBase && iworkPtrBase != mPtrBase && iworkPtrBase != isuppzPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
  }
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase && workPtrBase != zPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
  }
  if(isuppzPtrBase && isuppzPtrBase != mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, isuppz, isuppzPtrBase, 0);
  }
  if(zPtrBase && zPtrBase != aPtrBase && zPtrBase != wPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, z, zPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
  }
  if(mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_ssyevx(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jfloatArray a, jint aIdx, jint lda, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jintArray m, jint mIdx, jfloatArray w, jint wIdx, jfloatArray z, jint zIdx, jint ldz, jfloatArray work, jint workIdx, jint lwork, jintArray iwork, jint iworkIdx, jintArray ifail, jint ifailIdx)
{
  extern void ssyevx_(char *, char *, char *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jint *, jfloat *, jint *, jfloat *, jfloat *, jint *, jfloat *, jint *, jint *, jint *, int *);
  
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
      iworkPtrBase = (*env)->GetIntArrayElements(env, iwork, NULL);
    iworkPtr = iworkPtrBase + iworkIdx;
  }
  jint *ifailPtrBase = 0, *ifailPtr = 0;
  if (ifail) {
    if((*env)->IsSameObject(env, ifail, m) == JNI_TRUE)
      ifailPtrBase = mPtrBase;
    else
      if((*env)->IsSameObject(env, ifail, iwork) == JNI_TRUE)
      ifailPtrBase = iworkPtrBase;
    else
      ifailPtrBase = (*env)->GetIntArrayElements(env, ifail, NULL);
    ifailPtr = ifailPtrBase + ifailIdx;
  }
  int info;

  savedEnv = env;
  ssyevx_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, mPtr, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  if(ifailPtrBase && ifailPtrBase != mPtrBase && ifailPtrBase != iworkPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, ifail, ifailPtrBase, 0);
  }
  if(iworkPtrBase && iworkPtrBase != mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, iwork, iworkPtrBase, 0);
  }
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wPtrBase && workPtrBase != zPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
  }
  if(zPtrBase && zPtrBase != aPtrBase && zPtrBase != wPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, z, zPtrBase, 0);
  }
  if(wPtrBase && wPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, w, wPtrBase, 0);
  }
  if(mPtrBase) {        
    (*env)->ReleaseIntArrayElements(env, m, mPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dposv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jdoubleArray a, jint aIdx, jint lda, jdoubleArray b, jint bIdx, jint ldb)
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
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_sposv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jfloatArray a, jint aIdx, jint lda, jfloatArray b, jint bIdx, jint ldb)
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
  if(bPtrBase && bPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, b, bPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_dgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jdoubleArray a, jint aIdx, jint lda, jdoubleArray wr, jint wrIdx, jdoubleArray wi, jint wiIdx, jdoubleArray vl, jint vlIdx, jint ldvl, jdoubleArray vr, jint vrIdx, jint ldvr, jdoubleArray work, jint workIdx, jint lwork)
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
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wrPtrBase && workPtrBase != wiPtrBase && workPtrBase != vlPtrBase && workPtrBase != vrPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, work, workPtrBase, 0);
  }
  if(vrPtrBase && vrPtrBase != aPtrBase && vrPtrBase != wrPtrBase && vrPtrBase != wiPtrBase && vrPtrBase != vlPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, vr, vrPtrBase, 0);
  }
  if(vlPtrBase && vlPtrBase != aPtrBase && vlPtrBase != wrPtrBase && vlPtrBase != wiPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, vl, vlPtrBase, 0);
  }
  if(wiPtrBase && wiPtrBase != aPtrBase && wiPtrBase != wrPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, wi, wiPtrBase, 0);
  }
  if(wrPtrBase && wrPtrBase != aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, wr, wrPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseDoubleArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

JNIEXPORT jint JNICALL Java_org_jblas_la_Blas_sgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jfloatArray a, jint aIdx, jint lda, jfloatArray wr, jint wrIdx, jfloatArray wi, jint wiIdx, jfloatArray vl, jint vlIdx, jint ldvl, jfloatArray vr, jint vrIdx, jint ldvr, jfloatArray work, jint workIdx, jint lwork)
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
  if(workPtrBase && workPtrBase != aPtrBase && workPtrBase != wrPtrBase && workPtrBase != wiPtrBase && workPtrBase != vlPtrBase && workPtrBase != vrPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, work, workPtrBase, 0);
  }
  if(vrPtrBase && vrPtrBase != aPtrBase && vrPtrBase != wrPtrBase && vrPtrBase != wiPtrBase && vrPtrBase != vlPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, vr, vrPtrBase, 0);
  }
  if(vlPtrBase && vlPtrBase != aPtrBase && vlPtrBase != wrPtrBase && vlPtrBase != wiPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, vl, vlPtrBase, 0);
  }
  if(wiPtrBase && wiPtrBase != aPtrBase && wiPtrBase != wrPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, wi, wiPtrBase, 0);
  }
  if(wrPtrBase && wrPtrBase != aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, wr, wrPtrBase, 0);
  }
  if(aPtrBase) {        
    (*env)->ReleaseFloatArrayElements(env, a, aPtrBase, 0);
  }

  return info;
}

