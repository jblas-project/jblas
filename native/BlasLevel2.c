#include "com_fhg_la_BlasLevel2.h"
#include "blas_wrapper.h"

JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel2_dgemv
  (JNIEnv *env, jclass this, jchar trans, jint m, jint n, jdouble alpha, jobject a, jint ldA, jobject dX, jint incX, jdouble beta, jobject dY, jint incY)
{
	jdouble* A	= (*env)->GetDirectBufferAddress(env, a);
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);

	dgemv(trans, m, n, alpha, A, ldA, DX, incX, beta, DY, incY);
}

/*
 * Class:     com_fhg_la_BlasLevel2
 * Method:    dger
 * Signature: (IIDLjava/nio/DoubleBuffer;ILjava/nio/DoubleBuffer;ILjava/nio/DoubleBuffer;I)V
 */
JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel2_dger
  (JNIEnv *env, jclass this, jint m, jint n, jdouble alpha, jobject dX, jint incX, jobject dY, jint incY, jobject a, jint ldA)
{
	jdouble* A	= (*env)->GetDirectBufferAddress(env, a);
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);

	dger(m, n, alpha, DX, incX, DY, incY, A, ldA);
}
