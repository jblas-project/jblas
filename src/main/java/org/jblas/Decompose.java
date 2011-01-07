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

//STOP
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
    }
//START

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
        DoubleMatrix p = Permutations.permutationMatrixFromPivotIndices(A.rows, ipiv);
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
}
