#include "com_fhg_la_BlasLevel1.h"
#include "blas_wrapper.h"



JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel1_drotg
  (JNIEnv *env, jclass this, jdouble a, jdouble b, jdouble c, jdouble s)
	{
		drotg(a, b, c, s);
	}

JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel1_drot
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX, jobject dY, jint incY, jdouble c, jdouble s)
	{
		jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
		jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);
		
		drot(n, DX, incX, DY, incY, c, s);
	}	

JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel1_dswap
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX, jobject dY, jint incY) 
	{
		jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
		jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);
	
		dswap(n, DX, incX, DY, incY);
	}


JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel1_dscal
  (JNIEnv *env, jclass this, jint n, jdouble alpha, jobject dX, jint incX) 
{
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	
	dscal(n, alpha, DX, incX);
}


JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel1_dcopy
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX, jobject dY, jint incY) 
{
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);

	dcopy(n, DX, incX, DY, incY);
}

JNIEXPORT void JNICALL Java_com_fhg_la_BlasLevel1_daxpy
  (JNIEnv *env, jclass this, jint n, jdouble dA, jobject dX, jint incX, jobject dY, jint incY) 
	{
		jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
		jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);
		
		//daxpy(int N, double DA, double* DX, int INCX, double* DY, int INCY);
		daxpy(n, dA, DX, incX, DY, incY);
	}

JNIEXPORT jdouble JNICALL Java_com_fhg_la_BlasLevel1_ddot
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX, jobject dY, jint incY) 
{
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	jdouble* DY	= (*env)->GetDirectBufferAddress(env, dY);

	return ddot(n, DX, incX, DY, incY);
}

JNIEXPORT jdouble JNICALL Java_com_fhg_la_BlasLevel1_dnrm2
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX) 
{
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	return dnrm2(n, DX, incX);
}

JNIEXPORT jdouble JNICALL Java_com_fhg_la_BlasLevel1_dasum
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX) 
{
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	return dasum(n, DX, incX);
}

JNIEXPORT jdouble JNICALL Java_com_fhg_la_BlasLevel1_idamax
  (JNIEnv *env, jclass this, jint n, jobject dX, jint incX)
{
	jdouble* DX	= (*env)->GetDirectBufferAddress(env, dX);
	return dasum(n, DX, incX);
}


