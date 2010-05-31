/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
public class Functions {
    public static final Type DOUBLE = DoubleMatrix.TYPE;
    public static final Type FLOAT = FloatMatrix.TYPE;

    public static Matrix matrix(int r, int c, Type t) {
        if (t.equals(FloatMatrix.TYPE)) {
            return new FloatMatrix(r, c);
        } else if(t.equals(DoubleMatrix.TYPE)) {
            return new DoubleMatrix(r, c);
        } else {
            throw new IllegalArgumentException("Cannot create matrix for type " + t + ".");
        }
    }

    public static Matrix matrix(int r, int c) {
        return matrix(r, c, DOUBLE);
    }

    public static Matrix matrix(int r, Type t) {
        return matrix(r, 1, t);
    }

    public static Matrix matrix(int r) {
        return matrix(r, DOUBLE);
    }
}
