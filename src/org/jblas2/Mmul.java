/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
class Mmul implements Binop {

    public Matrix compute(FloatMatrix left, FloatMatrix right, FloatMatrix result) {
        org.jblas.NativeBlas.sgemm('N', 'N', left.rows(), right.columns(), left.columns(), 1.0f, left.data(), 0, left.rows(), right.data(), 0, right.rows(), 0.0f, result.data(), 0, result.rows());

        return result;
    }

    public Matrix compute(DoubleMatrix left, DoubleMatrix right, DoubleMatrix result) {
        org.jblas.NativeBlas.dgemm('N', 'N', left.rows(), right.columns(), left.columns(), 1.0, left.data(), 0, left.rows(), right.data(), 0, right.rows(), 0.0, result.data(), 0, result.rows());

        return result;
    }

    public Matrix createResult(Matrix left, Matrix right) {
        return Functions.matrix(left.rows(), right.columns(), left.type());
    }
}
