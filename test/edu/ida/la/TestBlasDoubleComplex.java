package edu.ida.la;

import junit.framework.TestCase;

import java.nio.DoubleBuffer;


import edu.ida.core.ComplexDouble;
import edu.ida.la.Blas;
import static edu.ida.core.BlasUtil.createDoubleBufferFrom;

public class TestBlasDoubleComplex extends TestCase {

	public void testDotc() {
		DoubleBuffer a = createDoubleBufferFrom(1.0, 1.0, 2.0, 0.0, 3.0, 0.0);
		
		ComplexDouble c = Blas.zdotu(3, a, 0, 1, a, 0, 1);
		System.out.println(c);
	}

	public void testAxpy() {
		DoubleBuffer x = createDoubleBufferFrom(0.0, -1.0);
		DoubleBuffer y = createDoubleBufferFrom(0.0, 1.0);
		ComplexDouble a = new ComplexDouble(0.0, 1.0);
		
		Blas.zdscal(1, 2.0, x, 0, 1);
		assertEquals(new ComplexDouble(0.0, -2.0), new ComplexDouble(x.get(0), x.get(1)));
		
		Blas.zaxpy(1, a, x, 0, 1, y, 0, 1);
		assertEquals(new ComplexDouble(2.0, 1.0), new ComplexDouble(y.get(0), y.get(1)));
	}
}
