package org.jblas.la;

import org.jblas.la.DoubleMatrix;
import junit.framework.TestCase;
import static org.jblas.la.MatrixFunctions.*;

public class TestMatrixFunctions extends TestCase {
	public void testAbs() {
		DoubleMatrix m = new DoubleMatrix(1, 2, 1.0, -1.0);
		
		assertEquals(new DoubleMatrix(1, 2, 1.0, 1.0), abs(m));
	}
}
