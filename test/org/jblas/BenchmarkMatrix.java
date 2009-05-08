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

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas;

import junit.framework.TestCase;

import static org.jblas.TicToc.*;

/**
 *
 * @author mikio
 */
public class BenchmarkMatrix extends TestCase {
    
    private void printMflops(long ops, double time) {
        System.out.printf("  [INFO] %.3f MFLOPS (%.1f million operations)\n", ops / time / 1e6d, ops / 1e6d);        
    }
    
    public void setUp() {
    }
    
    public void testBenchmarkMMulDouble() {
        final int n = 1000;
        DoubleMatrix x, y, z;
        x = DoubleMatrix.randn(n, n);
        y = DoubleMatrix.randn(n, n);
        z = DoubleMatrix.randn(n, n);        

        double time;
        
        tic("Multiplying DOUBLE matrices of size %d", n);
        SimpleBlas.gemm(1.0, x, y, 0.0, z);
        time = toc();
        
        long ops = 2L*n*n*n;
        printMflops(ops, time);
    }

    public void testBenchmarkMMulFloat()  {
        final int n = 1000;
        FloatMatrix x = FloatMatrix.randn(n, n);
        FloatMatrix y = FloatMatrix.randn(n, n);
        FloatMatrix z = FloatMatrix.randn(n, n);
        double time;
        
        tic("Multiplying FLOAT matrices of size %d", n);
        SimpleBlas.gemm(1.0f, x, y, 0.0f, z);
        printMflops(2L*n*n*n, toc());
    }
    
    public void testBenchmarkAxpyAndDot() {
        final int n = 10000;
        final int ITERS = 10000;
        DoubleMatrix x = DoubleMatrix.randn(n);
        DoubleMatrix y = DoubleMatrix.randn(n);
        
        tic("Doing axpy with %d elements %d times", n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.axpy(1.0, x, y);
        printMflops(n * ITERS, toc());

        tic("Doing dot with %d elements %d times", n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.dot(x, y);
        printMflops(n * ITERS, toc());
    }    

    public void testBenchmarkAxpyAndDotFloat() {
        final int n = 10000;
        final int ITERS = 10000;
        FloatMatrix x = FloatMatrix.randn(n);
        FloatMatrix y = FloatMatrix.randn(n);
        
        tic("Doing FLOAT axpy with %d elements %d times", n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.axpy(1.0f, x, y);
        printMflops(n * ITERS, toc());

        tic("Doing FLOAT dot with %d elements %d times", n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.dot(x, y);
        printMflops(n * ITERS, toc());
    }
    
    public void testBenchmarkGemvDouble() {
        final int m = 1000;
        final int n = 2000;
        final int ITERS = 100;
        DoubleMatrix x = DoubleMatrix.randn(m, n);
        DoubleMatrix y = DoubleMatrix.randn(n, 1);
        DoubleMatrix z = DoubleMatrix.randn(m, 1);
        
        tic("Doing DOUBLE Gemv with matrices of size %d * %d for %d times", m, n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.gemv(1.0, x, y, 0.0, z);
        printMflops(2l*m*n*ITERS, toc());

        tic("Doing DOUBLE Gemm with matrices of size %d * %d for %d times", m, n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.gemm(1.0, x, y, 0.0, z);
        printMflops(2l*m*n*ITERS, toc());
    }
    
    public void testBenchmarkGemvFloat() {
        final int m = 1000;
        final int n = 2000;
        final int ITERS = 100;
        FloatMatrix x = FloatMatrix.randn(m, n);
        FloatMatrix y = FloatMatrix.randn(n, 1);
        FloatMatrix z = FloatMatrix.randn(m, 1);
        
        tic("Doing Float Gemv with matrices of size %d * %d for %d times", m, n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.gemv(1.0f, x, y, 0.0f, z);
        printMflops(2l*m*n*ITERS, toc());

        tic("Doing Float Gemm with matrices of size %d * %d for %d times", m, n, ITERS);
        for (int i = 0; i < ITERS; i++)
            SimpleBlas.gemm(1.0f, x, y, 0.0f, z);
        printMflops(2l*m*n*ITERS, toc());
    }
}
