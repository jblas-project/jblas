package org.jblas.la;

import org.jblas.la.DoubleMatrix;
import org.jblas.la.Geometry;
import junit.framework.TestCase;

public class TestGeometry extends TestCase {
	public void testCenter() {
		DoubleMatrix x = new DoubleMatrix(3, 1, 1.0, 2.0, 3.0);
		
		Geometry.center(x);
		
		assertEquals(new DoubleMatrix(3, 1, -1.0, 0.0, 1.0), x);
		
		DoubleMatrix M = new DoubleMatrix(new double[][] {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 9.0}});
		
		//M.print();
		
		DoubleMatrix MR = Geometry.centerRows(M.dup());
		DoubleMatrix MC = Geometry.centerColumns(M.dup());
		
		//MR.print();
		//MC.print();
		
		assertEquals(new DoubleMatrix(new double[][] {{-1.0, 0.0, 1.0}, {-1.0, 0.0, 1.0}, {-1.0, 0.0, 1.0}}), MR);
		assertEquals(new DoubleMatrix(new double[][] {{-3.0, -3.0, -3.0}, {0.0, 0.0, 0.0}, {3.0, 3.0, 3.0}}), MC);
	}				
	
	public void testPwDist() {
		DoubleMatrix M = new DoubleMatrix(3, 5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0);
		
		DoubleMatrix D = Geometry.pairwiseSquaredDistances(M, M);
		
		D.print();
		
		M = M.transpose();
				
		D = Geometry.pairwiseSquaredDistances(M, M);
		
		D.print();
	}
}
