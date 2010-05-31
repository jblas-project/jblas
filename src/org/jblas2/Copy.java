/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
class Copy implements MatrixFunction<Matrix> {
    Matrix target;
    
    Copy(Matrix t) {
        target = t;
    }

    public Matrix compute(FloatMatrix m) {
        for (int i = 0; i < m.size(); i++) {
            target.put(i, m.getFloat(i));
        }
        return target;
    }

    public Matrix compute(DoubleMatrix m) {
        for (int i = 0; i < m.size(); i++) {
            target.put(i, m.getDouble(i));
        }
        return target;
    }
}
