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
