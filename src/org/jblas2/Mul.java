/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
class Mul extends ElementwiseBinop {

public Matrix compute(FloatMatrix left, FloatMatrix right, FloatMatrix result) {
        for (int i = 0; i < result.size(); i++) {
            result.put(i, left.getFloat(i) * result.getFloat(i));
        }

        return result;
    }

    public Matrix compute(DoubleMatrix left, DoubleMatrix right, DoubleMatrix result) {
        for (int i = 0; i < result.size(); i++) {
            result.put(i, left.getDouble(i) * right.getDouble(i));
        }

        return result;
    }
}
