/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas;

import org.jblas.exceptions.LapackConvergenceException;

import static org.jblas.util.Functions.min;

/**
 *
 */
public class Singular {

    /**
     * Compute a singular-value decomposition of A.
     *
     * @return A DoubleMatrix[3] array of U, S, V such that A = U * diag(S) * V'
     */
    public static DoubleMatrix[] fullSVD(DoubleMatrix A) {
        int m = A.rows;
        int n = A.columns;

        DoubleMatrix U = new DoubleMatrix(m, m);
        DoubleMatrix S = new DoubleMatrix(min(m, n));
        DoubleMatrix V = new DoubleMatrix(n, n);

        int info = NativeBlas.dgesvd('A', 'A', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, n);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return new DoubleMatrix[]{U, S, V.transpose()};
    }

    /**
     * Compute a singular-value decomposition of A (sparse variant).
     * Sparse means that the matrices U and V are not square but
     * only have as many columns (or rows) as necessary.
     * 
     * @param A
     * @return A DoubleMatrix[3] array of U, S, V such that A = U * diag(S) * V'
     */
    public static DoubleMatrix[] sparseSVD(DoubleMatrix A) {
        int m = A.rows;
        int n = A.columns;

        DoubleMatrix U = new DoubleMatrix(m, min(m, n));
        DoubleMatrix S = new DoubleMatrix(min(m, n));
        DoubleMatrix V = new DoubleMatrix(min(m, n), n);

        int info = NativeBlas.dgesvd('S', 'S', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, min(m, n));

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return new DoubleMatrix[]{U, S, V.transpose()};
    }

  /**
   * Compute a singular-value decomposition of A (sparse variant).
   * Sparse means that the matrices U and V are not square but only have
   * as many columns (or rows) as necessary.
   *
   * @param A
   * @return A ComplexDoubleMatrix[3] array of U, S, V such that A = U * diag(S) * V*
   */
    public static ComplexDoubleMatrix[] sparseSVD(ComplexDoubleMatrix A) {
        int m = A.rows;
        int n = A.columns;

        ComplexDoubleMatrix U = new ComplexDoubleMatrix(m, min(m, n));
        DoubleMatrix S = new DoubleMatrix(min(m, n));
        ComplexDoubleMatrix V = new ComplexDoubleMatrix(min(m, n), n);

        double[] rwork = new double[5*min(m,n)];

        int info = NativeBlas.zgesvd('S', 'S', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, min(m, n), rwork, 0);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return new ComplexDoubleMatrix[]{U, new ComplexDoubleMatrix(S), V.hermitian()};
    }

    /**
     * Compute a singular-value decomposition of A.
     *
     * @return A ComplexDoubleMatrix[3] array of U, S, V such that A = U * diag(S) * V'
     */
    public static ComplexDoubleMatrix[] fullSVD(ComplexDoubleMatrix A) {
      int m = A.rows;
      int n = A.columns;

      ComplexDoubleMatrix U = new ComplexDoubleMatrix(m, m);
      DoubleMatrix S = new DoubleMatrix(min(m, n));
      ComplexDoubleMatrix V = new ComplexDoubleMatrix(n, n);

      double[] rwork = new double[5*min(m,n)];

      int info = NativeBlas.zgesvd('A', 'A', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, n, rwork, 0);

      if (info > 0) {
        throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
      }

      return new ComplexDoubleMatrix[]{U, new ComplexDoubleMatrix(S), V.hermitian()};
    }

    /**
     * Compute the singular values of a matrix.
     *
     * @param A DoubleMatrix of dimension m * n
     * @return A min(m, n) vector of singular values.
     */
    public static DoubleMatrix SVDValues(DoubleMatrix A) {
        int m = A.rows;
        int n = A.columns;
        DoubleMatrix S = new DoubleMatrix(min(m, n));

        int info = NativeBlas.dgesvd('N', 'N', m, n, A.dup().data, 0, m, S.data, 0, null, 0, 1, null, 0, 1);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return S;
    }

    /**
     * Compute the singular values of a complex matrix.
     *
     * @param A ComplexDoubleMatrix of dimension m * n
     * @return A real-valued (!) min(m, n) vector of singular values.
     */
    public static DoubleMatrix SVDValues(ComplexDoubleMatrix A) {
        int m = A.rows;
        int n = A.columns;
        DoubleMatrix S = new DoubleMatrix(min(m, n));
        double[] rwork = new double[5*min(m,n)];

        int info = NativeBlas.zgesvd('N', 'N', m, n, A.dup().data, 0, m, S.data, 0, null, 0, 1, null, 0, min(m,n), rwork, 0);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return S;
    }

    //BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!

    /**
     * Compute a singular-value decomposition of A.
     *
     * @return A FloatMatrix[3] array of U, S, V such that A = U * diag(S) * V'
     */
    public static FloatMatrix[] fullSVD(FloatMatrix A) {
        int m = A.rows;
        int n = A.columns;

        FloatMatrix U = new FloatMatrix(m, m);
        FloatMatrix S = new FloatMatrix(min(m, n));
        FloatMatrix V = new FloatMatrix(n, n);

        int info = NativeBlas.sgesvd('A', 'A', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, n);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return new FloatMatrix[]{U, S, V.transpose()};
    }

    /**
     * Compute a singular-value decomposition of A (sparse variant).
     * Sparse means that the matrices U and V are not square but
     * only have as many columns (or rows) as necessary.
     * 
     * @param A
     * @return A FloatMatrix[3] array of U, S, V such that A = U * diag(S) * V'
     */
    public static FloatMatrix[] sparseSVD(FloatMatrix A) {
        int m = A.rows;
        int n = A.columns;

        FloatMatrix U = new FloatMatrix(m, min(m, n));
        FloatMatrix S = new FloatMatrix(min(m, n));
        FloatMatrix V = new FloatMatrix(min(m, n), n);

        int info = NativeBlas.sgesvd('S', 'S', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, min(m, n));

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return new FloatMatrix[]{U, S, V.transpose()};
    }

  /**
   * Compute a singular-value decomposition of A (sparse variant).
   * Sparse means that the matrices U and V are not square but only have
   * as many columns (or rows) as necessary.
   *
   * @param A
   * @return A ComplexFloatMatrix[3] array of U, S, V such that A = U * diag(S) * V*
   */
    public static ComplexFloatMatrix[] sparseSVD(ComplexFloatMatrix A) {
        int m = A.rows;
        int n = A.columns;

        ComplexFloatMatrix U = new ComplexFloatMatrix(m, min(m, n));
        FloatMatrix S = new FloatMatrix(min(m, n));
        ComplexFloatMatrix V = new ComplexFloatMatrix(min(m, n), n);

        float[] rwork = new float[5*min(m,n)];

        int info = NativeBlas.cgesvd('S', 'S', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, min(m, n), rwork, 0);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return new ComplexFloatMatrix[]{U, new ComplexFloatMatrix(S), V.hermitian()};
    }

    /**
     * Compute a singular-value decomposition of A.
     *
     * @return A ComplexFloatMatrix[3] array of U, S, V such that A = U * diag(S) * V'
     */
    public static ComplexFloatMatrix[] fullSVD(ComplexFloatMatrix A) {
      int m = A.rows;
      int n = A.columns;

      ComplexFloatMatrix U = new ComplexFloatMatrix(m, m);
      FloatMatrix S = new FloatMatrix(min(m, n));
      ComplexFloatMatrix V = new ComplexFloatMatrix(n, n);

      float[] rwork = new float[5*min(m,n)];

      int info = NativeBlas.cgesvd('A', 'A', m, n, A.dup().data, 0, m, S.data, 0, U.data, 0, m, V.data, 0, n, rwork, 0);

      if (info > 0) {
        throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
      }

      return new ComplexFloatMatrix[]{U, new ComplexFloatMatrix(S), V.hermitian()};
    }

    /**
     * Compute the singular values of a matrix.
     *
     * @param A FloatMatrix of dimension m * n
     * @return A min(m, n) vector of singular values.
     */
    public static FloatMatrix SVDValues(FloatMatrix A) {
        int m = A.rows;
        int n = A.columns;
        FloatMatrix S = new FloatMatrix(min(m, n));

        int info = NativeBlas.sgesvd('N', 'N', m, n, A.dup().data, 0, m, S.data, 0, null, 0, 1, null, 0, 1);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return S;
    }

    /**
     * Compute the singular values of a complex matrix.
     *
     * @param A ComplexFloatMatrix of dimension m * n
     * @return A real-valued (!) min(m, n) vector of singular values.
     */
    public static FloatMatrix SVDValues(ComplexFloatMatrix A) {
        int m = A.rows;
        int n = A.columns;
        FloatMatrix S = new FloatMatrix(min(m, n));
        float[] rwork = new float[5*min(m,n)];

        int info = NativeBlas.cgesvd('N', 'N', m, n, A.dup().data, 0, m, S.data, 0, null, 0, 1, null, 0, min(m,n), rwork, 0);

        if (info > 0) {
          throw new LapackConvergenceException("GESVD", info + " superdiagonals of an intermediate bidiagonal form failed to converge.");
        }

        return S;
    }

    //END
}
