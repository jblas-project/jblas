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

public class BenchmarkComplex extends TestCase {
    public void testMul() {
        int ITERS = 50000;
        int SIZE = 100;
        ComplexDoubleMatrix A = new ComplexDoubleMatrix(SIZE, SIZE);
        ComplexDoubleMatrix B = new ComplexDoubleMatrix(SIZE, SIZE);
        ComplexDoubleMatrix C = new ComplexDoubleMatrix(SIZE, SIZE);
        
        tic("Multiplying two matrices %d times", ITERS);
        for (int i = 0; i < ITERS; i++)
            A.muli(B, C);
        printMflops(6L*SIZE*SIZE*ITERS, toc());
        
        tic("Doing it with temp variables");
        for (int i = 0; i < ITERS; i++)
            for (int j = 0; j < A.length; j++)
                C.put(j, A.get(j).mul(B.get(j)));
        printMflops(6L*SIZE*SIZE*ITERS, toc());
        
        tic("Doing it by hand");
        for (int i = 0; i < ITERS; i++)
            for (int j = 0; j < 2*A.length; j+=2) {
                double ar = A.data[j], ai = A.data[j+1];
                double br = B.data[j], bi = B.data[j+1];
                double cr = ar * br - ai * bi;
                double ci = ar * bi + ai * br;
                C.data[j] = cr;
                C.data[j+1] = ci;
            }
        printMflops(6L*SIZE*SIZE*ITERS, toc());
    }
    
    private void printMflops(long ops, double time) {
        System.out.printf("  [INFO] %.3f MFLOPS (%.1f million operations)\n", ops / time / 1e6d, ops / 1e6d);        
    }

}
