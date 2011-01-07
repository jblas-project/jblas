// --- BEGIN LICENSE BLOCK ---
/*
 * Copyright (c) 2009-2011, Mikio L. Braun
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
package org.jblas.util;

import org.jblas.ComplexDouble;
import org.jblas.ComplexDoubleMatrix;
import org.jblas.NativeBlas;
import org.jblas.DoubleMatrix;

/**
 * Run a few sanity checks on the installation to see whether
 * everything runs as expected.
 *
 * @author Mikio L. Braun
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
        System.out.println("Check whether we're catching XERBLA errors. If you see something like \"** On entry to DGEMM  parameter number  4 had an illegal value\", it didn't work!");
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

    public static void checkSVD() {
        double[][] data = new double[][]{
            {1.0, 2.0, 3.0},
            {4.0, 5.0, 6.0},
            {7.0, 8.0, 9.0},
            {-1.0, -2.0, -3.0}
        };

        DoubleMatrix A = new DoubleMatrix(data);

        DoubleMatrix[] USV = org.jblas.Singular.sparseSVD(A);
        System.out.println(USV[0].toString());
        System.out.println(USV[1].toString());
        System.out.println(USV[2].toString());

        System.out.println(org.jblas.Singular.SVDValues(A));

        /*ComplexDoubleMatrix[] AZB = org.jblas.Singular.sparseSVD(new ComplexDoubleMatrix(data));
        System.out.println(AZB[0].toString());
        System.out.println(AZB[1].toString());
        System.out.println(AZB[2].toString());*/
        check("checking existence of dgesvd...", true);
    }

    public static void checkGeneralizedEigenvalues() {
        DoubleMatrix A = new DoubleMatrix(3, 3, 2.0, 1.0, 0.0, 1.0, 2.0, 1.0, 0.0, 1.0, 2.0);
        DoubleMatrix B = new DoubleMatrix(3, 3, 4.0, 2.0, 1.0, 2.0, 4.0, 2.0, 1.0, 2.0, 4.0);

        DoubleMatrix[] LA = org.jblas.Eigen.symmetricGeneralizedEigenvectors(A, B);

        check("checkign existence of gsyevd (generalized eigenvalues)...", true);
    }

    public static void checkComplexReturnValues() {
        double[] data = new double[] {
            1.0, 2.0, 3.0, 4.0, 5.0, 6.0
        };

        ComplexDoubleMatrix A = new ComplexDoubleMatrix(data);

        ComplexDouble z = A.dotu(A);

        System.out.print("Checking complex return values... ");
        System.out.println("(z = " + z.toString() + ")");
    }

    public static void main(String[] args) {
        Logger.getLogger().setLevel(Logger.CONFIG);
        for (String arg : args) {
            if (arg.equals("--debug")) {
                Logger.getLogger().setLevel(Logger.DEBUG);
            }
        }
        checkVectorAddition();
        checkMatrixMultiplication();
        checkEigenvalues();
        checkSVD();
        checkComplexReturnValues();
        checkXerbla();
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
