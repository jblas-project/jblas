/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas.util;

import org.jblas.NativeBlas;
import org.jblas.DoubleMatrix;

/**
 *
 */
public class SanityChecks {

    public static int checksFailed;

    public static void check(String message, boolean condition) {
        System.out.print(message + "... ");
        if (condition) {
            System.out.println("ok");
        } else {
            System.out.println("failed");
            checksFailed++;
        }
    }

    /** Check whether vector addition works. This is pure Java code and should work. */
    public static void checkVectorAddition() {
        DoubleMatrix x = new DoubleMatrix(3, 1, 1.0, 2.0, 3.0);
        DoubleMatrix y = new DoubleMatrix(3, 1, 4.0, 5.0, 6.0);
        DoubleMatrix z = new DoubleMatrix(3, 1, 5.0, 7.0, 9.0);

        check("checking vector addition", x.add(y).equals(z));
    }

    /** Check matrix multiplication. This is already ATLAS/BLAS code. */
    public static void checkMatrixMultiplication() {

        DoubleMatrix A = new DoubleMatrix(new double[][]{
                    {1.0, 2.0, 3.0},
                    {4.0, 5.0, 6.0},
                    {7.0, 8.0, 9.0}
                });
        DoubleMatrix E = new DoubleMatrix(new double[][]{
                    {0.0, 0.0, 1.0},
                    {0.0, 1.0, 0.0},
                    {1.0, 0.0, 0.0}
                });
        DoubleMatrix B = new DoubleMatrix(new double[][]{
                    {3.0, 2.0, 1.0},
                    {6.0, 5.0, 4.0},
                    {9.0, 8.0, 7.0}
                });

        check("checking matrix multiplication", A.mmul(E).equals(B));
    }

    /**
     * Check whether error handling works. If it works, you should see an
     * ok, otherwise, you might see the actual error message and then
     * the program exits.
     */
    public static void checkXerbla() {
        double[] x = new double[9];
        try {
            NativeBlas.dgemm('N', 'N', 3, -1, 3, 1.0, x, 0, 3, x, 0, 3, 0.0, x, 0, 3);
        } catch (IllegalArgumentException e) {
            check("checking XERBLA", e.getMessage().contains("XERBLA"));
            return;
        }
        assert (false); // shouldn't happen
    }

    /**
     * Compute eigenvalues. This is a routine not in ATLAS, but in the original
     * LAPACK.
     */
    public static void checkEigenvalues() {
        DoubleMatrix A = new DoubleMatrix(new double[][]{
                    {3.0, 2.0, 0.0},
                    {2.0, 3.0, 2.0},
                    {0.0, 2.0, 3.0}
                });

        DoubleMatrix E = new DoubleMatrix(3, 1);

        NativeBlas.dsyev('N', 'U', 3, A.data, 0, 3, E.data, 0);
        check("checking existence of dsyev...", true);
    }

    public static void main(String[] args) {
        checkVectorAddition();
        checkMatrixMultiplication();
        checkXerbla();
        checkEigenvalues();
        printSummary();
    }

    private static void printSummary() {
        if (checksFailed == 0) {
            System.out.println("Sanity checks passed.");
        } else {
            System.out.println("Sainty checks FAILED!");
        }
    }
}
