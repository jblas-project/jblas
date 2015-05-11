// --- BEGIN LICENSE BLOCK ---
/* 
 * Copyright (c) 2009, Mikio L. Braun
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the Technische Universität Berlin nor the
 *       names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// --- END LICENSE BLOCK ---
package org.jblas;

import org.jblas.util.Logger;

/**
 * Native BLAS and LAPACK functions.
 *
 * <p>The NativeBlas class contains the native BLAS and LAPACK functions. Each
 * Fortran function is mapped to a static method of this class. For each array argument,
 * an additional parameter is introduced which gives the offset from the beginning of
 * the passed array. In C, you would be able to pass a different pointer, but
 * in Java, you can only pass the whole array.</p>
 *
 * <p>Note that due to the way the JNI is usually implemented, the arrays are first
 * copied outside of the JVM before the function is called. This means that
 * functions whose runtime is linear in the amount of memory do usually not
 * run faster just because you are using a native implementation. This holds true
 * for most Level 1 BLAS routines (like vector addition), and unfortunately also
 * for most Level 2 BLAS routines (like matrix-vector multiplication). For these,
 * there exists a class JavaBlas which contains Java implementations.</p>
 *
 * <p>In LAPACK, there exist routines which require workspace to be allocated together
 * with a standard procedure for computing the size of these workspaces. jblas
 * automatically also generates wrappers for these routines with automatic
 * workspace allocation. These routines have the same name, but the workspace
 * arguments are removed.</p>
 *
 * <p>Finally, an example: The fortran routine<pre>
 * SUBROUTINE DAXPY(N,DA,DX,INCX,DY,INCY)
 *     DOUBLE PRECISION DA
 *     INTEGER INCX,INCY,N
 *     DOUBLE PRECISION DX(*),DY(*)
 * </pre>
 * becomes <pre>
 * public static native void daxpy(int n, double da, double[] dx, int dxIdx,
 *    int incx, double[] dy, int dyIdx, int incy);
 * </pre>
 */
public class NativeBlas {

  static {
    NativeBlasLibraryLoader.loadLibraryAndCheckErrors();
  }

  private static int[] intDummy = new int[1];
  private static double[] doubleDummy = new double[1];
  private static float[] floatDummy = new float[1];

  public static native void ccopy(int n, float[] cx, int cxIdx, int incx, float[] cy, int cyIdx, int incy);
  public static native void dcopy(int n, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy);
  public static native void scopy(int n, float[] sx, int sxIdx, int incx, float[] sy, int syIdx, int incy);
  public static native void zcopy(int n, double[] zx, int zxIdx, int incx, double[] zy, int zyIdx, int incy);
  public static native void cswap(int n, float[] cx, int cxIdx, int incx, float[] cy, int cyIdx, int incy);
  public static native void dswap(int n, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy);
  public static native void sswap(int n, float[] sx, int sxIdx, int incx, float[] sy, int syIdx, int incy);
  public static native void zswap(int n, double[] zx, int zxIdx, int incx, double[] zy, int zyIdx, int incy);
  public static native void caxpy(int n, ComplexFloat ca, float[] cx, int cxIdx, int incx, float[] cy, int cyIdx, int incy);
  public static native void daxpy(int n, double da, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy);
  public static native void saxpy(int n, float sa, float[] sx, int sxIdx, int incx, float[] sy, int syIdx, int incy);
  public static native void zaxpy(int n, ComplexDouble za, double[] zx, int zxIdx, int incx, double[] zy, int zyIdx, int incy);
  public static native void cscal(int n, ComplexFloat ca, float[] cx, int cxIdx, int incx);
  public static native void dscal(int n, double da, double[] dx, int dxIdx, int incx);
  public static native void sscal(int n, float sa, float[] sx, int sxIdx, int incx);
  public static native void zscal(int n, ComplexDouble za, double[] zx, int zxIdx, int incx);
  public static native void csscal(int n, float sa, float[] cx, int cxIdx, int incx);
  public static native void zdscal(int n, double da, double[] zx, int zxIdx, int incx);
  public static native ComplexFloat cdotc(int n, float[] cx, int cxIdx, int incx, float[] cy, int cyIdx, int incy);
  public static native ComplexFloat cdotu(int n, float[] cx, int cxIdx, int incx, float[] cy, int cyIdx, int incy);
  public static native double ddot(int n, double[] dx, int dxIdx, int incx, double[] dy, int dyIdx, int incy);
  public static native float sdot(int n, float[] sx, int sxIdx, int incx, float[] sy, int syIdx, int incy);
  public static native ComplexDouble zdotc(int n, double[] zx, int zxIdx, int incx, double[] zy, int zyIdx, int incy);
  public static native ComplexDouble zdotu(int n, double[] zx, int zxIdx, int incx, double[] zy, int zyIdx, int incy);
  public static native double dnrm2(int n, double[] x, int xIdx, int incx);
  public static native double dznrm2(int n, double[] x, int xIdx, int incx);
  public static native float scnrm2(int n, float[] x, int xIdx, int incx);
  public static native float snrm2(int n, float[] x, int xIdx, int incx);
  public static native double dasum(int n, double[] dx, int dxIdx, int incx);
  public static native double dzasum(int n, double[] zx, int zxIdx, int incx);
  public static native float sasum(int n, float[] sx, int sxIdx, int incx);
  public static native float scasum(int n, float[] cx, int cxIdx, int incx);
  public static native int icamax(int n, float[] cx, int cxIdx, int incx);
  public static native int idamax(int n, double[] dx, int dxIdx, int incx);
  public static native int isamax(int n, float[] sx, int sxIdx, int incx);
  public static native int izamax(int n, double[] zx, int zxIdx, int incx);
  public static native void cgemv(char trans, int m, int n, ComplexFloat alpha, float[] a, int aIdx, int lda, float[] x, int xIdx, int incx, ComplexFloat beta, float[] y, int yIdx, int incy);
  public static native void dgemv(char trans, int m, int n, double alpha, double[] a, int aIdx, int lda, double[] x, int xIdx, int incx, double beta, double[] y, int yIdx, int incy);
  public static native void sgemv(char trans, int m, int n, float alpha, float[] a, int aIdx, int lda, float[] x, int xIdx, int incx, float beta, float[] y, int yIdx, int incy);
  public static native void zgemv(char trans, int m, int n, ComplexDouble alpha, double[] a, int aIdx, int lda, double[] x, int xIdx, int incx, ComplexDouble beta, double[] y, int yIdx, int incy);
  public static native void cgerc(int m, int n, ComplexFloat alpha, float[] x, int xIdx, int incx, float[] y, int yIdx, int incy, float[] a, int aIdx, int lda);
  public static native void cgeru(int m, int n, ComplexFloat alpha, float[] x, int xIdx, int incx, float[] y, int yIdx, int incy, float[] a, int aIdx, int lda);
  public static native void dger(int m, int n, double alpha, double[] x, int xIdx, int incx, double[] y, int yIdx, int incy, double[] a, int aIdx, int lda);
  public static native void sger(int m, int n, float alpha, float[] x, int xIdx, int incx, float[] y, int yIdx, int incy, float[] a, int aIdx, int lda);
  public static native void zgerc(int m, int n, ComplexDouble alpha, double[] x, int xIdx, int incx, double[] y, int yIdx, int incy, double[] a, int aIdx, int lda);
  public static native void zgeru(int m, int n, ComplexDouble alpha, double[] x, int xIdx, int incx, double[] y, int yIdx, int incy, double[] a, int aIdx, int lda);
  public static native void cgemm(char transa, char transb, int m, int n, int k, ComplexFloat alpha, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, ComplexFloat beta, float[] c, int cIdx, int ldc);
  public static native void dgemm(char transa, char transb, int m, int n, int k, double alpha, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double beta, double[] c, int cIdx, int ldc);
  public static native void sgemm(char transa, char transb, int m, int n, int k, float alpha, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float beta, float[] c, int cIdx, int ldc);
  public static native void zgemm(char transa, char transb, int m, int n, int k, ComplexDouble alpha, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, ComplexDouble beta, double[] c, int cIdx, int ldc);
  public static native int dgesv(int n, int nrhs, double[] a, int aIdx, int lda, int[] ipiv, int ipivIdx, double[] b, int bIdx, int ldb);
  public static native int sgesv(int n, int nrhs, float[] a, int aIdx, int lda, int[] ipiv, int ipivIdx, float[] b, int bIdx, int ldb);
  public static native int dsysv(char uplo, int n, int nrhs, double[] a, int aIdx, int lda, int[] ipiv, int ipivIdx, double[] b, int bIdx, int ldb, double[] work, int workIdx, int lwork);
  public static int dsysv(char uplo, int n, int nrhs, double[] a, int aIdx, int lda, int[] ipiv, int ipivIdx, double[] b, int bIdx, int ldb) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dsysv(uplo, n, nrhs, doubleDummy, 0, lda, intDummy, 0, doubleDummy, 0, ldb, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dsysv(uplo, n, nrhs, a, aIdx, lda, ipiv, ipivIdx, b, bIdx, ldb, work, 0, lwork);
    return info;
  }

  public static native int ssysv(char uplo, int n, int nrhs, float[] a, int aIdx, int lda, int[] ipiv, int ipivIdx, float[] b, int bIdx, int ldb, float[] work, int workIdx, int lwork);
  public static int ssysv(char uplo, int n, int nrhs, float[] a, int aIdx, int lda, int[] ipiv, int ipivIdx, float[] b, int bIdx, int ldb) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = ssysv(uplo, n, nrhs, floatDummy, 0, lda, intDummy, 0, floatDummy, 0, ldb, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = ssysv(uplo, n, nrhs, a, aIdx, lda, ipiv, ipivIdx, b, bIdx, ldb, work, 0, lwork);
    return info;
  }

  public static native int dsyev(char jobz, char uplo, int n, double[] a, int aIdx, int lda, double[] w, int wIdx, double[] work, int workIdx, int lwork);
  public static int dsyev(char jobz, char uplo, int n, double[] a, int aIdx, int lda, double[] w, int wIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dsyev(jobz, uplo, n, doubleDummy, 0, lda, doubleDummy, 0, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dsyev(jobz, uplo, n, a, aIdx, lda, w, wIdx, work, 0, lwork);
    return info;
  }

  public static native int ssyev(char jobz, char uplo, int n, float[] a, int aIdx, int lda, float[] w, int wIdx, float[] work, int workIdx, int lwork);
  public static int ssyev(char jobz, char uplo, int n, float[] a, int aIdx, int lda, float[] w, int wIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = ssyev(jobz, uplo, n, floatDummy, 0, lda, floatDummy, 0, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = ssyev(jobz, uplo, n, a, aIdx, lda, w, wIdx, work, 0, lwork);
    return info;
  }

  public static native int dsyevd(char jobz, char uplo, int n, double[] a, int aIdx, int lda, double[] w, int wIdx, double[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int liwork);
  public static int dsyevd(char jobz, char uplo, int n, double[] a, int aIdx, int lda, double[] w, int wIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    int[] iwork = new int[1];
    int liwork;
    info = dsyevd(jobz, uplo, n, doubleDummy, 0, lda, doubleDummy, 0, work, 0, -1, iwork, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    liwork = (int) iwork[0]; iwork = new int[liwork];
    info = dsyevd(jobz, uplo, n, a, aIdx, lda, w, wIdx, work, 0, lwork, iwork, 0, liwork);
    return info;
  }

  public static native int dsyevr(char jobz, char range, char uplo, int n, double[] a, int aIdx, int lda, double vl, double vu, int il, int iu, double abstol, int[] m, int mIdx, double[] w, int wIdx, double[] z, int zIdx, int ldz, int[] isuppz, int isuppzIdx, double[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int liwork);
  public static int dsyevr(char jobz, char range, char uplo, int n, double[] a, int aIdx, int lda, double vl, double vu, int il, int iu, double abstol, int[] m, int mIdx, double[] w, int wIdx, double[] z, int zIdx, int ldz, int[] isuppz, int isuppzIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    int[] iwork = new int[1];
    int liwork;
    info = dsyevr(jobz, range, uplo, n, doubleDummy, 0, lda, vl, vu, il, iu, abstol, intDummy, 0, doubleDummy, 0, doubleDummy, 0, ldz, intDummy, 0, work, 0, -1, iwork, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    liwork = (int) iwork[0]; iwork = new int[liwork];
    info = dsyevr(jobz, range, uplo, n, a, aIdx, lda, vl, vu, il, iu, abstol, m, mIdx, w, wIdx, z, zIdx, ldz, isuppz, isuppzIdx, work, 0, lwork, iwork, 0, liwork);
    return info;
  }

  public static native int dsyevx(char jobz, char range, char uplo, int n, double[] a, int aIdx, int lda, double vl, double vu, int il, int iu, double abstol, int[] m, int mIdx, double[] w, int wIdx, double[] z, int zIdx, int ldz, double[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx);
  public static int dsyevx(char jobz, char range, char uplo, int n, double[] a, int aIdx, int lda, double vl, double vu, int il, int iu, double abstol, int[] m, int mIdx, double[] w, int wIdx, double[] z, int zIdx, int ldz, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dsyevx(jobz, range, uplo, n, doubleDummy, 0, lda, vl, vu, il, iu, abstol, intDummy, 0, doubleDummy, 0, doubleDummy, 0, ldz, work, 0, -1, intDummy, 0, intDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dsyevx(jobz, range, uplo, n, a, aIdx, lda, vl, vu, il, iu, abstol, m, mIdx, w, wIdx, z, zIdx, ldz, work, 0, lwork, iwork, iworkIdx, ifail, ifailIdx);
    return info;
  }

  public static native int ssyevd(char jobz, char uplo, int n, float[] a, int aIdx, int lda, float[] w, int wIdx, float[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int liwork);
  public static int ssyevd(char jobz, char uplo, int n, float[] a, int aIdx, int lda, float[] w, int wIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    int[] iwork = new int[1];
    int liwork;
    info = ssyevd(jobz, uplo, n, floatDummy, 0, lda, floatDummy, 0, work, 0, -1, iwork, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    liwork = (int) iwork[0]; iwork = new int[liwork];
    info = ssyevd(jobz, uplo, n, a, aIdx, lda, w, wIdx, work, 0, lwork, iwork, 0, liwork);
    return info;
  }

  public static native int ssyevr(char jobz, char range, char uplo, int n, float[] a, int aIdx, int lda, float vl, float vu, int il, int iu, float abstol, int[] m, int mIdx, float[] w, int wIdx, float[] z, int zIdx, int ldz, int[] isuppz, int isuppzIdx, float[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int liwork);
  public static int ssyevr(char jobz, char range, char uplo, int n, float[] a, int aIdx, int lda, float vl, float vu, int il, int iu, float abstol, int[] m, int mIdx, float[] w, int wIdx, float[] z, int zIdx, int ldz, int[] isuppz, int isuppzIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    int[] iwork = new int[1];
    int liwork;
    info = ssyevr(jobz, range, uplo, n, floatDummy, 0, lda, vl, vu, il, iu, abstol, intDummy, 0, floatDummy, 0, floatDummy, 0, ldz, intDummy, 0, work, 0, -1, iwork, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    liwork = (int) iwork[0]; iwork = new int[liwork];
    info = ssyevr(jobz, range, uplo, n, a, aIdx, lda, vl, vu, il, iu, abstol, m, mIdx, w, wIdx, z, zIdx, ldz, isuppz, isuppzIdx, work, 0, lwork, iwork, 0, liwork);
    return info;
  }

  public static native int ssyevx(char jobz, char range, char uplo, int n, float[] a, int aIdx, int lda, float vl, float vu, int il, int iu, float abstol, int[] m, int mIdx, float[] w, int wIdx, float[] z, int zIdx, int ldz, float[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx);
  public static int ssyevx(char jobz, char range, char uplo, int n, float[] a, int aIdx, int lda, float vl, float vu, int il, int iu, float abstol, int[] m, int mIdx, float[] w, int wIdx, float[] z, int zIdx, int ldz, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = ssyevx(jobz, range, uplo, n, floatDummy, 0, lda, vl, vu, il, iu, abstol, intDummy, 0, floatDummy, 0, floatDummy, 0, ldz, work, 0, -1, intDummy, 0, intDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = ssyevx(jobz, range, uplo, n, a, aIdx, lda, vl, vu, il, iu, abstol, m, mIdx, w, wIdx, z, zIdx, ldz, work, 0, lwork, iwork, iworkIdx, ifail, ifailIdx);
    return info;
  }

  public static native int dposv(char uplo, int n, int nrhs, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb);
  public static native int sposv(char uplo, int n, int nrhs, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb);
  public static native int cgeev(char jobvl, char jobvr, int n, float[] a, int aIdx, int lda, float[] w, int wIdx, float[] vl, int vlIdx, int ldvl, float[] vr, int vrIdx, int ldvr, float[] work, int workIdx, int lwork, float[] rwork, int rworkIdx);
  public static int cgeev(char jobvl, char jobvr, int n, float[] a, int aIdx, int lda, float[] w, int wIdx, float[] vl, int vlIdx, int ldvl, float[] vr, int vrIdx, int ldvr, float[] rwork, int rworkIdx) {
    int info;
    float[] work = new float[1*2];
    int lwork;
    info = cgeev(jobvl, jobvr, n, floatDummy, 0, lda, floatDummy, 0, floatDummy, 0, ldvl, floatDummy, 0, ldvr, work, 0, -1, floatDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork*2];
    info = cgeev(jobvl, jobvr, n, a, aIdx, lda, w, wIdx, vl, vlIdx, ldvl, vr, vrIdx, ldvr, work, 0, lwork, rwork, rworkIdx);
    return info;
  }

  public static native int dgeev(char jobvl, char jobvr, int n, double[] a, int aIdx, int lda, double[] wr, int wrIdx, double[] wi, int wiIdx, double[] vl, int vlIdx, int ldvl, double[] vr, int vrIdx, int ldvr, double[] work, int workIdx, int lwork);
  public static int dgeev(char jobvl, char jobvr, int n, double[] a, int aIdx, int lda, double[] wr, int wrIdx, double[] wi, int wiIdx, double[] vl, int vlIdx, int ldvl, double[] vr, int vrIdx, int ldvr) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dgeev(jobvl, jobvr, n, doubleDummy, 0, lda, doubleDummy, 0, doubleDummy, 0, doubleDummy, 0, ldvl, doubleDummy, 0, ldvr, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dgeev(jobvl, jobvr, n, a, aIdx, lda, wr, wrIdx, wi, wiIdx, vl, vlIdx, ldvl, vr, vrIdx, ldvr, work, 0, lwork);
    return info;
  }

  public static native int sgeev(char jobvl, char jobvr, int n, float[] a, int aIdx, int lda, float[] wr, int wrIdx, float[] wi, int wiIdx, float[] vl, int vlIdx, int ldvl, float[] vr, int vrIdx, int ldvr, float[] work, int workIdx, int lwork);
  public static int sgeev(char jobvl, char jobvr, int n, float[] a, int aIdx, int lda, float[] wr, int wrIdx, float[] wi, int wiIdx, float[] vl, int vlIdx, int ldvl, float[] vr, int vrIdx, int ldvr) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = sgeev(jobvl, jobvr, n, floatDummy, 0, lda, floatDummy, 0, floatDummy, 0, floatDummy, 0, ldvl, floatDummy, 0, ldvr, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = sgeev(jobvl, jobvr, n, a, aIdx, lda, wr, wrIdx, wi, wiIdx, vl, vlIdx, ldvl, vr, vrIdx, ldvr, work, 0, lwork);
    return info;
  }

  public static native int zgeev(char jobvl, char jobvr, int n, double[] a, int aIdx, int lda, double[] w, int wIdx, double[] vl, int vlIdx, int ldvl, double[] vr, int vrIdx, int ldvr, double[] work, int workIdx, int lwork, double[] rwork, int rworkIdx);
  public static int zgeev(char jobvl, char jobvr, int n, double[] a, int aIdx, int lda, double[] w, int wIdx, double[] vl, int vlIdx, int ldvl, double[] vr, int vrIdx, int ldvr, double[] rwork, int rworkIdx) {
    int info;
    double[] work = new double[1*2];
    int lwork;
    info = zgeev(jobvl, jobvr, n, doubleDummy, 0, lda, doubleDummy, 0, doubleDummy, 0, ldvl, doubleDummy, 0, ldvr, work, 0, -1, doubleDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork*2];
    info = zgeev(jobvl, jobvr, n, a, aIdx, lda, w, wIdx, vl, vlIdx, ldvl, vr, vrIdx, ldvr, work, 0, lwork, rwork, rworkIdx);
    return info;
  }

  public static native int dgetrf(int m, int n, double[] a, int aIdx, int lda, int[] ipiv, int ipivIdx);
  public static native int sgetrf(int m, int n, float[] a, int aIdx, int lda, int[] ipiv, int ipivIdx);
  public static native int dpotrf(char uplo, int n, double[] a, int aIdx, int lda);
  public static native int spotrf(char uplo, int n, float[] a, int aIdx, int lda);
  public static native int cgesvd(char jobu, char jobvt, int m, int n, float[] a, int aIdx, int lda, float[] s, int sIdx, float[] u, int uIdx, int ldu, float[] vt, int vtIdx, int ldvt, float[] work, int workIdx, int lwork, float[] rwork, int rworkIdx);
  public static int cgesvd(char jobu, char jobvt, int m, int n, float[] a, int aIdx, int lda, float[] s, int sIdx, float[] u, int uIdx, int ldu, float[] vt, int vtIdx, int ldvt, float[] rwork, int rworkIdx) {
    int info;
    float[] work = new float[1*2];
    int lwork;
    info = cgesvd(jobu, jobvt, m, n, floatDummy, 0, lda, floatDummy, 0, floatDummy, 0, ldu, floatDummy, 0, ldvt, work, 0, -1, floatDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork*2];
    info = cgesvd(jobu, jobvt, m, n, a, aIdx, lda, s, sIdx, u, uIdx, ldu, vt, vtIdx, ldvt, work, 0, lwork, rwork, rworkIdx);
    return info;
  }

  public static native int dgesvd(char jobu, char jobvt, int m, int n, double[] a, int aIdx, int lda, double[] s, int sIdx, double[] u, int uIdx, int ldu, double[] vt, int vtIdx, int ldvt, double[] work, int workIdx, int lwork);
  public static int dgesvd(char jobu, char jobvt, int m, int n, double[] a, int aIdx, int lda, double[] s, int sIdx, double[] u, int uIdx, int ldu, double[] vt, int vtIdx, int ldvt) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dgesvd(jobu, jobvt, m, n, doubleDummy, 0, lda, doubleDummy, 0, doubleDummy, 0, ldu, doubleDummy, 0, ldvt, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dgesvd(jobu, jobvt, m, n, a, aIdx, lda, s, sIdx, u, uIdx, ldu, vt, vtIdx, ldvt, work, 0, lwork);
    return info;
  }

  public static native int sgesvd(char jobu, char jobvt, int m, int n, float[] a, int aIdx, int lda, float[] s, int sIdx, float[] u, int uIdx, int ldu, float[] vt, int vtIdx, int ldvt, float[] work, int workIdx, int lwork);
  public static int sgesvd(char jobu, char jobvt, int m, int n, float[] a, int aIdx, int lda, float[] s, int sIdx, float[] u, int uIdx, int ldu, float[] vt, int vtIdx, int ldvt) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = sgesvd(jobu, jobvt, m, n, floatDummy, 0, lda, floatDummy, 0, floatDummy, 0, ldu, floatDummy, 0, ldvt, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = sgesvd(jobu, jobvt, m, n, a, aIdx, lda, s, sIdx, u, uIdx, ldu, vt, vtIdx, ldvt, work, 0, lwork);
    return info;
  }

  public static native int zgesvd(char jobu, char jobvt, int m, int n, double[] a, int aIdx, int lda, double[] s, int sIdx, double[] u, int uIdx, int ldu, double[] vt, int vtIdx, int ldvt, double[] work, int workIdx, int lwork, double[] rwork, int rworkIdx);
  public static int zgesvd(char jobu, char jobvt, int m, int n, double[] a, int aIdx, int lda, double[] s, int sIdx, double[] u, int uIdx, int ldu, double[] vt, int vtIdx, int ldvt, double[] rwork, int rworkIdx) {
    int info;
    double[] work = new double[1*2];
    int lwork;
    info = zgesvd(jobu, jobvt, m, n, doubleDummy, 0, lda, doubleDummy, 0, doubleDummy, 0, ldu, doubleDummy, 0, ldvt, work, 0, -1, doubleDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork*2];
    info = zgesvd(jobu, jobvt, m, n, a, aIdx, lda, s, sIdx, u, uIdx, ldu, vt, vtIdx, ldvt, work, 0, lwork, rwork, rworkIdx);
    return info;
  }

  public static native int dsygvd(int itype, char jobz, char uplo, int n, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double[] w, int wIdx, double[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int liwork);
  public static int dsygvd(int itype, char jobz, char uplo, int n, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double[] w, int wIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    int[] iwork = new int[1];
    int liwork;
    info = dsygvd(itype, jobz, uplo, n, doubleDummy, 0, lda, doubleDummy, 0, ldb, doubleDummy, 0, work, 0, -1, iwork, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    liwork = (int) iwork[0]; iwork = new int[liwork];
    info = dsygvd(itype, jobz, uplo, n, a, aIdx, lda, b, bIdx, ldb, w, wIdx, work, 0, lwork, iwork, 0, liwork);
    return info;
  }

  public static native int ssygvd(int itype, char jobz, char uplo, int n, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float[] w, int wIdx, float[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int liwork);
  public static int ssygvd(int itype, char jobz, char uplo, int n, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float[] w, int wIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    int[] iwork = new int[1];
    int liwork;
    info = ssygvd(itype, jobz, uplo, n, floatDummy, 0, lda, floatDummy, 0, ldb, floatDummy, 0, work, 0, -1, iwork, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    liwork = (int) iwork[0]; iwork = new int[liwork];
    info = ssygvd(itype, jobz, uplo, n, a, aIdx, lda, b, bIdx, ldb, w, wIdx, work, 0, lwork, iwork, 0, liwork);
    return info;
  }

  public static native int dgelsd(int m, int n, int nrhs, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double[] s, int sIdx, double rcond, int[] rank, int rankIdx, double[] work, int workIdx, int lwork, int[] iwork, int iworkIdx);
  public static int dgelsd(int m, int n, int nrhs, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double[] s, int sIdx, double rcond, int[] rank, int rankIdx, int[] iwork, int iworkIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dgelsd(m, n, nrhs, doubleDummy, 0, lda, doubleDummy, 0, ldb, doubleDummy, 0, rcond, intDummy, 0, work, 0, -1, intDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dgelsd(m, n, nrhs, a, aIdx, lda, b, bIdx, ldb, s, sIdx, rcond, rank, rankIdx, work, 0, lwork, iwork, iworkIdx);
    return info;
  }

  public static native int sgelsd(int m, int n, int nrhs, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float[] s, int sIdx, float rcond, int[] rank, int rankIdx, float[] work, int workIdx, int lwork, int[] iwork, int iworkIdx);
  public static int sgelsd(int m, int n, int nrhs, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float[] s, int sIdx, float rcond, int[] rank, int rankIdx, int[] iwork, int iworkIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = sgelsd(m, n, nrhs, floatDummy, 0, lda, floatDummy, 0, ldb, floatDummy, 0, rcond, intDummy, 0, work, 0, -1, intDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = sgelsd(m, n, nrhs, a, aIdx, lda, b, bIdx, ldb, s, sIdx, rcond, rank, rankIdx, work, 0, lwork, iwork, iworkIdx);
    return info;
  }

  public static native int ilaenv(int ispec, String name, String opts, int n1, int n2, int n3, int n4);
  public static native int dgeqrf(int m, int n, double[] a, int aIdx, int lda, double[] tau, int tauIdx, double[] work, int workIdx, int lwork);
  public static int dgeqrf(int m, int n, double[] a, int aIdx, int lda, double[] tau, int tauIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dgeqrf(m, n, doubleDummy, 0, lda, doubleDummy, 0, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dgeqrf(m, n, a, aIdx, lda, tau, tauIdx, work, 0, lwork);
    return info;
  }

  public static native int sgeqrf(int m, int n, float[] a, int aIdx, int lda, float[] tau, int tauIdx, float[] work, int workIdx, int lwork);
  public static int sgeqrf(int m, int n, float[] a, int aIdx, int lda, float[] tau, int tauIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = sgeqrf(m, n, floatDummy, 0, lda, floatDummy, 0, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = sgeqrf(m, n, a, aIdx, lda, tau, tauIdx, work, 0, lwork);
    return info;
  }

  public static native int dormqr(char side, char trans, int m, int n, int k, double[] a, int aIdx, int lda, double[] tau, int tauIdx, double[] c, int cIdx, int ldc, double[] work, int workIdx, int lwork);
  public static int dormqr(char side, char trans, int m, int n, int k, double[] a, int aIdx, int lda, double[] tau, int tauIdx, double[] c, int cIdx, int ldc) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dormqr(side, trans, m, n, k, doubleDummy, 0, lda, doubleDummy, 0, doubleDummy, 0, ldc, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dormqr(side, trans, m, n, k, a, aIdx, lda, tau, tauIdx, c, cIdx, ldc, work, 0, lwork);
    return info;
  }

  public static native int sormqr(char side, char trans, int m, int n, int k, float[] a, int aIdx, int lda, float[] tau, int tauIdx, float[] c, int cIdx, int ldc, float[] work, int workIdx, int lwork);
  public static int sormqr(char side, char trans, int m, int n, int k, float[] a, int aIdx, int lda, float[] tau, int tauIdx, float[] c, int cIdx, int ldc) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = sormqr(side, trans, m, n, k, floatDummy, 0, lda, floatDummy, 0, floatDummy, 0, ldc, work, 0, -1);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = sormqr(side, trans, m, n, k, a, aIdx, lda, tau, tauIdx, c, cIdx, ldc, work, 0, lwork);
    return info;
  }

  public static native int dsygvx(int itype, char jobz, char range, char uplo, int n, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double vl, double vu, int il, int iu, double abstol, int[] m, int mIdx, double[] w, int wIdx, double[] z, int zIdx, int ldz, double[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx);
  public static int dsygvx(int itype, char jobz, char range, char uplo, int n, double[] a, int aIdx, int lda, double[] b, int bIdx, int ldb, double vl, double vu, int il, int iu, double abstol, int[] m, int mIdx, double[] w, int wIdx, double[] z, int zIdx, int ldz, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx) {
    int info;
    double[] work = new double[1];
    int lwork;
    info = dsygvx(itype, jobz, range, uplo, n, doubleDummy, 0, lda, doubleDummy, 0, ldb, vl, vu, il, iu, abstol, intDummy, 0, doubleDummy, 0, doubleDummy, 0, ldz, work, 0, -1, intDummy, 0, intDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new double[lwork];
    info = dsygvx(itype, jobz, range, uplo, n, a, aIdx, lda, b, bIdx, ldb, vl, vu, il, iu, abstol, m, mIdx, w, wIdx, z, zIdx, ldz, work, 0, lwork, iwork, iworkIdx, ifail, ifailIdx);
    return info;
  }

  public static native int ssygvx(int itype, char jobz, char range, char uplo, int n, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float vl, float vu, int il, int iu, float abstol, int[] m, int mIdx, float[] w, int wIdx, float[] z, int zIdx, int ldz, float[] work, int workIdx, int lwork, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx);
  public static int ssygvx(int itype, char jobz, char range, char uplo, int n, float[] a, int aIdx, int lda, float[] b, int bIdx, int ldb, float vl, float vu, int il, int iu, float abstol, int[] m, int mIdx, float[] w, int wIdx, float[] z, int zIdx, int ldz, int[] iwork, int iworkIdx, int[] ifail, int ifailIdx) {
    int info;
    float[] work = new float[1];
    int lwork;
    info = ssygvx(itype, jobz, range, uplo, n, floatDummy, 0, lda, floatDummy, 0, ldb, vl, vu, il, iu, abstol, intDummy, 0, floatDummy, 0, floatDummy, 0, ldz, work, 0, -1, intDummy, 0, intDummy, 0);
    if (info != 0)
      return info;
    lwork = (int) work[0]; work = new float[lwork];
    info = ssygvx(itype, jobz, range, uplo, n, a, aIdx, lda, b, bIdx, ldb, vl, vu, il, iu, abstol, m, mIdx, w, wIdx, z, zIdx, ldz, work, 0, lwork, iwork, iworkIdx, ifail, ifailIdx);
    return info;
  }


}
