package org.jblas.core;

import org.jblas.core.ComplexFloat;
import junit.framework.TestCase;

public class TestComplexFloat extends TestCase {
    public TestComplexFloat() {
    }

	private ComplexFloat a, b;
	
	public void setUp() {
		a = new ComplexFloat(1, 2);
		b = new ComplexFloat(3, 4);
	}
	
	public void testAdd() {
		ComplexFloat c = a.add(b);
		
		assertEquals(4.0f, c.real());
		assertEquals(6.0f, c.imag());
	}

	public void testMul() {
		ComplexFloat c = a.mul(b);
		
		assertEquals(-5.0f, c.real());
		assertEquals(10.0f, c.imag());
	}
	
	public void testMulAndDiv() {
		ComplexFloat d = a.mul(b).div(b);
		
		assertEquals(new ComplexFloat(1.0f, 2.0f), d);
		
		d = a.mul(b).mul(b.inv());

		assertEquals(new ComplexFloat(1.0f, 2.0f), d);
	}
	
	public void testDivByZero() {
		a.div(new ComplexFloat(0.0f, 0.0f));
	}
}
