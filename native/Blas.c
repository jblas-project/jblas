#include "edu_ida_la_Blas.h"

#define CORE_PACKAGE "edu/ida/core/"

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


JNIEXPORT void JNICALL Java_edu_ida_la_Blas_caxpy(JNIEnv *env, jclass this, jint n, jobject ca, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy)
{
  extern void caxpy_(int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *);

  ComplexFloat caCplx;
  getComplexFloat(env, ca, &caCplx);
  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  FloatBuffer *cyPtr = (cy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  caxpy_(&n, &caCplx, cxPtr, &incx, cyPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ccopy(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy)
{
  extern void ccopy_(int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  FloatBuffer *cyPtr = (cy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  ccopy_(&n, cxPtr, &incx, cyPtr, &incy);
}

JNIEXPORT jobject JNICALL Java_edu_ida_la_Blas_cdotc(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy)
{
  extern void cdotc_(ComplexFloat *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  ComplexFloat retval;
  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  FloatBuffer *cyPtr = (cy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  cdotc_(&retval, &n, cxPtr, &incx, cyPtr, &incy);
  return createComplexFloat(env, &retval);
}

JNIEXPORT jobject JNICALL Java_edu_ida_la_Blas_cdotu(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy)
{
  extern void cdotu_(ComplexFloat *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  ComplexFloat retval;
  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  FloatBuffer *cyPtr = (cy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  cdotu_(&retval, &n, cxPtr, &incx, cyPtr, &incy);
  return createComplexFloat(env, &retval);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void cgbmv_(char *, int *, int *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  cgbmv_(&transChr, &m, &n, &kl, &ku, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void cgemm_(char *, char *, int *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char transaChr = (char) transa;
  char transbChr = (char) transb;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  cgemm_(&transaChr, &transbChr, &m, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void cgemv_(char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  cgemv_(&transChr, &m, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cgerc(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void cgerc_(int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  cgerc_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cgeru(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void cgeru_(int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  cgeru_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_chbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void chbmv_(char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  chbmv_(&uploChr, &n, &k, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_chemm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void chemm_(char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  chemm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_chemv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void chemv_(char *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  chemv_(&uploChr, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cher2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void cher2_(char *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  cher2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cher2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jfloat beta, jobject c, jint cIdx, jint ldc)
{
  extern void cher2k_(char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  cher2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cher(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject a, jint aIdx, jint lda)
{
  extern void cher_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  cher_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cherk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jfloat alpha, jobject a, jint aIdx, jint lda, jfloat beta, jobject c, jint cIdx, jint ldc)
{
  extern void cherk_(char *, char *, int *, int *, float *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  cherk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_chpmv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void chpmv_(char *, int *, ComplexFloat *, FloatBuffer *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  chpmv_(&uploChr, &n, &alphaCplx, apPtr, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_chpr2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject ap, jint apIdx)
{
  extern void chpr2_(char *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *);

  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;

  savedEnv = env;
  chpr2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_chpr(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject ap, jint apIdx)
{
  extern void chpr_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *);

  char uploChr = (char) uplo;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;

  savedEnv = env;
  chpr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_crotg(JNIEnv *env, jclass this, jobject ca, jobject cb, jfloat c, jobject s)
{
  extern void crotg_(ComplexFloat *, ComplexFloat *, float *, ComplexFloat *);

  ComplexFloat caCplx;
  getComplexFloat(env, ca, &caCplx);
  ComplexFloat cbCplx;
  getComplexFloat(env, cb, &cbCplx);
  ComplexFloat sCplx;
  getComplexFloat(env, s, &sCplx);

  savedEnv = env;
  crotg_(&caCplx, &cbCplx, &c, &sCplx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cscal(JNIEnv *env, jclass this, jint n, jobject ca, jobject cx, jint cxIdx, jint incx)
{
  extern void cscal_(int *, ComplexFloat *, FloatBuffer *, int *);

  ComplexFloat caCplx;
  getComplexFloat(env, ca, &caCplx);
  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;

  savedEnv = env;
  cscal_(&n, &caCplx, cxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_csrot(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy, jfloat c, jfloat s)
{
  extern void csrot_(int *, FloatBuffer *, int *, FloatBuffer *, int *, float *, float *);

  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  FloatBuffer *cyPtr = (cy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  csrot_(&n, cxPtr, &incx, cyPtr, &incy, &c, &s);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_csscal(JNIEnv *env, jclass this, jint n, jfloat sa, jobject cx, jint cxIdx, jint incx)
{
  extern void csscal_(int *, float *, FloatBuffer *, int *);

  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;

  savedEnv = env;
  csscal_(&n, &sa, cxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_cswap(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy)
{
  extern void cswap_(int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  FloatBuffer *cyPtr = (cy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  cswap_(&n, cxPtr, &incx, cyPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_csymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void csymm_(char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  csymm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_csyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void csyr2k_(char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  csyr2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_csyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void csyrk_(char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, ComplexFloat *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  ComplexFloat betaCplx;
  getComplexFloat(env, beta, &betaCplx);
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  csyrk_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ctbmv_(char *, char *, char *, int *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ctbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ctbsv_(char *, char *, char *, int *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ctbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void ctpmv_(char *, char *, char *, int *, FloatBuffer *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ctpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void ctpsv_(char *, char *, char *, int *, FloatBuffer *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ctpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctrmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void ctrmm_(char *, char *, char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;

  savedEnv = env;
  ctrmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctrmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ctrmv_(char *, char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ctrmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctrsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void ctrsm_(char *, char *, char *, char *, int *, int *, ComplexFloat *, FloatBuffer *, int *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexFloat alphaCplx;
  getComplexFloat(env, alpha, &alphaCplx);
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;

  savedEnv = env;
  ctrsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ctrsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ctrsv_(char *, char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ctrsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_dasum(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx)
{
  extern jdouble dasum_(int *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;

  savedEnv = env;
  return dasum_(&n, dxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_daxpy(JNIEnv *env, jclass this, jint n, jdouble da, jobject dx, jint dxIdx, jint incx, jobject dy, jint dyIdx, jint incy)
{
  extern void daxpy_(int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;
  DoubleBuffer *dyPtr = (dy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dy)) + dyIdx) : (void*)0;

  savedEnv = env;
  daxpy_(&n, &da, dxPtr, &incx, dyPtr, &incy);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_dcabs1(JNIEnv *env, jclass this, jobject z)
{
  extern jdouble dcabs1_(ComplexDouble *);

  ComplexDouble zCplx;
  getComplexDouble(env, z, &zCplx);

  savedEnv = env;
  return dcabs1_(&zCplx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dcopy(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx, jobject dy, jint dyIdx, jint incy)
{
  extern void dcopy_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;
  DoubleBuffer *dyPtr = (dy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dy)) + dyIdx) : (void*)0;

  savedEnv = env;
  dcopy_(&n, dxPtr, &incx, dyPtr, &incy);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_ddot(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx, jobject dy, jint dyIdx, jint incy)
{
  extern jdouble ddot_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;
  DoubleBuffer *dyPtr = (dy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dy)) + dyIdx) : (void*)0;

  savedEnv = env;
  return ddot_(&n, dxPtr, &incx, dyPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jdouble alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jdouble beta, jobject y, jint yIdx, jint incy)
{
  extern void dgbmv_(char *, int *, int *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char transChr = (char) trans;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  dgbmv_(&transChr, &m, &n, &kl, &ku, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jdouble alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jdouble beta, jobject c, jint cIdx, jint ldc)
{
  extern void dgemm_(char *, char *, int *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char transaChr = (char) transa;
  char transbChr = (char) transb;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  dgemm_(&transaChr, &transbChr, &m, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jdouble alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jdouble beta, jobject y, jint yIdx, jint incy)
{
  extern void dgemv_(char *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char transChr = (char) trans;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  dgemv_(&transChr, &m, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dger(JNIEnv *env, jclass this, jint m, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void dger_(int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;

  savedEnv = env;
  dger_(&m, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_dnrm2(JNIEnv *env, jclass this, jint n, jobject x, jint xIdx, jint incx)
{
  extern jdouble dnrm2_(int *, DoubleBuffer *, int *);

  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  return dnrm2_(&n, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_drot(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx, jobject dy, jint dyIdx, jint incy, jdouble c, jdouble s)
{
  extern void drot_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, double *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;
  DoubleBuffer *dyPtr = (dy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dy)) + dyIdx) : (void*)0;

  savedEnv = env;
  drot_(&n, dxPtr, &incx, dyPtr, &incy, &c, &s);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_drotg(JNIEnv *env, jclass this, jdouble da, jdouble db, jdouble c, jdouble s)
{
  extern void drotg_(double *, double *, double *, double *);


  savedEnv = env;
  drotg_(&da, &db, &c, &s);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_drotm(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx, jobject dy, jint dyIdx, jint incy, jobject dparam, jint dparamIdx)
{
  extern void drotm_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;
  DoubleBuffer *dyPtr = (dy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dy)) + dyIdx) : (void*)0;
  DoubleBuffer *dparamPtr = (dparam) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dparam)) + dparamIdx) : (void*)0;

  savedEnv = env;
  drotm_(&n, dxPtr, &incx, dyPtr, &incy, dparamPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_drotmg(JNIEnv *env, jclass this, jdouble dd1, jdouble dd2, jdouble dx1, jdouble dy1, jobject dparam, jint dparamIdx)
{
  extern void drotmg_(double *, double *, double *, double *, DoubleBuffer *);

  DoubleBuffer *dparamPtr = (dparam) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dparam)) + dparamIdx) : (void*)0;

  savedEnv = env;
  drotmg_(&dd1, &dd2, &dx1, &dy1, dparamPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jdouble alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jdouble beta, jobject y, jint yIdx, jint incy)
{
  extern void dsbmv_(char *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  dsbmv_(&uploChr, &n, &k, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dscal(JNIEnv *env, jclass this, jint n, jdouble da, jobject dx, jint dxIdx, jint incx)
{
  extern void dscal_(int *, double *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;

  savedEnv = env;
  dscal_(&n, &da, dxPtr, &incx);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_dsdot(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy)
{
  extern jdouble dsdot_(int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  return dsdot_(&n, sxPtr, &incx, syPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dspmv(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx, jdouble beta, jobject y, jint yIdx, jint incy)
{
  extern void dspmv_(char *, int *, double *, DoubleBuffer *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  dspmv_(&uploChr, &n, &alpha, apPtr, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dspr2(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject ap, jint apIdx)
{
  extern void dspr2_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *);

  char uploChr = (char) uplo;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;

  savedEnv = env;
  dspr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dspr(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject ap, jint apIdx)
{
  extern void dspr_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *);

  char uploChr = (char) uplo;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;

  savedEnv = env;
  dspr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dswap(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx, jobject dy, jint dyIdx, jint incy)
{
  extern void dswap_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;
  DoubleBuffer *dyPtr = (dy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dy)) + dyIdx) : (void*)0;

  savedEnv = env;
  dswap_(&n, dxPtr, &incx, dyPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jdouble alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jdouble beta, jobject c, jint cIdx, jint ldc)
{
  extern void dsymm_(char *, char *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  dsymm_(&sideChr, &uploChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsymv(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jdouble beta, jobject y, jint yIdx, jint incy)
{
  extern void dsymv_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  dsymv_(&uploChr, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsyr2(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void dsyr2_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;

  savedEnv = env;
  dsyr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jdouble alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jdouble beta, jobject c, jint cIdx, jint ldc)
{
  extern void dsyr2k_(char *, char *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  dsyr2k_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsyr(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject a, jint aIdx, jint lda)
{
  extern void dsyr_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;

  savedEnv = env;
  dsyr_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dsyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jdouble alpha, jobject a, jint aIdx, jint lda, jdouble beta, jobject c, jint cIdx, jint ldc)
{
  extern void dsyrk_(char *, char *, int *, int *, double *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  dsyrk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void dtbmv_(char *, char *, char *, int *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  dtbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void dtbsv_(char *, char *, char *, int *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  dtbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void dtpmv_(char *, char *, char *, int *, DoubleBuffer *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  dtpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void dtpsv_(char *, char *, char *, int *, DoubleBuffer *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  dtpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtrmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jdouble alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void dtrmm_(char *, char *, char *, char *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;

  savedEnv = env;
  dtrmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtrmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void dtrmv_(char *, char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  dtrmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtrsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jdouble alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void dtrsm_(char *, char *, char *, char *, int *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;

  savedEnv = env;
  dtrsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_dtrsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void dtrsv_(char *, char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  dtrsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_dzasum(JNIEnv *env, jclass this, jint n, jobject zx, jint zxIdx, jint incx)
{
  extern jdouble dzasum_(int *, DoubleBuffer *, int *);

  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;

  savedEnv = env;
  return dzasum_(&n, zxPtr, &incx);
}

JNIEXPORT jdouble JNICALL Java_edu_ida_la_Blas_dznrm2(JNIEnv *env, jclass this, jint n, jobject x, jint xIdx, jint incx)
{
  extern jdouble dznrm2_(int *, DoubleBuffer *, int *);

  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  return dznrm2_(&n, xPtr, &incx);
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_icamax(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx)
{
  extern jint icamax_(int *, FloatBuffer *, int *);

  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;

  savedEnv = env;
  return icamax_(&n, cxPtr, &incx);
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_idamax(JNIEnv *env, jclass this, jint n, jobject dx, jint dxIdx, jint incx)
{
  extern jint idamax_(int *, DoubleBuffer *, int *);

  DoubleBuffer *dxPtr = (dx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, dx)) + dxIdx) : (void*)0;

  savedEnv = env;
  return idamax_(&n, dxPtr, &incx);
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_isamax(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx)
{
  extern jint isamax_(int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;

  savedEnv = env;
  return isamax_(&n, sxPtr, &incx);
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_izamax(JNIEnv *env, jclass this, jint n, jobject zx, jint zxIdx, jint incx)
{
  extern jint izamax_(int *, DoubleBuffer *, int *);

  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;

  savedEnv = env;
  return izamax_(&n, zxPtr, &incx);
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_lsame(JNIEnv *env, jclass this, jchar ca, jchar cb)
{
  extern jint lsame_(char *, char *);

  char caChr = (char) ca;
  char cbChr = (char) cb;

  savedEnv = env;
  return lsame_(&caChr, &cbChr);
}

JNIEXPORT jfloat JNICALL Java_edu_ida_la_Blas_sasum(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx)
{
  extern jdouble sasum_(int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;

  savedEnv = env;
  return (jdouble)sasum_(&n, sxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_saxpy(JNIEnv *env, jclass this, jint n, jfloat sa, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy)
{
  extern void saxpy_(int *, float *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  saxpy_(&n, &sa, sxPtr, &incx, syPtr, &incy);
}

JNIEXPORT jfloat JNICALL Java_edu_ida_la_Blas_scasum(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx)
{
  extern jdouble scasum_(int *, FloatBuffer *, int *);

  FloatBuffer *cxPtr = (cx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;

  savedEnv = env;
  return (jdouble)scasum_(&n, cxPtr, &incx);
}

JNIEXPORT jfloat JNICALL Java_edu_ida_la_Blas_scnrm2(JNIEnv *env, jclass this, jint n, jobject x, jint xIdx, jint incx)
{
  extern jdouble scnrm2_(int *, FloatBuffer *, int *);

  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  return (jdouble)scnrm2_(&n, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_scopy(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy)
{
  extern void scopy_(int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  scopy_(&n, sxPtr, &incx, syPtr, &incy);
}

JNIEXPORT jfloat JNICALL Java_edu_ida_la_Blas_sdot(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy)
{
  extern jdouble sdot_(int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  return (jdouble)sdot_(&n, sxPtr, &incx, syPtr, &incy);
}

JNIEXPORT jfloat JNICALL Java_edu_ida_la_Blas_sdsdot(JNIEnv *env, jclass this, jint n, jfloat sb, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy)
{
  extern jdouble sdsdot_(int *, float *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  return (jdouble)sdsdot_(&n, &sb, sxPtr, &incx, syPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jfloat alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jfloat beta, jobject y, jint yIdx, jint incy)
{
  extern void sgbmv_(char *, int *, int *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char transChr = (char) trans;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  sgbmv_(&transChr, &m, &n, &kl, &ku, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jfloat alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jfloat beta, jobject c, jint cIdx, jint ldc)
{
  extern void sgemm_(char *, char *, int *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char transaChr = (char) transa;
  char transbChr = (char) transb;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  sgemm_(&transaChr, &transbChr, &m, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jfloat alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jfloat beta, jobject y, jint yIdx, jint incy)
{
  extern void sgemv_(char *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char transChr = (char) trans;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  sgemv_(&transChr, &m, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sger(JNIEnv *env, jclass this, jint m, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void sger_(int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;

  savedEnv = env;
  sger_(&m, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT jfloat JNICALL Java_edu_ida_la_Blas_snrm2(JNIEnv *env, jclass this, jint n, jobject x, jint xIdx, jint incx)
{
  extern jdouble snrm2_(int *, FloatBuffer *, int *);

  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  return (jdouble)snrm2_(&n, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_srot(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy, jfloat c, jfloat s)
{
  extern void srot_(int *, FloatBuffer *, int *, FloatBuffer *, int *, float *, float *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  srot_(&n, sxPtr, &incx, syPtr, &incy, &c, &s);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_srotg(JNIEnv *env, jclass this, jfloat sa, jfloat sb, jfloat c, jfloat s)
{
  extern void srotg_(float *, float *, float *, float *);


  savedEnv = env;
  srotg_(&sa, &sb, &c, &s);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_srotm(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy, jobject sparam, jint sparamIdx)
{
  extern void srotm_(int *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;
  FloatBuffer *sparamPtr = (sparam) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sparam)) + sparamIdx) : (void*)0;

  savedEnv = env;
  srotm_(&n, sxPtr, &incx, syPtr, &incy, sparamPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_srotmg(JNIEnv *env, jclass this, jfloat sd1, jfloat sd2, jfloat sx1, jfloat sy1, jobject sparam, jint sparamIdx)
{
  extern void srotmg_(float *, float *, float *, float *, FloatBuffer *);

  FloatBuffer *sparamPtr = (sparam) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sparam)) + sparamIdx) : (void*)0;

  savedEnv = env;
  srotmg_(&sd1, &sd2, &sx1, &sy1, sparamPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jfloat alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jfloat beta, jobject y, jint yIdx, jint incy)
{
  extern void ssbmv_(char *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  ssbmv_(&uploChr, &n, &k, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sscal(JNIEnv *env, jclass this, jint n, jfloat sa, jobject sx, jint sxIdx, jint incx)
{
  extern void sscal_(int *, float *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;

  savedEnv = env;
  sscal_(&n, &sa, sxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sspmv(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx, jfloat beta, jobject y, jint yIdx, jint incy)
{
  extern void sspmv_(char *, int *, float *, FloatBuffer *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  sspmv_(&uploChr, &n, &alpha, apPtr, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sspr2(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject ap, jint apIdx)
{
  extern void sspr2_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *);

  char uploChr = (char) uplo;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;

  savedEnv = env;
  sspr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sspr(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject ap, jint apIdx)
{
  extern void sspr_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *);

  char uploChr = (char) uplo;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;

  savedEnv = env;
  sspr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_sswap(JNIEnv *env, jclass this, jint n, jobject sx, jint sxIdx, jint incx, jobject sy, jint syIdx, jint incy)
{
  extern void sswap_(int *, FloatBuffer *, int *, FloatBuffer *, int *);

  FloatBuffer *sxPtr = (sx) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sx)) + sxIdx) : (void*)0;
  FloatBuffer *syPtr = (sy) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, sy)) + syIdx) : (void*)0;

  savedEnv = env;
  sswap_(&n, sxPtr, &incx, syPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jfloat alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jfloat beta, jobject c, jint cIdx, jint ldc)
{
  extern void ssymm_(char *, char *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  ssymm_(&sideChr, &uploChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssymv(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jfloat beta, jobject y, jint yIdx, jint incy)
{
  extern void ssymv_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;

  savedEnv = env;
  ssymv_(&uploChr, &n, &alpha, aPtr, &lda, xPtr, &incx, &beta, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssyr2(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void ssyr2_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *yPtr = (y) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, y)) + yIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;

  savedEnv = env;
  ssyr2_(&uploChr, &n, &alpha, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jfloat alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jfloat beta, jobject c, jint cIdx, jint ldc)
{
  extern void ssyr2k_(char *, char *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  ssyr2k_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssyr(JNIEnv *env, jclass this, jchar uplo, jint n, jfloat alpha, jobject x, jint xIdx, jint incx, jobject a, jint aIdx, jint lda)
{
  extern void ssyr_(char *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;

  savedEnv = env;
  ssyr_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ssyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jfloat alpha, jobject a, jint aIdx, jint lda, jfloat beta, jobject c, jint cIdx, jint ldc)
{
  extern void ssyrk_(char *, char *, int *, int *, float *, FloatBuffer *, int *, float *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *cPtr = (c) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, c)) + cIdx) : (void*)0;

  savedEnv = env;
  ssyrk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_stbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void stbmv_(char *, char *, char *, int *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  stbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_stbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void stbsv_(char *, char *, char *, int *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  stbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_stpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void stpmv_(char *, char *, char *, int *, FloatBuffer *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  stpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_stpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void stpsv_(char *, char *, char *, int *, FloatBuffer *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *apPtr = (ap) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + apIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  stpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_strmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jfloat alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void strmm_(char *, char *, char *, char *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;

  savedEnv = env;
  strmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_strmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void strmv_(char *, char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  strmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_strsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jfloat alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void strsm_(char *, char *, char *, char *, int *, int *, float *, FloatBuffer *, int *, FloatBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;

  savedEnv = env;
  strsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alpha, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_strsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void strsv_(char *, char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *xPtr = (x) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, x)) + xIdx) : (void*)0;

  savedEnv = env;
  strsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zaxpy(JNIEnv *env, jclass this, jint n, jobject za, jobject zx, jint zxIdx, jint incx, jobject zy, jint zyIdx, jint incy)
{
  extern void zaxpy_(int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  ComplexDouble zaCplx;
  getComplexDouble(env, za, &zaCplx);
  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;
  DoubleBuffer *zyPtr = (zy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zy)) + 2*zyIdx) : (void*)0;

  savedEnv = env;
  zaxpy_(&n, &zaCplx, zxPtr, &incx, zyPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zcopy(JNIEnv *env, jclass this, jint n, jobject zx, jint zxIdx, jint incx, jobject zy, jint zyIdx, jint incy)
{
  extern void zcopy_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;
  DoubleBuffer *zyPtr = (zy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zy)) + 2*zyIdx) : (void*)0;

  savedEnv = env;
  zcopy_(&n, zxPtr, &incx, zyPtr, &incy);
}

JNIEXPORT jobject JNICALL Java_edu_ida_la_Blas_zdotc(JNIEnv *env, jclass this, jint n, jobject zx, jint zxIdx, jint incx, jobject zy, jint zyIdx, jint incy)
{
  extern void zdotc_(ComplexDouble *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  ComplexDouble retval;
  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;
  DoubleBuffer *zyPtr = (zy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zy)) + 2*zyIdx) : (void*)0;

  savedEnv = env;
  zdotc_(&retval, &n, zxPtr, &incx, zyPtr, &incy);
  return createComplexDouble(env, &retval);
}

JNIEXPORT jobject JNICALL Java_edu_ida_la_Blas_zdotu(JNIEnv *env, jclass this, jint n, jobject zx, jint zxIdx, jint incx, jobject zy, jint zyIdx, jint incy)
{
  extern void zdotu_(ComplexDouble *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  ComplexDouble retval;
  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;
  DoubleBuffer *zyPtr = (zy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zy)) + 2*zyIdx) : (void*)0;

  savedEnv = env;
  zdotu_(&retval, &n, zxPtr, &incx, zyPtr, &incy);
  return createComplexDouble(env, &retval);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zdrot(JNIEnv *env, jclass this, jint n, jobject cx, jint cxIdx, jint incx, jobject cy, jint cyIdx, jint incy, jdouble c, jdouble s)
{
  extern void zdrot_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, double *);

  DoubleBuffer *cxPtr = (cx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, cx)) + 2*cxIdx) : (void*)0;
  DoubleBuffer *cyPtr = (cy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, cy)) + 2*cyIdx) : (void*)0;

  savedEnv = env;
  zdrot_(&n, cxPtr, &incx, cyPtr, &incy, &c, &s);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zdscal(JNIEnv *env, jclass this, jint n, jdouble da, jobject zx, jint zxIdx, jint incx)
{
  extern void zdscal_(int *, double *, DoubleBuffer *, int *);

  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;

  savedEnv = env;
  zdscal_(&n, &da, zxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zgbmv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jint kl, jint ku, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void zgbmv_(char *, int *, int *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  zgbmv_(&transChr, &m, &n, &kl, &ku, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zgemm(JNIEnv *env, jclass this, jchar transa, jchar transb, jint m, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void zgemm_(char *, char *, int *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char transaChr = (char) transa;
  char transbChr = (char) transb;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zgemm_(&transaChr, &transbChr, &m, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zgemv(JNIEnv *env, jclass this, jchar trans, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void zgemv_(char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  zgemv_(&transChr, &m, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zgerc(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void zgerc_(int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  zgerc_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zgeru(JNIEnv *env, jclass this, jint m, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void zgeru_(int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  zgeru_(&m, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zhbmv(JNIEnv *env, jclass this, jchar uplo, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void zhbmv_(char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  zhbmv_(&uploChr, &n, &k, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zhemm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void zhemm_(char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zhemm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zhemv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void zhemv_(char *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  zhemv_(&uploChr, &n, &alphaCplx, aPtr, &lda, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zher2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject a, jint aIdx, jint lda)
{
  extern void zher2_(char *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  zher2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zher2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jdouble beta, jobject c, jint cIdx, jint ldc)
{
  extern void zher2k_(char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zher2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zher(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject a, jint aIdx, jint lda)
{
  extern void zher_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;

  savedEnv = env;
  zher_(&uploChr, &n, &alpha, xPtr, &incx, aPtr, &lda);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zherk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jdouble alpha, jobject a, jint aIdx, jint lda, jdouble beta, jobject c, jint cIdx, jint ldc)
{
  extern void zherk_(char *, char *, int *, int *, double *, DoubleBuffer *, int *, double *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zherk_(&uploChr, &transChr, &n, &k, &alpha, aPtr, &lda, &beta, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zhpmv(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx, jobject beta, jobject y, jint yIdx, jint incy)
{
  extern void zhpmv_(char *, int *, ComplexDouble *, DoubleBuffer *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;

  savedEnv = env;
  zhpmv_(&uploChr, &n, &alphaCplx, apPtr, xPtr, &incx, &betaCplx, yPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zhpr2(JNIEnv *env, jclass this, jchar uplo, jint n, jobject alpha, jobject x, jint xIdx, jint incx, jobject y, jint yIdx, jint incy, jobject ap, jint apIdx)
{
  extern void zhpr2_(char *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *);

  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  DoubleBuffer *yPtr = (y) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, y)) + 2*yIdx) : (void*)0;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;

  savedEnv = env;
  zhpr2_(&uploChr, &n, &alphaCplx, xPtr, &incx, yPtr, &incy, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zhpr(JNIEnv *env, jclass this, jchar uplo, jint n, jdouble alpha, jobject x, jint xIdx, jint incx, jobject ap, jint apIdx)
{
  extern void zhpr_(char *, int *, double *, DoubleBuffer *, int *, DoubleBuffer *);

  char uploChr = (char) uplo;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;

  savedEnv = env;
  zhpr_(&uploChr, &n, &alpha, xPtr, &incx, apPtr);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zrotg(JNIEnv *env, jclass this, jobject ca, jobject cb, jdouble c, jobject s)
{
  extern void zrotg_(ComplexDouble *, ComplexDouble *, double *, ComplexDouble *);

  ComplexDouble caCplx;
  getComplexDouble(env, ca, &caCplx);
  ComplexDouble cbCplx;
  getComplexDouble(env, cb, &cbCplx);
  ComplexDouble sCplx;
  getComplexDouble(env, s, &sCplx);

  savedEnv = env;
  zrotg_(&caCplx, &cbCplx, &c, &sCplx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zscal(JNIEnv *env, jclass this, jint n, jobject za, jobject zx, jint zxIdx, jint incx)
{
  extern void zscal_(int *, ComplexDouble *, DoubleBuffer *, int *);

  ComplexDouble zaCplx;
  getComplexDouble(env, za, &zaCplx);
  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;

  savedEnv = env;
  zscal_(&n, &zaCplx, zxPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zswap(JNIEnv *env, jclass this, jint n, jobject zx, jint zxIdx, jint incx, jobject zy, jint zyIdx, jint incy)
{
  extern void zswap_(int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  DoubleBuffer *zxPtr = (zx) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zx)) + 2*zxIdx) : (void*)0;
  DoubleBuffer *zyPtr = (zy) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, zy)) + 2*zyIdx) : (void*)0;

  savedEnv = env;
  zswap_(&n, zxPtr, &incx, zyPtr, &incy);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zsymm(JNIEnv *env, jclass this, jchar side, jchar uplo, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void zsymm_(char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zsymm_(&sideChr, &uploChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zsyr2k(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void zsyr2k_(char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zsyr2k_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, bPtr, &ldb, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_zsyrk(JNIEnv *env, jclass this, jchar uplo, jchar trans, jint n, jint k, jobject alpha, jobject a, jint aIdx, jint lda, jobject beta, jobject c, jint cIdx, jint ldc)
{
  extern void zsyrk_(char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, ComplexDouble *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  ComplexDouble betaCplx;
  getComplexDouble(env, beta, &betaCplx);
  DoubleBuffer *cPtr = (c) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, c)) + 2*cIdx) : (void*)0;

  savedEnv = env;
  zsyrk_(&uploChr, &transChr, &n, &k, &alphaCplx, aPtr, &lda, &betaCplx, cPtr, &ldc);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztbmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ztbmv_(char *, char *, char *, int *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ztbmv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztbsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jint k, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ztbsv_(char *, char *, char *, int *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ztbsv_(&uploChr, &transChr, &diagChr, &n, &k, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztpmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void ztpmv_(char *, char *, char *, int *, DoubleBuffer *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ztpmv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztpsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject ap, jint apIdx, jobject x, jint xIdx, jint incx)
{
  extern void ztpsv_(char *, char *, char *, int *, DoubleBuffer *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *apPtr = (ap) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, ap)) + 2*apIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ztpsv_(&uploChr, &transChr, &diagChr, &n, apPtr, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztrmm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void ztrmm_(char *, char *, char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;

  savedEnv = env;
  ztrmm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztrmv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ztrmv_(char *, char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ztrmv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztrsm(JNIEnv *env, jclass this, jchar side, jchar uplo, jchar transa, jchar diag, jint m, jint n, jobject alpha, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void ztrsm_(char *, char *, char *, char *, int *, int *, ComplexDouble *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char sideChr = (char) side;
  char uploChr = (char) uplo;
  char transaChr = (char) transa;
  char diagChr = (char) diag;
  ComplexDouble alphaCplx;
  getComplexDouble(env, alpha, &alphaCplx);
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + 2*bIdx) : (void*)0;

  savedEnv = env;
  ztrsm_(&sideChr, &uploChr, &transaChr, &diagChr, &m, &n, &alphaCplx, aPtr, &lda, bPtr, &ldb);
}

JNIEXPORT void JNICALL Java_edu_ida_la_Blas_ztrsv(JNIEnv *env, jclass this, jchar uplo, jchar trans, jchar diag, jint n, jobject a, jint aIdx, jint lda, jobject x, jint xIdx, jint incx)
{
  extern void ztrsv_(char *, char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *);

  char uploChr = (char) uplo;
  char transChr = (char) trans;
  char diagChr = (char) diag;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + 2*aIdx) : (void*)0;
  DoubleBuffer *xPtr = (x) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, x)) + 2*xIdx) : (void*)0;

  savedEnv = env;
  ztrsv_(&uploChr, &transChr, &diagChr, &n, aPtr, &lda, xPtr, &incx);
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dgesv(JNIEnv *env, jclass this, jint n, jint nrhs, jobject a, jint aIdx, jint lda, jobject ipiv, jint ipivIdx, jobject b, jint bIdx, jint ldb)
{
  extern void dgesv_(int *, int *, DoubleBuffer *, int *, IntBuffer *, DoubleBuffer *, int *, int *);

  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  IntBuffer *ipivPtr = (ipiv) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, ipiv)) + ipivIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  int info;

  savedEnv = env;
  dgesv_(&n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_sgesv(JNIEnv *env, jclass this, jint n, jint nrhs, jobject a, jint aIdx, jint lda, jobject ipiv, jint ipivIdx, jobject b, jint bIdx, jint ldb)
{
  extern void sgesv_(int *, int *, FloatBuffer *, int *, IntBuffer *, FloatBuffer *, int *, int *);

  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  IntBuffer *ipivPtr = (ipiv) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, ipiv)) + ipivIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  int info;

  savedEnv = env;
  sgesv_(&n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dsysv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jobject a, jint aIdx, jint lda, jobject ipiv, jint ipivIdx, jobject b, jint bIdx, jint ldb, jobject work, jint workIdx, jint lwork)
{
  extern void dsysv_(char *, int *, int *, DoubleBuffer *, int *, IntBuffer *, DoubleBuffer *, int *, DoubleBuffer *, int *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  IntBuffer *ipivPtr = (ipiv) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, ipiv)) + ipivIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  DoubleBuffer *workPtr = (work) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  int info;

  savedEnv = env;
  dsysv_(&uploChr, &n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, workPtr, &lwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_ssysv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jobject a, jint aIdx, jint lda, jobject ipiv, jint ipivIdx, jobject b, jint bIdx, jint ldb, jobject work, jint workIdx, jint lwork)
{
  extern void ssysv_(char *, int *, int *, FloatBuffer *, int *, IntBuffer *, FloatBuffer *, int *, FloatBuffer *, int *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  IntBuffer *ipivPtr = (ipiv) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, ipiv)) + ipivIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  FloatBuffer *workPtr = (work) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  int info;

  savedEnv = env;
  ssysv_(&uploChr, &n, &nrhs, aPtr, &lda, ipivPtr, bPtr, &ldb, workPtr, &lwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dsyev(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jobject w, jint wIdx, jobject work, jint workIdx, jint lwork)
{
  extern void dsyev_(char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, DoubleBuffer *, int *, int *);

  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *wPtr = (w) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  DoubleBuffer *workPtr = (work) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  int info;

  savedEnv = env;
  dsyev_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_ssyev(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jobject w, jint wIdx, jobject work, jint workIdx, jint lwork)
{
  extern void ssyev_(char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, FloatBuffer *, int *, int *);

  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *wPtr = (w) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  FloatBuffer *workPtr = (work) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  int info;

  savedEnv = env;
  ssyev_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dsyevd(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jobject w, jint wIdx, jobject work, jint workIdx, jint lwork, jobject iwork, jint iworkIdx, jint liwork)
{
  extern void dsyevd_(char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, DoubleBuffer *, int *, IntBuffer *, int *, int *);

  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *wPtr = (w) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  DoubleBuffer *workPtr = (work) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  IntBuffer *iworkPtr = (iwork) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, iwork)) + iworkIdx) : (void*)0;
  int info;

  savedEnv = env;
  dsyevd_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dsyevr(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jint m, jobject w, jint wIdx, jobject z, jint zIdx, jint ldz, jobject isuppz, jint isuppzIdx, jobject work, jint workIdx, jint lwork, jobject iwork, jint iworkIdx, jint liwork)
{
  extern void dsyevr_(char *, char *, char *, int *, DoubleBuffer *, int *, double *, double *, int *, int *, double *, int *, DoubleBuffer *, DoubleBuffer *, int *, IntBuffer *, DoubleBuffer *, int *, IntBuffer *, int *, int *);

  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *wPtr = (w) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  DoubleBuffer *zPtr = (z) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, z)) + zIdx) : (void*)0;
  IntBuffer *isuppzPtr = (isuppz) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, isuppz)) + isuppzIdx) : (void*)0;
  DoubleBuffer *workPtr = (work) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  IntBuffer *iworkPtr = (iwork) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, iwork)) + iworkIdx) : (void*)0;
  int info;

  savedEnv = env;
  dsyevr_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, &m, wPtr, zPtr, &ldz, isuppzPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dsyevx(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jdouble vl, jdouble vu, jint il, jint iu, jdouble abstol, jint m, jobject w, jint wIdx, jobject z, jint zIdx, jint ldz, jobject work, jint workIdx, jint lwork, jobject iwork, jint iworkIdx, jobject ifail, jint ifailIdx)
{
  extern void dsyevx_(char *, char *, char *, int *, DoubleBuffer *, int *, double *, double *, int *, int *, double *, int *, DoubleBuffer *, DoubleBuffer *, int *, DoubleBuffer *, int *, IntBuffer *, IntBuffer *, int *);

  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *wPtr = (w) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  DoubleBuffer *zPtr = (z) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, z)) + zIdx) : (void*)0;
  DoubleBuffer *workPtr = (work) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  IntBuffer *iworkPtr = (iwork) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, iwork)) + iworkIdx) : (void*)0;
  IntBuffer *ifailPtr = (ifail) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, ifail)) + ifailIdx) : (void*)0;
  int info;

  savedEnv = env;
  dsyevx_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, &m, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_ssyevd(JNIEnv *env, jclass this, jchar jobz, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jobject w, jint wIdx, jobject work, jint workIdx, jint lwork, jobject iwork, jint iworkIdx, jint liwork)
{
  extern void ssyevd_(char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, FloatBuffer *, int *, IntBuffer *, int *, int *);

  char jobzChr = (char) jobz;
  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *wPtr = (w) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  FloatBuffer *workPtr = (work) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  IntBuffer *iworkPtr = (iwork) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, iwork)) + iworkIdx) : (void*)0;
  int info;

  savedEnv = env;
  ssyevd_(&jobzChr, &uploChr, &n, aPtr, &lda, wPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_ssyevr(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jint m, jobject w, jint wIdx, jobject z, jint zIdx, jint ldz, jobject isuppz, jint isuppzIdx, jobject work, jint workIdx, jint lwork, jobject iwork, jint iworkIdx, jint liwork)
{
  extern void ssyevr_(char *, char *, char *, int *, FloatBuffer *, int *, float *, float *, int *, int *, float *, int *, FloatBuffer *, FloatBuffer *, int *, IntBuffer *, FloatBuffer *, int *, IntBuffer *, int *, int *);

  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *wPtr = (w) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  FloatBuffer *zPtr = (z) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, z)) + zIdx) : (void*)0;
  IntBuffer *isuppzPtr = (isuppz) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, isuppz)) + isuppzIdx) : (void*)0;
  FloatBuffer *workPtr = (work) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  IntBuffer *iworkPtr = (iwork) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, iwork)) + iworkIdx) : (void*)0;
  int info;

  savedEnv = env;
  ssyevr_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, &m, wPtr, zPtr, &ldz, isuppzPtr, workPtr, &lwork, iworkPtr, &liwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_ssyevx(JNIEnv *env, jclass this, jchar jobz, jchar range, jchar uplo, jint n, jobject a, jint aIdx, jint lda, jfloat vl, jfloat vu, jint il, jint iu, jfloat abstol, jint m, jobject w, jint wIdx, jobject z, jint zIdx, jint ldz, jobject work, jint workIdx, jint lwork, jobject iwork, jint iworkIdx, jobject ifail, jint ifailIdx)
{
  extern void ssyevx_(char *, char *, char *, int *, FloatBuffer *, int *, float *, float *, int *, int *, float *, int *, FloatBuffer *, FloatBuffer *, int *, FloatBuffer *, int *, IntBuffer *, IntBuffer *, int *);

  char jobzChr = (char) jobz;
  char rangeChr = (char) range;
  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *wPtr = (w) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, w)) + wIdx) : (void*)0;
  FloatBuffer *zPtr = (z) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, z)) + zIdx) : (void*)0;
  FloatBuffer *workPtr = (work) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  IntBuffer *iworkPtr = (iwork) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, iwork)) + iworkIdx) : (void*)0;
  IntBuffer *ifailPtr = (ifail) ? (((IntBuffer*)(*env)->GetDirectBufferAddress(env, ifail)) + ifailIdx) : (void*)0;
  int info;

  savedEnv = env;
  ssyevx_(&jobzChr, &rangeChr, &uploChr, &n, aPtr, &lda, &vl, &vu, &il, &iu, &abstol, &m, wPtr, zPtr, &ldz, workPtr, &lwork, iworkPtr, ifailPtr, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dposv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void dposv_(char *, int *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *, int *);

  char uploChr = (char) uplo;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *bPtr = (b) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  int info;

  savedEnv = env;
  dposv_(&uploChr, &n, &nrhs, aPtr, &lda, bPtr, &ldb, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_sposv(JNIEnv *env, jclass this, jchar uplo, jint n, jint nrhs, jobject a, jint aIdx, jint lda, jobject b, jint bIdx, jint ldb)
{
  extern void sposv_(char *, int *, int *, FloatBuffer *, int *, FloatBuffer *, int *, int *);

  char uploChr = (char) uplo;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *bPtr = (b) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, b)) + bIdx) : (void*)0;
  int info;

  savedEnv = env;
  sposv_(&uploChr, &n, &nrhs, aPtr, &lda, bPtr, &ldb, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_dgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jobject a, jint aIdx, jint lda, jobject wr, jint wrIdx, jobject wi, jint wiIdx, jobject vl, jint vlIdx, jint ldvl, jobject vr, jint vrIdx, jint ldvr, jobject work, jint workIdx, jint lwork)
{
  extern void dgeev_(char *, char *, int *, DoubleBuffer *, int *, DoubleBuffer *, DoubleBuffer *, DoubleBuffer *, int *, DoubleBuffer *, int *, DoubleBuffer *, int *, int *);

  char jobvlChr = (char) jobvl;
  char jobvrChr = (char) jobvr;
  DoubleBuffer *aPtr = (a) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  DoubleBuffer *wrPtr = (wr) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, wr)) + wrIdx) : (void*)0;
  DoubleBuffer *wiPtr = (wi) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, wi)) + wiIdx) : (void*)0;
  DoubleBuffer *vlPtr = (vl) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, vl)) + vlIdx) : (void*)0;
  DoubleBuffer *vrPtr = (vr) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, vr)) + vrIdx) : (void*)0;
  DoubleBuffer *workPtr = (work) ? (((DoubleBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  int info;

  savedEnv = env;
  dgeev_(&jobvlChr, &jobvrChr, &n, aPtr, &lda, wrPtr, wiPtr, vlPtr, &ldvl, vrPtr, &ldvr, workPtr, &lwork, &info);
  return info;
}

JNIEXPORT jint JNICALL Java_edu_ida_la_Blas_sgeev(JNIEnv *env, jclass this, jchar jobvl, jchar jobvr, jint n, jobject a, jint aIdx, jint lda, jobject wr, jint wrIdx, jobject wi, jint wiIdx, jobject vl, jint vlIdx, jint ldvl, jobject vr, jint vrIdx, jint ldvr, jobject work, jint workIdx, jint lwork)
{
  extern void sgeev_(char *, char *, int *, FloatBuffer *, int *, FloatBuffer *, FloatBuffer *, FloatBuffer *, int *, FloatBuffer *, int *, FloatBuffer *, int *, int *);

  char jobvlChr = (char) jobvl;
  char jobvrChr = (char) jobvr;
  FloatBuffer *aPtr = (a) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, a)) + aIdx) : (void*)0;
  FloatBuffer *wrPtr = (wr) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, wr)) + wrIdx) : (void*)0;
  FloatBuffer *wiPtr = (wi) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, wi)) + wiIdx) : (void*)0;
  FloatBuffer *vlPtr = (vl) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, vl)) + vlIdx) : (void*)0;
  FloatBuffer *vrPtr = (vr) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, vr)) + vrIdx) : (void*)0;
  FloatBuffer *workPtr = (work) ? (((FloatBuffer*)(*env)->GetDirectBufferAddress(env, work)) + workIdx) : (void*)0;
  int info;

  savedEnv = env;
  sgeev_(&jobvlChr, &jobvrChr, &n, aPtr, &lda, wrPtr, wiPtr, vlPtr, &ldvl, vrPtr, &ldvr, workPtr, &lwork, &info);
  return info;
}

