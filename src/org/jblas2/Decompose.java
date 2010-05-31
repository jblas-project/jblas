/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

/**
 *
 */
public class Decompose {

    public static Matrix lup(Matrix x) {
        return x.compute(new LUP());
    }
}
