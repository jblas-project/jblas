/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

import junit.framework.TestCase;

/**
 *
 * @author mikio
 */
public class SimpleBlasTest extends TestCase {
    
    public SimpleBlasTest(String testName) {
        super(testName);
    }

    public void testGeev() {
        DoubleMatrix A = new DoubleMatrix(2, 2, 3.0, -3.0, 1.0, 1.0);
        DoubleMatrix WR = new DoubleMatrix(2);
        DoubleMatrix WI = new DoubleMatrix(2);
        DoubleMatrix VR = new DoubleMatrix(2,2);
        DoubleMatrix VL = new DoubleMatrix(2,2);
        
        SimpleBlas.geev('V', 'N', A, WR, WI, VR, VL);
        
        assertEquals(new DoubleMatrix(2, 1, 2.0, 2.0), WR);
        assertEquals(new DoubleMatrix(2, 1, Math.sqrt(2.0), -Math.sqrt(2.0)), WI);
        
        System.out.printf("WR = %s\n", WR.toString());
        System.out.printf("WI = %s\n", WI.toString());
        System.out.printf("VR = %s\n", VR.toString());
        System.out.printf("VL = %s\n", VL.toString());
        System.out.printf("A = %s\n", A.toString());
        
    }
}
