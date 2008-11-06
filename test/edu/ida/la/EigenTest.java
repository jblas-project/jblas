/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package edu.ida.la;

import junit.framework.TestCase;
import static edu.ida.la.TicToc.*;

/**
 *
 * @author mikio
 */
public class EigenTest extends TestCase {

    public EigenTest(String testName) {
        super(testName);
    }

    public void testEigenvalues() {
        DoubleMatrix A = new DoubleMatrix(2, 2, 3.0, -3.0, 1.0, 1.0);

        ComplexDoubleMatrix E = Eigen.eigenvalues(A);
        
        System.out.printf("E = %s\n", E.toString());
        
        ComplexDoubleMatrix[] EV = Eigen.eigenvectors(A);
        
        System.out.printf("values = %s\n", EV[1].toString());
        System.out.printf("vectors = %s\n", EV[0].toString());
    }
    
    public void testBenchmarkEigenvalues() {
        tic("Computing eigenvalues of random 1000*1000 matrix");
        ComplexDoubleMatrix EV = Eigen.eigenvalues(DoubleMatrix.randn(500,500));
        toc();
        
        System.out.printf("EV = %s\n", EV.toString());
    }
}
