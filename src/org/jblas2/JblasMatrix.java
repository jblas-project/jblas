/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 * The parent matrix for all jblas-internal matrices.
 */
public abstract class JblasMatrix implements Matrix {

    private Type type;
    private int columns;
    private int rows;
    private int size;

    public JblasMatrix(int r, int c, Type t) {
        rows = r;
        columns = c;
        size = r * c;
        type = t;
    }

    public JblasMatrix(int l, Type t) {
        this(l, 1, t);
    }

    public JblasMatrix(int r, int c) {
        this(r, c, DoubleMatrix.TYPE);
    }

    public JblasMatrix(int l) {
        this(l, DoubleMatrix.TYPE);
    }

    public Matrix set(double... data) {
        for (int i = 0; i < data.length; i++) {
            put(i, data[i]);
        }

        return this;
    }

    public int columns() {
        return columns;
    }

    public int rows() {
        return rows;
    }

    public int size() {
        return size;
    }

    public Type type() {
        return type;
    }

    public Matrix add(Matrix other) {
        return binop(this, other, new Add());
    }

    private static final Add add = new Add();

    public Matrix addi(Matrix other, Matrix result) {
        Type common = type().commonType(other.type());
        Matrix left = convertTo(common);
        Matrix right = other.convertTo(common);
        result = result.convertTo(common);
        return left.computeBinop(add, right, result);
        //return binop(this, other, result, new Add());
    }

    public Matrix sub(Matrix other) {
        return binop(this, other, new Sub());
    }

    public Matrix mul(Matrix other) {
        return binop(this, other, new Mul());
    }

    public Matrix div(Matrix other) {
        return binop(this, other, new Div());
    }

    public Matrix mmul(Matrix other) {
        return binop(this, other, new Mmul());
    }

    protected Matrix binop(Matrix l, Matrix r, Binop op) {
        Type common = l.type().commonType(r.type());
        Matrix left = l.convertTo(common);
        Matrix right = r.convertTo(common);
        Matrix result = op.createResult(left, right);
        return left.computeBinop(op, right, result);
    }

    public static Matrix binop(Matrix l, Matrix r, Matrix result, Binop op) {
        Type common = l.type().commonType(r.type());
        Matrix left = l.convertTo(common);
        Matrix right = r.convertTo(common);
        result = result.convertTo(common);
        return left.computeBinop(op, right, result);
    }

    public Matrix convertTo(Type t) {
        if (type().isSame(t)) {
            return this;
        } else {
            Matrix result = Functions.matrix(rows(), columns(), t);

            this.compute(new Copy(result));

            return result;
        }
    }

    @Override
    public String toString() {
        StringBuilder out = new StringBuilder();

        out.append("[");
        for (int i = 0; i < rows(); i++) {
            for (int j = 0; j < columns(); j++) {
                out.append(get(i, j));
                if (j < columns() - 1) {
                    out.append(", ");
                }
            }
            if (i < rows() - 1) {
                out.append("; ");
            }
        }
        out.append("]");
        return out.toString();
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof Matrix) {
            Matrix m = (Matrix) o;
            if (rows() != m.rows() || columns() != m.columns()) {
                return false;
            }

            for (int i = 0; i < size(); i++) {
                if (!get(i).equals(m.get(i))) {
                    return false;
                }
            }
            return true;
        } else {
            return false;
        }
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 23 * hash + this.columns;
        hash = 23 * hash + this.rows;
        for (int i = 0; i < size(); i++) {
            hash = 23 * hash + get(i).hashCode();
        }

        return hash;
    }
}
