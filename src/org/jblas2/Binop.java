/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
public interface Binop {

    public Matrix createResult(Matrix left, Matrix right);

    public Matrix compute(FloatMatrix left, FloatMatrix right, FloatMatrix result);

    public Matrix compute(DoubleMatrix left, DoubleMatrix right, DoubleMatrix result);
}
