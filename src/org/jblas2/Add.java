/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
public class Add extends ElementwiseBinop {

    private void checkSizes(Matrix l, Matrix r) {
        if (l.size() != r.size()) {
            throw new MatrixSizeException();
        }
    }

    public Matrix compute(FloatMatrix left, FloatMatrix right, FloatMatrix result) {
        for (int i = 0; i < result.size(); i++) {
            result.put(i, left.getFloat(i) + result.getFloat(i));
        }

        return result;
    }

    public Matrix compute(DoubleMatrix left, DoubleMatrix right, DoubleMatrix result) {
        double[] o = result.data();
        double[] l = left.data();
        double[] r = right.data();
        int n = result.size();

        for (int i = 0; i < n; i++) {
            // result.put(i, left.getDouble(i) + right.getDouble(i));
            o[i] = l[i] + r[i];
        }

        //org.jblas.JavaBlas.rzgxpy(n, o, l, r);

        return result;
    }
}
