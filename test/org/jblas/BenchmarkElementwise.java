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
import static org.jblas.DoubleMatrix.*;

public class BenchmarkElementwise extends TestCase {
	public void testMuli() {
		int SIZE = 1000;
		int ITERS = 1000000;
		DoubleMatrix x = rand(SIZE);
		DoubleMatrix y = rand(SIZE);
		DoubleMatrix z = zeros(SIZE);
		
		tic("muli():");
		for (int i = 0; i < ITERS; i++)
			x.muli(y, z);
		toc();

		double xa[]= new double[SIZE];
		double ya[]= new double[SIZE];
		double za[]= new double[SIZE];
		
		tic("muli (array):");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				za[j] = xa[j] * ya[j];
		toc();

		/*tic("muli (double buffer:");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				z.data.put(j, x.data.get(j) * y.data.get(j));
		toc();*/
		
		tic("divi():");
		for (int i = 0; i < ITERS/10; i++)
			x.divi(y, z);
		toc();

		tic("addi():");
		for (int i = 0; i < ITERS; i++)
			x.addi(y, z);
		toc();

		tic("addi() array:");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				za[j] = xa[j] + ya[j];
		toc();

		/*tic("addi() doublebuffer:");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				z.data.put(j, x.data.get(j) + y.data.get(j));
		toc();*/
		
		tic("subi():");
		for (int i = 0; i < ITERS; i++)
			x.subi(y, z);
		toc();
	}
}
