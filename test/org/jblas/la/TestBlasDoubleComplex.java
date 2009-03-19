package org.jblas.la;

import junit.framework.TestCase;

import java.nio.DoubleBuffer;


import org.jblas.core.ComplexDouble;
import org.jblas.la.Blas;
import static org.jblas.core.BlasUtil.createDoubleBufferFrom;

public class TestBlasDoubleComplex extends TestCase {

	public void testDotc() {
		double[] a = new double[] { 1.0, 1.0, 2.0, 0.0, 3.0, 0.0 };
		
		ComplexDouble c = Blas.zdotu(3, a, 0, 1, a, 0, 1);
		System.out.println(c);
	}

	public void testAxpy() {
		double[] x = new double[] { 0.0, -1.0 };
		double[] y = new double[] { 0.0, 1.0 };
		ComplexDouble a = new ComplexDouble(0.0, 1.0);
		
		Blas.zdscal(1, 2.0, x, 0, 1);
		assertEquals(new ComplexDouble(0.0, -2.0), new ComplexDouble(x[0], x[1]));
		
		Blas.zaxpy(1, a, x, 0, 1, y, 0, 1);
		assertEquals(new ComplexDouble(2.0, 1.0), new ComplexDouble(y[0], y[1]));
	}
}
