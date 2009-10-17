/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.ranges;


import junit.framework.TestCase;
import org.jblas.DoubleMatrix;

/**
 *
 * @author mikio
 */
public class IntervalRangeTest extends TestCase {

    DoubleMatrix A;

    public IntervalRangeTest(String testName) {
        super(testName);
    }
    
    @Override
    public void setUp() {
            A = new DoubleMatrix(4, 3,
                    1.0, 2.0, 3.0, 4.0,
                    5.0, 6.0, 7.0, 8.0,
                    9.0, 10.0, 11.0, 12.0).transpose();
    }

    public void testGetRows() {
        DoubleMatrix ARows1to2 = new DoubleMatrix(2, 4,
                5.0, 9.0,
                6.0, 10.0,
                7.0, 11.0,
                8.0, 12.0);
        assertEquals(ARows1to2, A.getRows(new IntervalRange(1, 3)));
    }
}
