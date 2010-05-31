/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
public abstract class DenseMatrix extends JblasMatrix {
    public DenseMatrix(int r, int c, Type t) {
        super(r, c, t);
    }

    protected int index(int i, int j) {
        return i + j * rows();
    }
}
