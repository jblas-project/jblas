/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2;

import junit.framework.TestCase;

/**
 *
 * @author mikio
 */
public class TestMatrix extends TestCase {

    public TestMatrix(String testName) {
        super(testName);
    }

    public void testConstruction() {
        assertEquals(FloatMatrix.TYPE, Functions.matrix(3, 3, FloatMatrix.TYPE).type());
        assertEquals(DoubleMatrix.TYPE, Functions.matrix(3, 3, DoubleMatrix.TYPE).type());
    }

    public void testConvert() {
        Matrix f = Functions.matrix(3, 3, FloatMatrix.TYPE);
        assertEquals(FloatMatrix.TYPE, f.convertTo(FloatMatrix.TYPE).type());
        assertTrue(f.convertTo(FloatMatrix.TYPE) instanceof FloatMatrix);
        assertEquals(DoubleMatrix.TYPE, f.convertTo(DoubleMatrix.TYPE).type());
        assertTrue(f.convertTo(DoubleMatrix.TYPE) instanceof DoubleMatrix);
    }

    public void testAdd() {
        Matrix a = Functions.matrix(3, 3, FloatMatrix.TYPE).set(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix b = Functions.matrix(3, 3, DoubleMatrix.TYPE).set(9, 8, 7, 6, 5, 4, 3, 2, 1);

        Matrix c = a.add(b);

        assertEquals(DoubleMatrix.TYPE, a.add(b).type());

        for (int i = 0; i < 9; i++) {
            assertEquals(10.0, c.getDouble(i));
        }
    }

    public void testMul() {
        Matrix a = Functions.matrix(3, 3).set(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Matrix b = Functions.matrix(3, 3).set(0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9);

        Matrix c = a.mul(b);

        for (int i = 0; i < 9; i++) {
            assertEquals(a.getDouble(i) * b.getDouble(i), c.getDouble(i));
        }
    }

    public void testMmul() {
        Matrix a = Functions.matrix(4, 3).set(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        Matrix b = Functions.matrix(3).set(1, 2, 3);

        Matrix c = Functions.matrix(4, 1);
        for (int i = 0; i < a.rows(); i++)
            for (int j = 0; j < a.columns(); j++)
                c.put(i, c.getDouble(i) + a.getDouble(i, j)*b.getDouble(j));

        Matrix c2 = a.mmul(b);

        System.out.println(a);
        System.out.println(b);
        System.out.println(c);
        System.out.println(c2);

        assertEquals(4, c2.rows());
        assertEquals(1, c2.columns());
        for (int i = 0; i < 3; i++)
            assertEquals(c.getDouble(i), c2.getDouble(i));
    }
}
