/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
public class Type {

    private int precision;
    private boolean complex;
    public final static int FLOAT = 0;
    public final static int DOUBLE = 1;
    public final static boolean REAL = false;
    public final static boolean COMPLEX = true;
    private String[] precisionNames = {
        "float", "double"
    };

    public Type(int precision, boolean complex) {
        this.precision = precision;
        this.complex = complex;
    }

    public int precision() {
        return precision;
    }

    public boolean isComplex() {
        return complex;
    }

    /***
     * Compute the common type where this and that
     * can be converted to.
     */
    public Type commonType(Type other) {
        if (isSame(other))
            return this;
        int commonPrec = precision;
        if (other.precision() > commonPrec) {
            commonPrec = other.precision();
        }
        boolean commonComplex = complex || other.isComplex();
        return new Type(commonPrec, commonComplex);
    }

    public boolean isSame(Type other) {
        return (precision() == other.precision())
                && (isComplex() == other.isComplex());
    }

    @Override
    public boolean equals(Object o) {
        if (o instanceof Type) {
            Type that = (Type) o;
            return (precision() == that.precision())
                    && (isComplex() == that.isComplex());
        } else {
            return false;
        }
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 23 * hash + this.precision;
        hash = 23 * hash + (this.complex ? 1 : 0);
        return hash;
    }

    @Override
    public String toString() {
        return (isComplex() ? "complex " : "real ") + precisionNames[precision()];
    }
}
