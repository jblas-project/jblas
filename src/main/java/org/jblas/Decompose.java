/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas;

import org.jblas.exceptions.LapackArgumentException;
import org.jblas.exceptions.LapackPositivityException;
import org.jblas.util.Permutations;
import static org.jblas.util.Functions.min;

/**
 * Matrix which collects all kinds of decompositions.
 */
public class Decompose {
    /**
     * Class to hold an LU decomposition result.
     *
     * Contains a lower matrix L, and upper matrix U, and a permutation matrix
     * P such that P*L*U is the original matrix.
     * @param <T>
     */
    public static class LUDecomposition<T> {

        public T l;
        public T u;
        public T p;

        public LUDecomposition(T l, T u, T p) {
            this.l = l;
            this.u = u;
            this.p = p;
        }

        @Override
        public String toString() {
          return String.format("<LUDecomposition L=%s U=%s P=%s>", l, u, p);
        }
    }

    /**
     * Compute LU Decomposition of a general matrix.
     *
     * Computes the LU decomposition using GETRF. Returns three matrices L, U, P,
     * where L is lower diagonal, U is upper diagonal, and P is a permutation
     * matrix such that A = P * L * U.
     *
     * @param A general matrix
     * @return An LUDecomposition object.
     */
    public static LUDecomposition<DoubleMatrix> lu(DoubleMatrix A) {
        int[] ipiv = new int[min(A.rows, A.columns)];
        DoubleMatrix result = A.dup();
        NativeBlas.dgetrf(A.rows, A.columns, result.data, 0, A.rows, ipiv, 0);

        // collect result
        DoubleMatrix l = new DoubleMatrix(A.rows, min(A.rows, A.columns));
        DoubleMatrix u = new DoubleMatrix(min(A.columns, A.rows), A.columns);
        decomposeLowerUpper(result, l, u);
        DoubleMatrix p = Permutations.permutationDoubleMatrixFromPivotIndices(A.rows, ipiv);
        return new LUDecomposition<DoubleMatrix>(l, u, p);
    }

    private static void decomposeLowerUpper(DoubleMatrix A, DoubleMatrix L, DoubleMatrix U) {
        for (int i = 0; i < A.rows; i++) {
            for (int j = 0; j < A.columns; j++) {
                if (i < j) {
                    U.put(i, j, A.get(i, j));
                } else if (i == j) {
                    U.put(i, i, A.get(i, i));
                    L.put(i, i, 1.0);
                } else {
                    L.put(i, j, A.get(i, j));
                }

            }
        }
    }

    /**if (info )
     * Compute Cholesky decomposition of A
     *
     * @param A symmetric, positive definite matrix (only upper half is used)
     * @return upper triangular matrix U such that  A = U' * U
     */
    public static FloatMatrix cholesky(FloatMatrix A) {
        FloatMatrix result = A.dup();
        int info = NativeBlas.spotrf('U', A.rows, result.data, 0, A.rows);
        if (info < 0) {
            throw new LapackArgumentException("DPOTRF", -info);
        } else if (info > 0) {
            throw new LapackPositivityException("DPOTRF", "Minor " + info + " was negative. Matrix must be positive definite.");
        }
        clearLower(result);
        return result;
    }

    private static void clearLower(FloatMatrix A) {
        for (int j = 0; j < A.columns; j++)
            for (int i = j + 1; i < A.rows; i++)
                A.put(i, j, 0.0f);
    }

  /**
   * Compute LU Decomposition of a general matrix.
   *
   * Computes the LU decomposition using GETRF. Returns three matrices L, U, P,
   * where L is lower diagonal, U is upper diagonal, and P is a permutation
   * matrix such that A = P * L * U.
   *
   * @param A general matrix
   * @return An LUDecomposition object.
   */
  public static LUDecomposition<FloatMatrix> lu(FloatMatrix A) {
      int[] ipiv = new int[min(A.rows, A.columns)];
      FloatMatrix result = A.dup();
      NativeBlas.sgetrf(A.rows, A.columns, result.data, 0, A.rows, ipiv, 0);

      // collect result
      FloatMatrix l = new FloatMatrix(A.rows, min(A.rows, A.columns));
      FloatMatrix u = new FloatMatrix(min(A.columns, A.rows), A.columns);
      decomposeLowerUpper(result, l, u);
      FloatMatrix p = Permutations.permutationFloatMatrixFromPivotIndices(A.rows, ipiv);
      return new LUDecomposition<FloatMatrix>(l, u, p);
  }

  private static void decomposeLowerUpper(FloatMatrix A, FloatMatrix L, FloatMatrix U) {
      for (int i = 0; i < A.rows; i++) {
          for (int j = 0; j < A.columns; j++) {
              if (i < j) {
                  U.put(i, j, A.get(i, j));
              } else if (i == j) {
                  U.put(i, i, A.get(i, i));
                  L.put(i, i, 1.0f);
              } else {
                  L.put(i, j, A.get(i, j));
              }

          }
      }
  }

  /**
   * Compute Cholesky decomposition of A
   *
   * @param A symmetric, positive definite matrix (only upper half is used)
   * @return upper triangular matrix U such that  A = U' * U
   */
  public static DoubleMatrix cholesky(DoubleMatrix A) {
      DoubleMatrix result = A.dup();
      int info = NativeBlas.dpotrf('U', A.rows, result.data, 0, A.rows);
      if (info < 0) {
          throw new LapackArgumentException("DPOTRF", -info);
      } else if (info > 0) {
          throw new LapackPositivityException("DPOTRF", "Minor " + info + " was negative. Matrix must be positive definite.");
      }
      clearLower(result);
      return result;
  }

  private static void clearLower(DoubleMatrix A) {
      for (int j = 0; j < A.columns; j++)
          for (int i = j + 1; i < A.rows; i++)
              A.put(i, j, 0.0);
  }

  /**
   * Class to represent a QR decomposition.
   *
   * @param <T>
   */
  public static class QRDecomposition<T> {
    public T q;
    public T r;

    QRDecomposition(T q, T r) {
      this.q = q;
      this.r = r;
    }

    @Override
    public String toString() {
      return "<Q=" + q + " R=" + r + ">";
    }
  }

  /**
   * QR decomposition.
   *
   * Decomposes (m,n) matrix A into a (m,m) matrix Q and an (m,n) matrix R such that
   * Q is orthogonal, R is upper triangular and Q * R = A
   *
   * Note that if A has more rows than columns, then the lower rows of R will contain
   * only zeros, such that the corresponding later columns of Q do not enter the computation
   * at all. For some reason, LAPACK does not properly normalize those columns.
   *
   * @param A matrix
   * @return QR decomposition
   */
  public static QRDecomposition<DoubleMatrix> qr(DoubleMatrix A) {
    int minmn = min(A.rows, A.columns);
    DoubleMatrix result = A.dup();
    DoubleMatrix tau = new DoubleMatrix(minmn);
    SimpleBlas.geqrf(result, tau);
    DoubleMatrix R = new DoubleMatrix(A.rows, A.columns);
    for (int i = 0; i < A.rows; i++) {
      for (int j = i; j < A.columns; j++) {
        R.put(i, j, result.get(i, j));
      }
    }
    DoubleMatrix Q = DoubleMatrix.eye(A.rows);
    SimpleBlas.ormqr('L', 'N', result, tau, Q);
    return new QRDecomposition<DoubleMatrix>(Q, R);
  }
  
  /**
   * QR decomposition.
   *
   * Decomposes (m,n) matrix A into a (m,m) matrix Q and an (m,n) matrix R such that
   * Q is orthogonal, R is upper triangular and Q * R = A
   *
   * @param A matrix
   * @return QR decomposition
   */
  public static QRDecomposition<FloatMatrix> qr(FloatMatrix A) {
    int minmn = min(A.rows, A.columns);
    FloatMatrix result = A.dup();
    FloatMatrix tau = new FloatMatrix(minmn);
    SimpleBlas.geqrf(result, tau);
    FloatMatrix R = new FloatMatrix(A.rows, A.columns);
    for (int i = 0; i < A.rows; i++) {
      for (int j = i; j < A.columns; j++) {
        R.put(i, j, result.get(i, j));
      }
    }
    FloatMatrix Q = FloatMatrix.eye(A.rows);
    SimpleBlas.ormqr('L', 'N', result, tau, Q);
    return new QRDecomposition<FloatMatrix>(Q, R);
  }
}
