/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
public class DoubleMatrix extends DenseMatrix {
    public static final Type TYPE = new Type(Type.DOUBLE, Type.REAL);
    private double[] data;

    public DoubleMatrix(int r, int c) {
        super(r, c, TYPE);
        data = new double[size()];
    }

    public void put(int i, float v) {
        data[i] = v;
    }

    public void put(int i, double v) {
        data[i] = v;
    }

    public void put(int i, int j, float v) {
        data[index(i,j)] = v;
    }

    public void put(int i, int j, double v) {
        data[index(i,j)] = v;
    }

    public float getFloat(int i) {
        return (float) data[i];
    }

    public double getDouble(int i) {
        return (double) data[i];
    }

    public float getFloat(int i, int j) {
        return (float) data[index(i,j)];
    }

    public double getDouble(int i, int j) {
        return data[index(i,j)];
    }

    public Object get(int i) {
        return new Double(data[i]);
    }

    public Object get(int i, int j) {
        return new Double(data[index(i,j)]);
    }

    public <T> T compute(MatrixFunction<T> fct) {
        return fct.compute(this);
    }

    double[] data() {
        return data;
    }

    public Matrix computeBinop(Binop op, Matrix r, Matrix result) {
        return op.compute(this, (DoubleMatrix) r, (DoubleMatrix) result);
    }
}
