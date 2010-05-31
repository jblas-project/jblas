/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2;

/**
 *
 */
abstract class ElementwiseBinop implements Binop {
    public Matrix createResult(Matrix left, Matrix right) {
        return Functions.matrix(left.rows(), left.columns(), left.type());
    }
}
