#include "com_fhg_la_BlasLevel3.h"
#include "blas_wrapper.h"


JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel3_dgemm
  (JNIEnv *env, jclass this, jchar transA, jchar transB, jint m, jint n, jint k, jdouble alpha, jobject a, jint ldA, jobject b, jint ldB, jdouble beta, jobject c, jint ldC)
{
	jdouble* A	= (*env)->GetDirectBufferAddress(env, a);
	jdouble* B	= (*env)->GetDirectBufferAddress(env, b);
	jdouble* C	= (*env)->GetDirectBufferAddress(env, c);
	
	dgemm(transA, transB, m, n, k, alpha, A, ldA, B, ldB, beta, C, ldC);
}

