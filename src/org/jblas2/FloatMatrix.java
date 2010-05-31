/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
public class FloatMatrix extends DenseMatrix {

    public static final Type TYPE = new Type(Type.FLOAT, Type.REAL);
    private float[] data;

    public FloatMatrix(int r, int c) {
        super(r, c, TYPE);
        data = new float[size()];
    }

    float[] data() {
        return data;
    }

    public void put(int i, float v) {
        data[i] = v;
    }

    public void put(int i, double v) {
        data[i] = (float) v;
    }

    public void put(int i, int j, float v) {
        data[i + j * rows()] = v;
    }

    public void put(int i, int j, double v) {
        data[i + j * rows()] = (float) v;
    }

    public float getFloat(int i) {
        return data[i];
    }

    public double getDouble(int i) {
        return (double) data[i];
    }

    public float getFloat(int i, int j) {
        return data[i + j * rows()];
    }

    public double getDouble(int i, int j) {
        return (double) data[i + j * rows()];
    }

    public Object get(int i) {
        return new Float(data[i]);
    }

    public Object get(int i, int j) {
        return new Float(data[i + j * rows()]);
    }

    public <T> T compute(MatrixFunction<T> fct) {
        return fct.compute(this);
    }

    public Matrix computeBinop(Binop op, Matrix r, Matrix result) {
        return op.compute(this, (FloatMatrix) r, (FloatMatrix) result);
    }
}
