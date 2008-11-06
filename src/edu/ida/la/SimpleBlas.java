package edu.ida.la;

import edu.ida.core.ComplexDouble; 
import edu.ida.core.ComplexFloat; 

import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

import static edu.ida.core.BlasUtil.*;

//import edu.ida.core.OutputValue;

/**
 * This class provides a cleaner direct interface to the BLAS routines by
 * extracting the parameters of the matrices from the matrices itself.
 * 
 * For example, you can just pass the vector and do not have to pass the length,
 * corresponding DoubleBuffer, offset and step size explicitly.
 * 
 * Currently, all the general matrix routines are implemented.
 * 
 */
public class SimpleBlas {
	/***************************************************************************
	 * BLAS Level 1
	 */

	/** Compute x <-> y (swap two matrices) */
	public static DoubleMatrix swap(DoubleMatrix x, DoubleMatrix y) {
		//Blas.dswap(x.length, x.data, 0, 1, y.data, 0, 1);
		JavaBlas.rswap(x.length, x.data, 0, 1, y.data, 0, 1);
                return y;
	}

	/** Compute x <- alpha * x (scale a matrix) */
	public static DoubleMatrix scal(double alpha, DoubleMatrix x) {
		Blas.dscal(x.length, alpha, x.data, 0, 1);
		return x;
	}

	public static ComplexDoubleMatrix scal(ComplexDouble alpha, ComplexDoubleMatrix x) {
		Blas.zscal(x.length, alpha, x.data, 0, 1);
		return x;
	}
		
	/** Compute y <- x (copy a matrix) */
	public static DoubleMatrix copy(DoubleMatrix x, DoubleMatrix y) {
		//Blas.dcopy(x.length, x.data, 0, 1, y.data, 0, 1);
                JavaBlas.rcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}
	
	public static ComplexDoubleMatrix copy(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		Blas.zcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;		
	}
	
	/** Compute y <- alpha * x + y (elementwise addition) */
	public static DoubleMatrix axpy(double da, DoubleMatrix dx, DoubleMatrix dy) {
		//Blas.daxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		JavaBlas.raxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
             
		return dy;
	}

	public static ComplexDoubleMatrix axpy(ComplexDouble da, ComplexDoubleMatrix dx, ComplexDoubleMatrix dy) {
		Blas.zaxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		return dy;
	}

	/** Compute x^T * y (dot product) */
	public static double dot(DoubleMatrix x, DoubleMatrix y) {
		//return Blas.ddot(x.length, x.data, 0, 1, y.data, 0, 1);
                return JavaBlas.rdot(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/** Compute x^T * y (dot product) */
	public static ComplexDouble dotc(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		return Blas.zdotc(x.length, x.data, 0, 1, y.data, 0, 1);
	}
	
	/** Compute x^T * y (dot product) */
	public static ComplexDouble dotu(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		return Blas.zdotu(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/** Compute || x ||_2 (2-norm) */
	public static double nrm2(DoubleMatrix x) {
		return Blas.dnrm2(x.length, x.data, 0, 1);
	}
	
	public static double nrm2(ComplexDoubleMatrix x) {
		return Blas.dznrm2(x.length, x.data, 0, 1);
	}

	/** Compute || x ||_1 (1-norm, sum of absolute values) */
	public static double asum(DoubleMatrix x) {
		return Blas.dasum(x.length, x.data, 0, 1);
	}

	public static double asum(ComplexDoubleMatrix x) {
		return Blas.dzasum(x.length, x.data, 0, 1);
	}

	/**
	 * Compute index of element with largest absolute value (index of absolute
	 * value maximum)
	 */
	public static int iamax(DoubleMatrix x) {
		return Blas.idamax(x.length, x.data, 0, 1) - 1;
	}

	public static int iamax(ComplexDoubleMatrix x) {
		return Blas.izamax(x.length, x.data, 0, 1);
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
		Blas.dgemv('N', a.rows, a.columns, alpha, a.data, 0, a.rows, x.data, 0,
				1, beta, y.data, 0, 1);
            }
            else {
                if (beta == 0.0) {
                    for (int i = 0; i < y.length; i++)
                        y.data[i] = 0.0;
                    
                        for (int j = 0; j < a.columns; j++)
                            for (int i = 0; i < a.rows; i++)
                                y.data[i] += a.get(i, j) * x.get(j);
                }
                else {
                     for (int j = 0; j < a.columns; j++)
                         for (int i = 0; i < a.rows; i++)
                              y.data[j] = a.get(i, j) * x.get(i) + y.data[j];                    
                }
            }
	    return y;
	}

	/** Compute A <- alpha * x * y^T + A (general rank-1 update) */
	public static DoubleMatrix ger(double alpha, DoubleMatrix x,
			DoubleMatrix y, DoubleMatrix a) {
		Blas.dger(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/** Compute A <- alpha * x * y^T + A (general rank-1 update) */
	public static ComplexDoubleMatrix geru(ComplexDouble alpha, ComplexDoubleMatrix x,
			ComplexDoubleMatrix y, ComplexDoubleMatrix a) {
		Blas.zgeru(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/** Compute A <- alpha * x * y^H + A (general rank-1 update) */
	public static ComplexDoubleMatrix gerc(ComplexDouble alpha, ComplexDoubleMatrix x,
			ComplexDoubleMatrix y, ComplexDoubleMatrix a) {
		Blas.zgerc(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
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
		Blas.dgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	public static ComplexDoubleMatrix gemm(ComplexDouble alpha, ComplexDoubleMatrix a,
			ComplexDoubleMatrix b, ComplexDouble beta, ComplexDoubleMatrix c) {
		Blas.zgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	/***************************************************************************
	 * LAPACK
	 */
	public static DoubleMatrix gesv(DoubleMatrix a, int[] ipiv,
			DoubleMatrix b) {
		int info = Blas.dgesv(a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
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
		double[] work = new double[1];
		int info = Blas.dsysv(uplo, a.rows, b.columns, a.data, 0, a.rows, ipiv,
				0, b.data, 0, b.rows, work, 0, -1);
		checkInfo("DSYSV", info);

		int lwork = (int) work[0];
		work = new double[lwork];

		// System.out.println("Optimal LWORK = " + lwork);

		info = Blas.dsysv(uplo, a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows, work, 0, lwork);
		checkInfo("DSYSV", info);

		if (info > 0)
			throw new IllegalArgumentException(
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}

	public static int syev(char jobz, char uplo, DoubleMatrix a, DoubleMatrix w) {
		double[] work = new double[1];
		int info = Blas.dsyev(jobz, uplo, a.rows, a.data, 0, a.rows, w.data, 0,
				work, 0, -1);
		checkInfo("DSYEV", info);

		int lwork = (int) work[0];
		work = new double[lwork];

		// System.out.println("Optimal LWORK = " + lwork);

		info = Blas.dsyev(jobz, uplo, a.rows, a.data, 0, a.rows, w.data, 0,
				work, 0, lwork);

		if (info > 0)
			throw new IllegalArgumentException(
					"Eigenvalues could not be computed " + info
							+ " off-diagonal elements did not converge");

		return info;
	}

	public static int syevx(char jobz, char range, char uplo, DoubleMatrix a,
			double vl, double vu, int il, int iu, double abstol,
			DoubleMatrix w, DoubleMatrix z) {
		int n = a.rows;
		double[] work = new double[1];
		int[] iwork = new int[5 * n];
		int[] ifail = new int[n];

		int info = Blas.dsyevx(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu,
				il, iu, abstol, 0, w.data, 0, z.data, 0, z.rows, work, 0, -1,
				iwork, 0, ifail, 0);
		checkInfo("DSYEVX", info);

		int lwork = (int) work[0];
		work = new double[lwork];

		// System.out.println("Optimal LWORK = " + lwork);

		info = Blas.dsyevx(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu, il,
				iu, abstol, 0, w.data, 0, z.data, 0, z.rows, work, 0, -1,
				iwork, 0, ifail, 0);

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
			throw new IllegalArgumentException(msg.toString());
		}

		return info;
	}

	public static int syevd(char jobz, char uplo, DoubleMatrix A,
			DoubleMatrix w) {
		int n = A.rows;

		double[] work = new double[1];
		int[] iwork = new int[1];

		int info = Blas.dsyevd(jobz, uplo, n, A.data, 0, A.rows, w.data, 0,
				work, 0, -1, iwork, 0, -1);
		checkInfo("DSYEVD", info);

		int lwork = (int) work[0];
		int liwork = iwork[0];

		// System.out.println("Optimal LWORK = " + lwork);
		// System.out.println("Optimal LIWORK = " + lwork);

		work = new double[lwork];
		iwork = new int[liwork];

		info = Blas.dsyevd(jobz, uplo, n, A.data, 0, A.rows, w.data, 0, work,
				0, lwork, iwork, 0, liwork);

		if (info > 0)
			throw new IllegalArgumentException("Not all eigenvalues converged.");

		return info;
	}

	public static void posv(char uplo, DoubleMatrix A, DoubleMatrix B) {
		int n = A.rows;
		int nrhs = B.columns;
		int info = Blas.dposv(uplo, n, nrhs, A.data, 0, A.rows, B.data, 0,
				B.rows);
		checkInfo("DPOSV", info);
		if (info > 0)
			throw new LapackException("DPOSV",
					"Leading minor of order i of A is not positive definite.");
	}
        
        public static int geev(char jobvl, char jobvr, DoubleMatrix A, 
                DoubleMatrix WR, DoubleMatrix WI, DoubleMatrix VL, DoubleMatrix VR) {
            int n = A.rows;
            double[] work = new double[1];
            int info = Blas.dgeev(jobvl, jobvr, A.rows, A.data, 0, A.rows, WR.data, 0, 
                    WI.data, 0, VL.data, 0, VL.rows, VR.data, 0, VR.rows, work, 0, -1);
            checkInfo("DGEEV", info);
            int lwork = (int)work[0];
            work = new double[lwork];
            info = Blas.dgeev(jobvl, jobvr, A.rows, A.data, 0, A.rows, WR.data, 0, 
                    WI.data, 0, VL.data, 0, VL.rows, VR.data, 0, VR.rows, work, 0, lwork);
            if (info > 0)
                throw new LapackException("DGEEV", "First " + info + " eigenvalues have not converged.");
            return info;
        }

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
	/***************************************************************************
	 * BLAS Level 1
	 */

	/** Compute x <-> y (swap two matrices) */
	public static FloatMatrix swap(FloatMatrix x, FloatMatrix y) {
		//Blas.sswap(x.length, x.data, 0, 1, y.data, 0, 1);
		JavaBlas.rswap(x.length, x.data, 0, 1, y.data, 0, 1);
                return y;
	}

	/** Compute x <- alpha * x (scale a matrix) */
	public static FloatMatrix scal(float alpha, FloatMatrix x) {
		Blas.sscal(x.length, alpha, x.data, 0, 1);
		return x;
	}

	public static ComplexFloatMatrix scal(ComplexFloat alpha, ComplexFloatMatrix x) {
		Blas.cscal(x.length, alpha, x.data, 0, 1);
		return x;
	}
		
	/** Compute y <- x (copy a matrix) */
	public static FloatMatrix copy(FloatMatrix x, FloatMatrix y) {
		//Blas.scopy(x.length, x.data, 0, 1, y.data, 0, 1);
                JavaBlas.rcopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;
	}
	
	public static ComplexFloatMatrix copy(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		Blas.ccopy(x.length, x.data, 0, 1, y.data, 0, 1);
		return y;		
	}
	
	/** Compute y <- alpha * x + y (elementwise addition) */
	public static FloatMatrix axpy(float da, FloatMatrix dx, FloatMatrix dy) {
		//Blas.saxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		JavaBlas.raxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
             
		return dy;
	}

	public static ComplexFloatMatrix axpy(ComplexFloat da, ComplexFloatMatrix dx, ComplexFloatMatrix dy) {
		Blas.caxpy(dx.length, da, dx.data, 0, 1, dy.data, 0, 1);
		return dy;
	}

	/** Compute x^T * y (dot product) */
	public static float dot(FloatMatrix x, FloatMatrix y) {
		//return Blas.sdot(x.length, x.data, 0, 1, y.data, 0, 1);
                return JavaBlas.rdot(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/** Compute x^T * y (dot product) */
	public static ComplexFloat dotc(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		return Blas.cdotc(x.length, x.data, 0, 1, y.data, 0, 1);
	}
	
	/** Compute x^T * y (dot product) */
	public static ComplexFloat dotu(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		return Blas.cdotu(x.length, x.data, 0, 1, y.data, 0, 1);
	}

	/** Compute || x ||_2 (2-norm) */
	public static float nrm2(FloatMatrix x) {
		return Blas.snrm2(x.length, x.data, 0, 1);
	}
	
	public static float nrm2(ComplexFloatMatrix x) {
		return Blas.scnrm2(x.length, x.data, 0, 1);
	}

	/** Compute || x ||_1 (1-norm, sum of absolute values) */
	public static float asum(FloatMatrix x) {
		return Blas.sasum(x.length, x.data, 0, 1);
	}

	public static float asum(ComplexFloatMatrix x) {
		return Blas.scasum(x.length, x.data, 0, 1);
	}

	/**
	 * Compute index of element with largest absolute value (index of absolute
	 * value maximum)
	 */
	public static int iamax(FloatMatrix x) {
		return Blas.isamax(x.length, x.data, 0, 1) - 1;
	}

	public static int iamax(ComplexFloatMatrix x) {
		return Blas.icamax(x.length, x.data, 0, 1);
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
		Blas.sgemv('N', a.rows, a.columns, alpha, a.data, 0, a.rows, x.data, 0,
				1, beta, y.data, 0, 1);
            }
            else {
                if (beta == 0.0f) {
                    for (int i = 0; i < y.length; i++)
                        y.data[i] = 0.0f;
                    
                        for (int j = 0; j < a.columns; j++)
                            for (int i = 0; i < a.rows; i++)
                                y.data[i] += a.get(i, j) * x.get(j);
                }
                else {
                     for (int j = 0; j < a.columns; j++)
                         for (int i = 0; i < a.rows; i++)
                              y.data[j] = a.get(i, j) * x.get(i) + y.data[j];                    
                }
            }
	    return y;
	}

	/** Compute A <- alpha * x * y^T + A (general rank-1 update) */
	public static FloatMatrix ger(float alpha, FloatMatrix x,
			FloatMatrix y, FloatMatrix a) {
		Blas.sger(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/** Compute A <- alpha * x * y^T + A (general rank-1 update) */
	public static ComplexFloatMatrix geru(ComplexFloat alpha, ComplexFloatMatrix x,
			ComplexFloatMatrix y, ComplexFloatMatrix a) {
		Blas.cgeru(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
				0, a.rows);
		return a;
	}

	/** Compute A <- alpha * x * y^H + A (general rank-1 update) */
	public static ComplexFloatMatrix gerc(ComplexFloat alpha, ComplexFloatMatrix x,
			ComplexFloatMatrix y, ComplexFloatMatrix a) {
		Blas.cgerc(a.rows, a.columns, alpha, x.data, 0, 1, y.data, 0, 1, a.data,
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
		Blas.sgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	public static ComplexFloatMatrix gemm(ComplexFloat alpha, ComplexFloatMatrix a,
			ComplexFloatMatrix b, ComplexFloat beta, ComplexFloatMatrix c) {
		Blas.cgemm('N', 'N', c.rows, c.columns, a.columns, alpha, a.data, 0,
				a.rows, b.data, 0, b.rows, beta, c.data, 0, c.rows);
		return c;
	}

	/***************************************************************************
	 * LAPACK
	 */
	public static FloatMatrix gesv(FloatMatrix a, int[] ipiv,
			FloatMatrix b) {
		int info = Blas.sgesv(a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows);
		checkInfo("DGESV", info);

		if (info > 0)
			throw new LapackException("DGESV",
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}


	public static FloatMatrix sysv(char uplo, FloatMatrix a, int[] ipiv,
			FloatMatrix b) {
		float[] work = new float[1];
		int info = Blas.ssysv(uplo, a.rows, b.columns, a.data, 0, a.rows, ipiv,
				0, b.data, 0, b.rows, work, 0, -1);
		checkInfo("DSYSV", info);

		int lwork = (int) work[0];
		work = new float[lwork];

		// System.out.println("Optimal LWORK = " + lwork);

		info = Blas.ssysv(uplo, a.rows, b.columns, a.data, 0, a.rows, ipiv, 0,
				b.data, 0, b.rows, work, 0, lwork);
		checkInfo("DSYSV", info);

		if (info > 0)
			throw new IllegalArgumentException(
					"Linear equation cannot be solved because the matrix was singular.");

		return b;
	}

	public static int syev(char jobz, char uplo, FloatMatrix a, FloatMatrix w) {
		float[] work = new float[1];
		int info = Blas.ssyev(jobz, uplo, a.rows, a.data, 0, a.rows, w.data, 0,
				work, 0, -1);
		checkInfo("DSYEV", info);

		int lwork = (int) work[0];
		work = new float[lwork];

		// System.out.println("Optimal LWORK = " + lwork);

		info = Blas.ssyev(jobz, uplo, a.rows, a.data, 0, a.rows, w.data, 0,
				work, 0, lwork);

		if (info > 0)
			throw new IllegalArgumentException(
					"Eigenvalues could not be computed " + info
							+ " off-diagonal elements did not converge");

		return info;
	}

	public static int syevx(char jobz, char range, char uplo, FloatMatrix a,
			float vl, float vu, int il, int iu, float abstol,
			FloatMatrix w, FloatMatrix z) {
		int n = a.rows;
		float[] work = new float[1];
		int[] iwork = new int[5 * n];
		int[] ifail = new int[n];

		int info = Blas.ssyevx(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu,
				il, iu, abstol, 0, w.data, 0, z.data, 0, z.rows, work, 0, -1,
				iwork, 0, ifail, 0);
		checkInfo("DSYEVX", info);

		int lwork = (int) work[0];
		work = new float[lwork];

		// System.out.println("Optimal LWORK = " + lwork);

		info = Blas.ssyevx(jobz, range, uplo, n, a.data, 0, a.rows, vl, vu, il,
				iu, abstol, 0, w.data, 0, z.data, 0, z.rows, work, 0, -1,
				iwork, 0, ifail, 0);

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
			throw new IllegalArgumentException(msg.toString());
		}

		return info;
	}

	public static int syevd(char jobz, char uplo, FloatMatrix A,
			FloatMatrix w) {
		int n = A.rows;

		float[] work = new float[1];
		int[] iwork = new int[1];

		int info = Blas.ssyevd(jobz, uplo, n, A.data, 0, A.rows, w.data, 0,
				work, 0, -1, iwork, 0, -1);
		checkInfo("DSYEVD", info);

		int lwork = (int) work[0];
		int liwork = iwork[0];

		// System.out.println("Optimal LWORK = " + lwork);
		// System.out.println("Optimal LIWORK = " + lwork);

		work = new float[lwork];
		iwork = new int[liwork];

		info = Blas.ssyevd(jobz, uplo, n, A.data, 0, A.rows, w.data, 0, work,
				0, lwork, iwork, 0, liwork);

		if (info > 0)
			throw new IllegalArgumentException("Not all eigenvalues converged.");

		return info;
	}

	public static void posv(char uplo, FloatMatrix A, FloatMatrix B) {
		int n = A.rows;
		int nrhs = B.columns;
		int info = Blas.sposv(uplo, n, nrhs, A.data, 0, A.rows, B.data, 0,
				B.rows);
		checkInfo("DPOSV", info);
		if (info > 0)
			throw new LapackException("DPOSV",
					"Leading minor of order i of A is not positive definite.");
	}
        
        public static int geev(char jobvl, char jobvr, FloatMatrix A, 
                FloatMatrix WR, FloatMatrix WI, FloatMatrix VL, FloatMatrix VR) {
            int n = A.rows;
            float[] work = new float[1];
            int info = Blas.sgeev(jobvl, jobvr, A.rows, A.data, 0, A.rows, WR.data, 0, 
                    WI.data, 0, VL.data, 0, VL.rows, VR.data, 0, VR.rows, work, 0, -1);
            checkInfo("DGEEV", info);
            int lwork = (int)work[0];
            work = new float[lwork];
            info = Blas.sgeev(jobvl, jobvr, A.rows, A.data, 0, A.rows, WR.data, 0, 
                    WI.data, 0, VL.data, 0, VL.rows, VR.data, 0, VR.rows, work, 0, lwork);
            if (info > 0)
                throw new LapackException("DGEEV", "First " + info + " eigenvalues have not converged.");
            return info;
        }

//END
}
