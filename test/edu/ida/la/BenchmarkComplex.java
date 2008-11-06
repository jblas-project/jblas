package edu.ida.la;

import junit.framework.TestCase;
import static edu.ida.la.TicToc.*;

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
