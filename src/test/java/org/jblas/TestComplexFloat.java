// --- BEGIN LICENSE BLOCK ---
/* 
 * Copyright (c) 2009, Mikio L. Braun
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the Technische Universität Berlin nor the
 *       names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// --- END LICENSE BLOCK ---

package org.jblas;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class TestComplexFloat  {

	private ComplexFloat a, b;

  private final double eps = 1e-16;

  @Before
	public void setUp() {
		a = new ComplexFloat(1, 2);
		b = new ComplexFloat(3, 4);
	}

  @Test
	public void testAdd() {
		ComplexFloat c = a.add(b);
		
		assertEquals(4.0f, c.real(), eps);
		assertEquals(6.0f, c.imag(), eps);
	}

  @Test
	public void testMul() {
		ComplexFloat c = a.mul(b);
		
		assertEquals(-5.0f, c.real(), eps);
		assertEquals(10.0f, c.imag(), eps);
	}

  @Test
	public void testMulAndDiv() {
		ComplexFloat d = a.mul(b).div(b);
		
		assertEquals(new ComplexFloat(1.0f, 2.0f), d);
		
		d = a.mul(b).mul(b.inv());

		assertEquals(new ComplexFloat(1.0f, 2.0f), d);
	}

  @Test
	public void testDivByZero() {
		a.div(new ComplexFloat(0.0f, 0.0f));
	}

  @Test
  public void testSqrt() {
    assertEquals(new ComplexFloat(0.0f, 1.0f), new ComplexFloat(-1.0f, 0.0f).sqrt());
    assertEquals(new ComplexFloat(3.0f, 0.0f), new ComplexFloat(9.0f).sqrt());
    assertEquals(new ComplexFloat(0.0f, 3.0f), new ComplexFloat(-9.0f).sqrt());
    assertEquals(new ComplexFloat(1.0f, -4.0f), new ComplexFloat(-15.0f, -8.0f).sqrt());
  }
}
