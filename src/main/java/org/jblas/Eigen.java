// --- BEGIN LICENSE BLOCK ---
/* 
 * Copyright (c) 2009-2011, Mikio L. Braun
 *               2011, Nicolas Oury
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

/**
 * <p>Eigenvalue and Eigenvector related functions.</p>
 * <p/>
 * <p>Methods exist for working with symmetric matrices or general eigenvalues.
 * The symmetric versions are usually much faster on symmetric matrices.</p>
 */
public class Eigen {
    private static final DoubleMatrix dummyDouble = new DoubleMatrix(1);

    /**
     * Compute the eigenvalues for a symmetric matrix.
     */
    public static DoubleMatrix symmetricEigenvalues(DoubleMatrix A) {
        A.assertSquare();
        DoubleMatrix eigenvalues = new DoubleMatrix(A.rows);
        int isuppz[] = new int[2 * A.rows];
        SimpleBlas.syevr('N', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, dummyDouble, isuppz);
        return eigenvalues;
    }

    /**
     * Computes the eigenvalues and eigenvectors for a symmetric matrix.
     *
     * @return an array of DoubleMatrix objects containing the eigenvectors
     *         stored as the columns of the first matrix, and the eigenvalues as
     *         diagonal elements of the second matrix.
     */
    public static DoubleMatrix[] symmetricEigenvectors(DoubleMatrix A) {
        A.assertSquare();
        DoubleMatrix eigenvalues = new DoubleMatrix(A.rows);
        DoubleMatrix eigenvectors = A.dup();
        int isuppz[] = new int[2 * A.rows];
        SimpleBlas.syevr('V', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, eigenvectors, isuppz);
        return new DoubleMatrix[]{eigenvectors, DoubleMatrix.diag(eigenvalues)};
    }

    /**
     * Computes the eigenvalues of a general matrix.
     */
    public static ComplexDoubleMatrix eigenvalues(DoubleMatrix A) {
        A.assertSquare();
        DoubleMatrix WR = new DoubleMatrix(A.rows);
        DoubleMatrix WI = WR.dup();
        SimpleBlas.geev('N', 'N', A.dup(), WR, WI, dummyDouble, dummyDouble);

        return new ComplexDoubleMatrix(WR, WI);
    }

    /**
     * Computes the eigenvalues and eigenvectors of a general matrix.
     *
     * @return an array of ComplexDoubleMatrix objects containing the eigenvectors
     *         stored as the columns of the first matrix, and the eigenvalues as the
     *         diagonal elements of the second matrix.
     */
    public static ComplexDoubleMatrix[] eigenvectors(DoubleMatrix A) {
        A.assertSquare();
        // setting up result arrays
        DoubleMatrix WR = new DoubleMatrix(A.rows);
        DoubleMatrix WI = WR.dup();
        DoubleMatrix VR = new DoubleMatrix(A.rows, A.rows);

        SimpleBlas.geev('N', 'V', A.dup(), WR, WI, dummyDouble, VR);

        // transferring the result
        ComplexDoubleMatrix E = new ComplexDoubleMatrix(WR, WI);
        ComplexDoubleMatrix V = new ComplexDoubleMatrix(A.rows, A.rows);
        //System.err.printf("VR = %s\n", VR.toString());
        for (int i = 0; i < A.rows; i++) {
            if (E.get(i).isReal()) {
                V.putColumn(i, new ComplexDoubleMatrix(VR.getColumn(i)));
            } else {
                ComplexDoubleMatrix v = new ComplexDoubleMatrix(VR.getColumn(i), VR.getColumn(i + 1));
                V.putColumn(i, v);
                V.putColumn(i + 1, v.conji());
                i += 1;
            }
        }
        return new ComplexDoubleMatrix[]{V, ComplexDoubleMatrix.diag(E)};
    }

    /**
     * Compute generalized eigenvalues of the problem A x = L B x.
     *
     * @param A symmetric Matrix A. Only the upper triangle will be considered.
     * @param B symmetric Matrix B. Only the upper triangle will be considered.
     * @return a vector of eigenvalues L.
     */
    public static DoubleMatrix symmetricGeneralizedEigenvalues(DoubleMatrix A, DoubleMatrix B) {
        A.assertSquare();
        B.assertSquare();
        DoubleMatrix W = new DoubleMatrix(A.rows);
        SimpleBlas.sygvd(1, 'N', 'U', A.dup(), B.dup(), W);
        return W;
    }

    /**
     * Solve a general problem A x = L B x.
     *
     * @param A symmetric matrix A
     * @param B symmetric matrix B
     * @return an array of matrices of length two. The first one is an array of the eigenvectors X
     *         The second one is A vector containing the corresponding eigenvalues L.
     */
    public static DoubleMatrix[] symmetricGeneralizedEigenvectors(DoubleMatrix A, DoubleMatrix B) {
        A.assertSquare();
        B.assertSquare();
        DoubleMatrix[] result = new DoubleMatrix[2];
        DoubleMatrix dA = A.dup();
        DoubleMatrix dB = B.dup();
        DoubleMatrix W = new DoubleMatrix(dA.rows);
        SimpleBlas.sygvd(1, 'V', 'U', dA, dB, W);
        result[0] = dA;
        result[1] = W;
        return result;
    }

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
    private static final FloatMatrix dummyFloat = new FloatMatrix(1);

    /**
     * Compute the eigenvalues for a symmetric matrix.
     */
    public static FloatMatrix symmetricEigenvalues(FloatMatrix A) {
        A.assertSquare();
        FloatMatrix eigenvalues = new FloatMatrix(A.rows);
        int isuppz[] = new int[2 * A.rows];
        SimpleBlas.syevr('N', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, dummyFloat, isuppz);
        return eigenvalues;
    }

    /**
     * Computes the eigenvalues and eigenvectors for a symmetric matrix.
     *
     * @return an array of FloatMatrix objects containing the eigenvectors
     *         stored as the columns of the first matrix, and the eigenvalues as
     *         diagonal elements of the second matrix.
     */
    public static FloatMatrix[] symmetricEigenvectors(FloatMatrix A) {
        A.assertSquare();
        FloatMatrix eigenvalues = new FloatMatrix(A.rows);
        FloatMatrix eigenvectors = A.dup();
        int isuppz[] = new int[2 * A.rows];
        SimpleBlas.syevr('V', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, eigenvectors, isuppz);
        return new FloatMatrix[]{eigenvectors, FloatMatrix.diag(eigenvalues)};
    }

    /**
     * Computes the eigenvalues of a general matrix.
     */
    public static ComplexFloatMatrix eigenvalues(FloatMatrix A) {
        A.assertSquare();
        FloatMatrix WR = new FloatMatrix(A.rows);
        FloatMatrix WI = WR.dup();
        SimpleBlas.geev('N', 'N', A.dup(), WR, WI, dummyFloat, dummyFloat);

        return new ComplexFloatMatrix(WR, WI);
    }

    /**
     * Computes the eigenvalues and eigenvectors of a general matrix.
     *
     * @return an array of ComplexFloatMatrix objects containing the eigenvectors
     *         stored as the columns of the first matrix, and the eigenvalues as the
     *         diagonal elements of the second matrix.
     */
    public static ComplexFloatMatrix[] eigenvectors(FloatMatrix A) {
        A.assertSquare();
        // setting up result arrays
        FloatMatrix WR = new FloatMatrix(A.rows);
        FloatMatrix WI = WR.dup();
        FloatMatrix VR = new FloatMatrix(A.rows, A.rows);

        SimpleBlas.geev('N', 'V', A.dup(), WR, WI, dummyFloat, VR);

        // transferring the result
        ComplexFloatMatrix E = new ComplexFloatMatrix(WR, WI);
        ComplexFloatMatrix V = new ComplexFloatMatrix(A.rows, A.rows);
        //System.err.printf("VR = %s\n", VR.toString());
        for (int i = 0; i < A.rows; i++) {
            if (E.get(i).isReal()) {
                V.putColumn(i, new ComplexFloatMatrix(VR.getColumn(i)));
            } else {
                ComplexFloatMatrix v = new ComplexFloatMatrix(VR.getColumn(i), VR.getColumn(i + 1));
                V.putColumn(i, v);
                V.putColumn(i + 1, v.conji());
                i += 1;
            }
        }
        return new ComplexFloatMatrix[]{V, ComplexFloatMatrix.diag(E)};
    }

    /**
     * Compute generalized eigenvalues of the problem A x = L B x.
     *
     * @param A symmetric Matrix A. Only the upper triangle will be considered.
     * @param B symmetric Matrix B. Only the upper triangle will be considered.
     * @return a vector of eigenvalues L.
     */
    public static FloatMatrix symmetricGeneralizedEigenvalues(FloatMatrix A, FloatMatrix B) {
        A.assertSquare();
        B.assertSquare();
        FloatMatrix W = new FloatMatrix(A.rows);
        SimpleBlas.sygvd(1, 'N', 'U', A.dup(), B.dup(), W);
        return W;
    }

    /**
     * Solve a general problem A x = L B x.
     *
     * @param A symmetric matrix A
     * @param B symmetric matrix B
     * @return an array of matrices of length two. The first one is an array of the eigenvectors X
     *         The second one is A vector containing the corresponding eigenvalues L.
     */
    public static FloatMatrix[] symmetricGeneralizedEigenvectors(FloatMatrix A, FloatMatrix B) {
        A.assertSquare();
        B.assertSquare();
        FloatMatrix[] result = new FloatMatrix[2];
        FloatMatrix dA = A.dup();
        FloatMatrix dB = B.dup();
        FloatMatrix W = new FloatMatrix(dA.rows);
        SimpleBlas.sygvd(1, 'V', 'U', dA, dB, W);
        result[0] = dA;
        result[1] = W;
        return result;
    }

//END
}
