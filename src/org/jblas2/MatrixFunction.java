/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
public interface MatrixFunction<T> {
    public T compute(FloatMatrix m);
    public T compute(DoubleMatrix m);
}
