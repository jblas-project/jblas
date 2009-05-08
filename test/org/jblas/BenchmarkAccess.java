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

import static org.jblas.TicToc.*;

public class BenchmarkAccess extends TestCase {
	public void testArrayVsDirectBuffer() {
		System.out.println("Testing array access versus direct buffer access");
		
		int SIZE = 100;
		int ITERS = 10000000;
		DoubleMatrix ma = new DoubleMatrix(1, SIZE);
		double[] aa = new double[SIZE];
		DoubleMatrix mb = new DoubleMatrix(1, SIZE);
		double[] ab = new double[SIZE];
		
		tic("double[]:");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				aa[i] = ab[i];
			}
		toc();
		
		/*tic("DoubleBuffer.put()");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				mb.data.put(i, ma.data.get(i));
			}
		toc();*/

		tic("DoubleMatrix.put()");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				mb.put(i, ma.get(i));
			}
		toc();
		
		tic("DoubleMatrix.put() (two-dim)");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				mb.put(0, i, ma.get(0, i));
			}
		toc();
		
		tic("NativeBlas.dcopy");
		for (int j = 0; j < ITERS; j++)
			NativeBlas.dcopy(SIZE, mb.data, 0, 1, ma.data, 0, 1);
		toc();
		
		System.out.println("Done");
	}
}
