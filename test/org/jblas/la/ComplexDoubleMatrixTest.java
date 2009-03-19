/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.la;

import org.jblas.la.DoubleMatrix;
import org.jblas.la.ComplexDoubleMatrix;
import org.jblas.core.ComplexDouble;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import junit.framework.TestCase;

/**
 *
 * @author mikio
 */
public class ComplexDoubleMatrixTest extends TestCase {
    
    public ComplexDoubleMatrixTest(String testName) {
        super(testName);
    }            

    @Override
    protected void setUp() throws Exception {
        super.setUp();
    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testConstruction() {
        ComplexDoubleMatrix A = new ComplexDoubleMatrix(3, 3);
        
        for (int i = 0; i < A.rows; i++)
            for (int j = 0; j < A.columns; j++)
                A.put(i, j, new ComplexDouble(i, j));
        System.out.printf("A = %s\n", A.toString());
        
        System.out.println(A.mmul(A));
        
        DoubleMatrix R = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
        A = new ComplexDoubleMatrix(R, R.transpose());
        System.out.println(A);
        
        assertEquals(A.real(), R);
        assertEquals(A.imag(), R.transpose());
    }
}
