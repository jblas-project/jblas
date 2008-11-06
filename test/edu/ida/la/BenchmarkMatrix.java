/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

import junit.framework.TestCase;

import static edu.ida.la.TicToc.*;

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
