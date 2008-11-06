package edu.ida.la;

import junit.framework.TestCase;

import edu.ida.la.*;

public class TestSolve extends TestCase {
	public void testFaultySolveSymmetric() {
		DoubleMatrix A = new DoubleMatrix(3, 3, 2d, 1d, 0d, 2d, 1d, 2d, 0d, 1d, 2d);
		DoubleMatrix x = new DoubleMatrix(1, 3, 1d, 2d, 3d);
		
		Solve.solveSymmetric(A, x);
	}
}
