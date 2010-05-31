/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
public interface Matrix {
    public Type type();
    public int columns();
    public int rows();
    public int size();

    public Matrix set(double... data);

    public void put(int i, float v);
    public void put(int i, double v);
    public void put(int i, int j, float v);
    public void put(int i, int j, double v);
    
    public float getFloat(int i);
    public double getDouble(int j);
    public float getFloat(int i, int j);
    public double getDouble(int i, int j);
    public Object get(int i);
    public Object get(int i, int j);

    public Matrix convertTo(Type t);
    public Matrix computeBinop(Binop op, Matrix r, Matrix result);
    public <T> T compute(MatrixFunction<T> fct);

    public Matrix add(Matrix other);
    public Matrix addi(Matrix other, Matrix result);
    public Matrix sub(Matrix other);
    public Matrix mul(Matrix other);
    public Matrix div(Matrix other);
    public Matrix mmul(Matrix other);
}
