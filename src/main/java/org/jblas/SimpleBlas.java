// --- BEGIN LICENSE BLOCK ---
/* 
 * Copyright (c) 2009-2013, Mikio L. Braun
 *               2011, Nicolas Oury
 *               2013, Alexander Sehlström
 *
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

import org.jblas.exceptions.*;

import static org.jblas.util.Functions.*;

/**
 * This class provides a cleaner direct interface to the BLAS routines by
 * extracting the parameters of the matrices from the matrices itself.
 * <p/>
 * For example, you can just pass the vector and do not have to pass the length,
 * corresponding DoubleBuffer, offset and step size explicitly.
 * <p/>
 * Currently, all the general matrix routines are implemented.
 */
public class SimpleBlas {
	/***************************************************************************
	 * BLAS Level 1
	 */

	/**
	 * Compute x <-> y (swap two matrices)
	 */
	public static DoubleMatrix swap(DoubleMatrix x, DoubleMatrix y) {
		//NativeBlas.dswap(x.length, x.data, 0, 1, y.data, 0, 1);
		JavaBlas.rswap(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}

	/**
	 * Compute x <- alpha * x (scale a matrix)
	 */
	public static DoubleMatrix scal(double alpha, DoubleMatrix x) {
		NativeBlas.dscal(x.length, alpha, x.data, 0, 1);
		return x;
	}

	public static ComplexDoubleMatrix scal(ComplexDouble alpha, ComplexDoubleMatrix x) {
		NativeBlas.zscal(x.length, alpha, x.data, 0, 1);
		return x;
	}

	/**
	 * Compute y <- x (copy a matrix)
	 */
	public static DoubleMatrix copy(DoubleMatrix x, DoubleMatrix y) {
		//NativeBlas.dcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		JavaBlas.rcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}

	public static ComplexDoubleMatrix copy(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		NativeBlas.zcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}

	/**
	 * Compute y <- alpha * x + y (elementwise addition)
	 */
	public static DoubleMatrix axpy(double da, DoubleMatrix dx, DoubleMatrix dy) {
		//NativeBlas.daxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		JavaBlas.raxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);

		return dy;
	}

	public static ComplexDoubleMatrix axpy(ComplexDouble da, ComplexDoubleMatrix dx, ComplexDoubleMatrix dy) {
		NativeBlas.zaxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		return dy;
	}

	/**
	 * Compute x^T * y (dot product)
	 */
	public static double dot(DoubleMatrix x, DoubleMatrix y) {
		//return NativeBlas.ddot(x.length, x.data, 0, 1, y.data, 0, 1);
		return JavaBlas.rdot(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/**
	 * Compute x^T * y (dot product)
	 */
	public static ComplexDouble dotc(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		return NativeBlas.zdotc(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/**
	 * Compute x^T * y (dot product)
	 */
	public static ComplexDouble dotu(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		return NativeBlas.zdotu(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/**
	 * Compute || x ||_2 (2-norm)
	 */
	public static double nrm2(DoubleMatrix x) {
		return NativeBlas.dnrm2(x.length, x.data, 0, 1);
	}

	public static double nrm2(ComplexDoubleMatrix x) {
		return NativeBlas.dznrm2(x.length, x.data, 0, 1);
	}

	/**
	 * Compute || x ||_1 (1-norm, sum of absolute values)
	 */
	public static double asum(DoubleMatrix x) {
		return NativeBlas.dasum(x.length, x.data, 0, 1);
	}

	public static double asum(ComplexDoubleMatrix x) {
		return NativeBlas.dzasum(x.length, x.data, 0, 1);
	}

	/**
	 * Compute index of element with largest absolute value (index of absolute
	 * value maximum)
	 */
	public static int iamax(DoubleMatrix x) {
		return NativeBlas.idamax(x.length, x.data, 0, 1) - 1;
	}

	/**
	 * Compute index of element with largest absolute value (complex version).
	 *
	 * @param x matrix
	 * @return index of element with largest absolute value.
	 */
	public static int iamax(ComplexDoubleMatrix x) {
		return NativeBlas.izamax(x.length, x.data, 0, 1) - 1;
	}

	/***************************************************************************
	 * BLAS Level 2
	 */

	/**
	 * Compute y <- alpha*op(a)*x + beta * y (general matrix vector
	 * multiplication)
	 */
	public static DoubleMatrix gemv(double alpha, DoubleMatrix a,
			DoubleMatrix x, double beta, DoubleMatrix y) {
		if (false) {
			NativeBlas.dgemv('N', a.rows, a.columns, alpha, a.data, 0, a.rows, x.data, 0,
					1, beta, y.data, 0, 1);
		} else {
			if (beta == 0.0) {
				for (int i = 0; i < y.length; i++)
					y.data[i] = 0.0;

				for (int j = 0; j < a.columns; j++) {
					double xj = x.get(j);
					if (xj != 0.0) {
						for (int i = 0; i < a.rows; i++)
							y.data[i] += a.get(i, j) * xj;
					}
				}
			} else {
				for (int j = 0; j < a.columns; j++) {
					double byj = beta * y.data[j];
					double xj = x.get(j);
					for (int i = 0; i < a.rows; i++)
						y.data[j] = a.get(i, j) * xj + byj;
				}
			}
		}
		return y;
	}

	/**
	 * Compute A <- alpha * x * y^T + A (general rank-1 update)
	 */
	public static DoubleMatrix ger(double alpha, DoubleMatrix x,
			DoubleMatrix y, DoubleMatrix a) {
		NativeBlas.dger(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/**
	 * Compute A <- alpha * x * y^T + A (general rank-1 update)
	 */
	public static ComplexDoubleMatrix geru(ComplexDouble alpha, ComplexDoubleMatrix x,
			ComplexDoubleMatrix y, ComplexDoubleMatrix a) {
		NativeBlas.zgeru(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/**
	 * Compute A <- alpha * x * y^H + A (general rank-1 update)
	 */
	public static ComplexDoubleMatrix gerc(ComplexDouble alpha, ComplexDoubleMatrix x,
			ComplexDoubleMatrix y, ComplexDoubleMatrix a) {
		NativeBlas.zgerc(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/***************************************************************************
	 * BLAS Level 3
	 */

	/**
	 * Compute c <- a*b + beta * c (general matrix matrix
	 * multiplication)
	 */
	public static DoubleMatrix gemm(double alpha, DoubleMatrix a,
			DoubleMatrix b, double beta, DoubleMatrix c) {
		NativeBlas.dgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	public static ComplexDoubleMatrix gemm(ComplexDouble alpha, ComplexDoubleMatrix a,
			ComplexDoubleMatrix b, ComplexDouble beta, ComplexDoubleMatrix c) {
		NativeBlas.zgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	/***************************************************************************
	 * LAPACK
	 */

	public static DoubleMatrix gesv(DoubleMatrix a, int[] ipiv,
			DoubleMatrix b) {
		int info = NativeBlas.dgesv(a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows);
		checkInfo("DGESV", info);

		if (info > 0)
			throw new LapackException("DGESV",
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}

//STOP

	private static void checkInfo(String name, int info) {
		if (info < -1)
			throw new LapackArgumentException(name, info);
	}
//START

	public static DoubleMatrix sysv(char uplo, DoubleMatrix a, int[] ipiv,
			DoubleMatrix b) {
		int info = NativeBlas.dsysv(uplo, a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows);
		checkInfo("SYSV", info);

		if (info > 0)
			throw new LapackSingularityException("SYV",
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}

	public static int syev(char jobz, char uplo, DoubleMatrix a, DoubleMatrix w) {
		int info = NativeBlas.dsyev(jobz, uplo, a.rows, a.data, 0, a.rows, w.data, 0);

		if (info > 0)
			throw new LapackConvergenceException("SYEV",
					"Eigenvalues could not be computed " + info
					+ " off-diagonal elements did not converge");

		return info;
	}

	public static int syevx(char jobz, char range, char uplo, DoubleMatrix a,
			double vl, double vu, int il, int iu, double abstol,
			DoubleMatrix w, DoubleMatrix z) {
		int n = a.rows;
		int[] iwork = new int[5 * n];
		int[] ifail = new int[n];
		int[] m = new int[1];
		int info;

		info = NativeBlas.dsyevx(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu, il,
				iu, abstol, m, 0, w.data, 0, z.data, 0, z.rows, iwork, 0, ifail, 0);

		if (info > 0) {
			StringBuilder msg = new StringBuilder();
			msg
			.append("Not all eigenvalues converged. Non-converging eigenvalues were: ");
			for (int i = 0; i < info; i++) {
				if (i > 0)
					msg.append(", ");
				msg.append(ifail[i]);
			}
			msg.append(".");
			throw new LapackConvergenceException("SYEVX", msg.toString());
		}

		return info;
	}

	public static int syevd(char jobz, char uplo, DoubleMatrix A,
			DoubleMatrix w) {
		int n = A.rows;

		int info = NativeBlas.dsyevd(jobz, uplo, n, A.data, 0, A.rows, w.data, 0);

		if (info > 0)
			throw new LapackConvergenceException("SYEVD", "Not all eigenvalues converged.");

		return info;
	}

	public static int syevr(char jobz, char range, char uplo, DoubleMatrix a,
			double vl, double vu, int il, int iu, double abstol,
			DoubleMatrix w, DoubleMatrix z, int[] isuppz) {
		int n = a.rows;
		int[] m = new int[1];

		int info = NativeBlas.dsyevr(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu,
				il, iu, abstol, m, 0, w.data, 0, z.data, 0, z.rows, isuppz, 0);

		checkInfo("SYEVR", info);

		return info;
	}

	public static void posv(char uplo, DoubleMatrix A, DoubleMatrix B) {
		int n = A.rows;
		int nrhs = B.columns;
		int info = NativeBlas.dposv(uplo, n, nrhs, A.data, 0, A.rows, B.data, 0,
				B.rows);
		checkInfo("DPOSV", info);
		if (info > 0)
			throw new LapackArgumentException("DPOSV",
					"Leading minor of order i of A is not positive definite.");
	}

	public static int geev(char jobvl, char jobvr, DoubleMatrix A,
			DoubleMatrix WR, DoubleMatrix WI, DoubleMatrix VL, DoubleMatrix VR) {
		int info = NativeBlas.dgeev(jobvl, jobvr, A.rows, A.data, 0, A.rows, WR.data, 0,
				WI.data, 0, VL.data, 0, VL.rows, VR.data, 0, VR.rows);
		if (info > 0)
			throw new LapackConvergenceException("DGEEV", "First " + info + " eigenvalues have not converged.");
		return info;
	}

	public static int sygvd(int itype, char jobz, char uplo, DoubleMatrix A, DoubleMatrix B, DoubleMatrix W) {
		int info = NativeBlas.dsygvd(itype, jobz, uplo, A.rows, A.data, 0, A.rows, B.data, 0, B.rows, W.data, 0);
		if (info == 0)
			return 0;
		else {
			if (info < 0)
				throw new LapackArgumentException("DSYGVD", -info);
			if (info <= A.rows && jobz == 'N')
				throw new LapackConvergenceException("DSYGVD", info + " off-diagonal elements did not converge to 0.");
			if (info <= A.rows && jobz == 'V')
				throw new LapackException("DSYGVD", "Failed to compute an eigenvalue while working on a sub-matrix  " + info + ".");
			else
				throw new LapackException("DSYGVD", "The leading minor of order " + (info - A.rows) + " of B is not positive definite.");
		}
	}
	
	public static int sygvx(int itype, char jobz, char range, char uplo, DoubleMatrix A,
			DoubleMatrix B, double vl, double vu, int il, int iu, double abstol,
			int[] m, DoubleMatrix W, DoubleMatrix Z) {
		int[] iwork = new int[1];
		int[] ifail = new int[1];
		int info = NativeBlas.dsygvx(itype, jobz, range, uplo, A.rows, A.data, 0, A.rows, B.data, 0, B.rows, vl, vu, il, iu, abstol, m, 0, W.data, 0, Z.data, 0, Z.rows, iwork, 0, ifail, 0);
		if (info == 0) {
			return 0;
		} else {
			if (info < 0) {
				throw new LapackArgumentException("DSYGVX", -info);
			}
			if(info <= A.rows) {
				throw new LapackConvergenceException("DSYGVX", info + " eigenvectors failed to converge");
			} else {
				throw new LapackException("DSYGVX", "The leading minor order " + (info - A.rows) + " of B is not postivie definite.");
			}
		}
	}

	/**
	 * Generalized Least Squares via *GELSD.
	 *
	 * Note that B must be padded to contain the solution matrix. This occurs when A has fewer rows
	 * than columns.
	 *
	 * For example: in A * X = B, A is (m,n), X is (n,k) and B is (m,k). Now if m < n, since B is overwritten to contain
	 * the solution (in classical LAPACK style), B needs to be padded to be an (n,k) matrix.
	 *
	 * Likewise, if m > n, the solution consists only of the first n rows of B.
	 *
	 * @param A an (m,n) matrix
	 * @param B an (max(m,n), k) matrix (well, at least)
	 */
	public static void gelsd(DoubleMatrix A, DoubleMatrix B) {
		int m = A.rows;
		int n = A.columns;
		int nrhs = B.columns;
		int minmn = min(m, n);
		int maxmn = max(m, n);

		if (B.rows < maxmn) {
			throw new SizeException("Result matrix B must be padded to contain the solution matrix X!");
		}

		//int smlsiz = NativeBlas.ilaenv(9, "DGELSD", "", m, n, nrhs, -1);
		int smlsiz = 25;
		int nlvl = max(0, (int) log2(minmn/ (smlsiz+1)) + 1);

		//System.err.printf("GELSD\n");
		//System.err.printf("m = %d, n = %d, nrhs = %d\n", m, n, nrhs);
		//System.err.printf("smlsiz = %d, nlvl = %d\n", smlsiz, nlvl);
		//System.err.printf("iwork size = %d\n", 3 * minmn * nlvl + 11 * minmn);

		int[] iwork = new int[3 * minmn * nlvl + 11 * minmn];
		double[] s = new double[minmn];
		int[] rank = new int[1];
		int info = NativeBlas.dgelsd(m, n, nrhs, A.data, 0, m, B.data, 0, B.rows, s, 0, -1, rank, 0, iwork, 0);
		if (info == 0) {
			return;
		} else if (info < 0) {
			throw new LapackArgumentException("DGESD", -info);
		} else if (info > 0) {
			throw new LapackConvergenceException("DGESD", info + " off-diagonal elements of an intermediat bidiagonal form did not converge to 0.");
		}
	}

	public static void geqrf(DoubleMatrix A, DoubleMatrix tau) {
		int info = NativeBlas.dgeqrf(A.rows, A.columns, A.data, 0, A.rows, tau.data, 0);
		checkInfo("GEQRF", info);
	}

	public static void ormqr(char side, char trans, DoubleMatrix A, DoubleMatrix tau, DoubleMatrix C) {
		int k = tau.length;
		int info = NativeBlas.dormqr(side, trans, C.rows, C.columns, k, A.data, 0, A.rows, tau.data, 0, C.data, 0, C.rows);
		checkInfo("ORMQR", info);
	}

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
	/***************************************************************************
	 * BLAS Level 1
	 */

	/**
	 * Compute x <-> y (swap two matrices)
	 */
	public static FloatMatrix swap(FloatMatrix x, FloatMatrix y) {
		//NativeBlas.sswap(x.length, x.data, 0, 1, y.data, 0, 1);
		JavaBlas.rswap(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}

	/**
	 * Compute x <- alpha * x (scale a matrix)
	 */
	public static FloatMatrix scal(float alpha, FloatMatrix x) {
		NativeBlas.sscal(x.length, alpha, x.data, 0, 1);
		return x;
	}

	public static ComplexFloatMatrix scal(ComplexFloat alpha, ComplexFloatMatrix x) {
		NativeBlas.cscal(x.length, alpha, x.data, 0, 1);
		return x;
	}

	/**
	 * Compute y <- x (copy a matrix)
	 */
	public static FloatMatrix copy(FloatMatrix x, FloatMatrix y) {
		//NativeBlas.scopy(x.length, x.data, 0, 1, y.data, 0, 1);
		JavaBlas.rcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}

	public static ComplexFloatMatrix copy(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		NativeBlas.ccopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}

	/**
	 * Compute y <- alpha * x + y (elementwise addition)
	 */
	public static FloatMatrix axpy(float da, FloatMatrix dx, FloatMatrix dy) {
		//NativeBlas.saxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		JavaBlas.raxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);

		return dy;
	}

	public static ComplexFloatMatrix axpy(ComplexFloat da, ComplexFloatMatrix dx, ComplexFloatMatrix dy) {
		NativeBlas.caxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		return dy;
	}

	/**
	 * Compute x^T * y (dot product)
	 */
	public static float dot(FloatMatrix x, FloatMatrix y) {
		//return NativeBlas.sdot(x.length, x.data, 0, 1, y.data, 0, 1);
		return JavaBlas.rdot(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/**
	 * Compute x^T * y (dot product)
	 */
	public static ComplexFloat dotc(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		return NativeBlas.cdotc(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/**
	 * Compute x^T * y (dot product)
	 */
	public static ComplexFloat dotu(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		return NativeBlas.cdotu(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/**
	 * Compute || x ||_2 (2-norm)
	 */
	public static float nrm2(FloatMatrix x) {
		return NativeBlas.snrm2(x.length, x.data, 0, 1);
	}

	public static float nrm2(ComplexFloatMatrix x) {
		return NativeBlas.scnrm2(x.length, x.data, 0, 1);
	}

	/**
	 * Compute || x ||_1 (1-norm, sum of absolute values)
	 */
	public static float asum(FloatMatrix x) {
		return NativeBlas.sasum(x.length, x.data, 0, 1);
	}

	public static float asum(ComplexFloatMatrix x) {
		return NativeBlas.scasum(x.length, x.data, 0, 1);
	}

	/**
	 * Compute index of element with largest absolute value (index of absolute
	 * value maximum)
	 */
	public static int iamax(FloatMatrix x) {
		return NativeBlas.isamax(x.length, x.data, 0, 1) - 1;
	}

	/**
	 * Compute index of element with largest absolute value (complex version).
	 *
	 * @param x matrix
	 * @return index of element with largest absolute value.
	 */
	public static int iamax(ComplexFloatMatrix x) {
		return NativeBlas.icamax(x.length, x.data, 0, 1) - 1;
	}

	/***************************************************************************
	 * BLAS Level 2
	 */

	/**
	 * Compute y <- alpha*op(a)*x + beta * y (general matrix vector
	 * multiplication)
	 */
	public static FloatMatrix gemv(float alpha, FloatMatrix a,
			FloatMatrix x, float beta, FloatMatrix y) {
		if (false) {
			NativeBlas.sgemv('N', a.rows, a.columns, alpha, a.data, 0, a.rows, x.data, 0,
					1, beta, y.data, 0, 1);
		} else {
			if (beta == 0.0f) {
				for (int i = 0; i < y.length; i++)
					y.data[i] = 0.0f;

				for (int j = 0; j < a.columns; j++) {
					float xj = x.get(j);
					if (xj != 0.0f) {
						for (int i = 0; i < a.rows; i++)
							y.data[i] += a.get(i, j) * xj;
					}
				}
			} else {
				for (int j = 0; j < a.columns; j++) {
					float byj = beta * y.data[j];
					float xj = x.get(j);
					for (int i = 0; i < a.rows; i++)
						y.data[j] = a.get(i, j) * xj + byj;
				}
			}
		}
		return y;
	}

	/**
	 * Compute A <- alpha * x * y^T + A (general rank-1 update)
	 */
	public static FloatMatrix ger(float alpha, FloatMatrix x,
			FloatMatrix y, FloatMatrix a) {
		NativeBlas.sger(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/**
	 * Compute A <- alpha * x * y^T + A (general rank-1 update)
	 */
	public static ComplexFloatMatrix geru(ComplexFloat alpha, ComplexFloatMatrix x,
			ComplexFloatMatrix y, ComplexFloatMatrix a) {
		NativeBlas.cgeru(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/**
	 * Compute A <- alpha * x * y^H + A (general rank-1 update)
	 */
	public static ComplexFloatMatrix gerc(ComplexFloat alpha, ComplexFloatMatrix x,
			ComplexFloatMatrix y, ComplexFloatMatrix a) {
		NativeBlas.cgerc(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/***************************************************************************
	 * BLAS Level 3
	 */

	/**
	 * Compute c <- a*b + beta * c (general matrix matrix
	 * multiplication)
	 */
	public static FloatMatrix gemm(float alpha, FloatMatrix a,
			FloatMatrix b, float beta, FloatMatrix c) {
		NativeBlas.sgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	public static ComplexFloatMatrix gemm(ComplexFloat alpha, ComplexFloatMatrix a,
			ComplexFloatMatrix b, ComplexFloat beta, ComplexFloatMatrix c) {
		NativeBlas.cgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	/***************************************************************************
	 * LAPACK
	 */

	public static FloatMatrix gesv(FloatMatrix a, int[] ipiv,
			FloatMatrix b) {
		int info = NativeBlas.sgesv(a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows);
		checkInfo("DGESV", info);

		if (info > 0)
			throw new LapackException("DGESV",
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}


	public static FloatMatrix sysv(char uplo, FloatMatrix a, int[] ipiv,
			FloatMatrix b) {
		int info = NativeBlas.ssysv(uplo, a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows);
		checkInfo("SYSV", info);

		if (info > 0)
			throw new LapackSingularityException("SYV",
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}

	public static int syev(char jobz, char uplo, FloatMatrix a, FloatMatrix w) {
		int info = NativeBlas.ssyev(jobz, uplo, a.rows, a.data, 0, a.rows, w.data, 0);

		if (info > 0)
			throw new LapackConvergenceException("SYEV",
					"Eigenvalues could not be computed " + info
					+ " off-diagonal elements did not converge");

		return info;
	}

	public static int syevx(char jobz, char range, char uplo, FloatMatrix a,
			float vl, float vu, int il, int iu, float abstol,
			FloatMatrix w, FloatMatrix z) {
		int n = a.rows;
		int[] iwork = new int[5 * n];
		int[] ifail = new int[n];
		int[] m = new int[1];
		int info;

		info = NativeBlas.ssyevx(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu, il,
				iu, abstol, m, 0, w.data, 0, z.data, 0, z.rows, iwork, 0, ifail, 0);

		if (info > 0) {
			StringBuilder msg = new StringBuilder();
			msg
			.append("Not all eigenvalues converged. Non-converging eigenvalues were: ");
			for (int i = 0; i < info; i++) {
				if (i > 0)
					msg.append(", ");
				msg.append(ifail[i]);
			}
			msg.append(".");
			throw new LapackConvergenceException("SYEVX", msg.toString());
		}

		return info;
	}

	public static int syevd(char jobz, char uplo, FloatMatrix A,
			FloatMatrix w) {
		int n = A.rows;

		int info = NativeBlas.ssyevd(jobz, uplo, n, A.data, 0, A.rows, w.data, 0);

		if (info > 0)
			throw new LapackConvergenceException("SYEVD", "Not all eigenvalues converged.");

		return info;
	}

	public static int syevr(char jobz, char range, char uplo, FloatMatrix a,
			float vl, float vu, int il, int iu, float abstol,
			FloatMatrix w, FloatMatrix z, int[] isuppz) {
		int n = a.rows;
		int[] m = new int[1];

		int info = NativeBlas.ssyevr(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu,
				il, iu, abstol, m, 0, w.data, 0, z.data, 0, z.rows, isuppz, 0);

		checkInfo("SYEVR", info);

		return info;
	}

	public static void posv(char uplo, FloatMatrix A, FloatMatrix B) {
		int n = A.rows;
		int nrhs = B.columns;
		int info = NativeBlas.sposv(uplo, n, nrhs, A.data, 0, A.rows, B.data, 0,
				B.rows);
		checkInfo("DPOSV", info);
		if (info > 0)
			throw new LapackArgumentException("DPOSV",
					"Leading minor of order i of A is not positive definite.");
	}

	public static int geev(char jobvl, char jobvr, FloatMatrix A,
			FloatMatrix WR, FloatMatrix WI, FloatMatrix VL, FloatMatrix VR) {
		int info = NativeBlas.sgeev(jobvl, jobvr, A.rows, A.data, 0, A.rows, WR.data, 0,
				WI.data, 0, VL.data, 0, VL.rows, VR.data, 0, VR.rows);
		if (info > 0)
			throw new LapackConvergenceException("DGEEV", "First " + info + " eigenvalues have not converged.");
		return info;
	}

	public static int sygvd(int itype, char jobz, char uplo, FloatMatrix A, FloatMatrix B, FloatMatrix W) {
		int info = NativeBlas.ssygvd(itype, jobz, uplo, A.rows, A.data, 0, A.rows, B.data, 0, B.rows, W.data, 0);
		if (info == 0)
			return 0;
		else {
			if (info < 0)
				throw new LapackArgumentException("DSYGVD", -info);
			if (info <= A.rows && jobz == 'N')
				throw new LapackConvergenceException("DSYGVD", info + " off-diagonal elements did not converge to 0.");
			if (info <= A.rows && jobz == 'V')
				throw new LapackException("DSYGVD", "Failed to compute an eigenvalue while working on a sub-matrix  " + info + ".");
			else
				throw new LapackException("DSYGVD", "The leading minor of order " + (info - A.rows) + " of B is not positive definite.");
		}
	}
	
	public static int sygvx(int itype, char jobz, char range, char uplo, FloatMatrix A,
			FloatMatrix B, float vl, float vu, int il, int iu, float abstol,
			int[] m, FloatMatrix W, FloatMatrix Z) {
		int[] iwork = new int[1];
		int[] ifail = new int[1];
		int info = NativeBlas.ssygvx(itype, jobz, range, uplo, A.rows, A.data, 0, A.rows, B.data, 0, B.rows, vl, vu, il, iu, abstol, m, 0, W.data, 0, Z.data, 0, Z.rows, iwork, 0, ifail, 0);
		if (info == 0) {
			return 0;
		} else {
			if (info < 0) {
				throw new LapackArgumentException("DSYGVX", -info);
			}
			if(info <= A.rows) {
				throw new LapackConvergenceException("DSYGVX", info + " eigenvectors failed to converge");
			} else {
				throw new LapackException("DSYGVX", "The leading minor order " + (info - A.rows) + " of B is not postivie definite.");
			}
		}
	}

	/**
	 * Generalized Least Squares via *GELSD.
	 *
	 * Note that B must be padded to contain the solution matrix. This occurs when A has fewer rows
	 * than columns.
	 *
	 * For example: in A * X = B, A is (m,n), X is (n,k) and B is (m,k). Now if m < n, since B is overwritten to contain
	 * the solution (in classical LAPACK style), B needs to be padded to be an (n,k) matrix.
	 *
	 * Likewise, if m > n, the solution consists only of the first n rows of B.
	 *
	 * @param A an (m,n) matrix
	 * @param B an (max(m,n), k) matrix (well, at least)
	 */
	public static void gelsd(FloatMatrix A, FloatMatrix B) {
		int m = A.rows;
		int n = A.columns;
		int nrhs = B.columns;
		int minmn = min(m, n);
		int maxmn = max(m, n);

		if (B.rows < maxmn) {
			throw new SizeException("Result matrix B must be padded to contain the solution matrix X!");
		}

		//int smlsiz = NativeBlas.ilaenv(9, "DGELSD", "", m, n, nrhs, -1);
		int smlsiz = 25;
		int nlvl = max(0, (int) log2(minmn/ (smlsiz+1)) + 1);

		//System.err.printf("GELSD\n");
		//System.err.printf("m = %d, n = %d, nrhs = %d\n", m, n, nrhs);
		//System.err.printf("smlsiz = %d, nlvl = %d\n", smlsiz, nlvl);
		//System.err.printf("iwork size = %d\n", 3 * minmn * nlvl + 11 * minmn);

		int[] iwork = new int[3 * minmn * nlvl + 11 * minmn];
		float[] s = new float[minmn];
		int[] rank = new int[1];
		int info = NativeBlas.sgelsd(m, n, nrhs, A.data, 0, m, B.data, 0, B.rows, s, 0, -1, rank, 0, iwork, 0);
		if (info == 0) {
			return;
		} else if (info < 0) {
			throw new LapackArgumentException("DGESD", -info);
		} else if (info > 0) {
			throw new LapackConvergenceException("DGESD", info + " off-diagonal elements of an intermediat bidiagonal form did not converge to 0.");
		}
	}

	public static void geqrf(FloatMatrix A, FloatMatrix tau) {
		int info = NativeBlas.sgeqrf(A.rows, A.columns, A.data, 0, A.rows, tau.data, 0);
		checkInfo("GEQRF", info);
	}

	public static void ormqr(char side, char trans, FloatMatrix A, FloatMatrix tau, FloatMatrix C) {
		int k = tau.length;
		int info = NativeBlas.sormqr(side, trans, C.rows, C.columns, k, A.data, 0, A.rows, tau.data, 0, C.data, 0, C.rows);
		checkInfo("ORMQR", info);
	}

//END
}
