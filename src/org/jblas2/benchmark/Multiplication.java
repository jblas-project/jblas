/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas2.benchmark;

import org.jblas2.Matrix;
import static org.jblas2.Functions.*;

/**
 *
 */
public class Multiplication {
    public static void main(String[] args) {
        CubicBenchmark bench = new Jblas2Multiplication();
        if (args.length > 0 && args[0].equals("jblas"))
            bench = new JblasMultiplication();
        
        int[] sizes = { 10, 100, 1000 };

        for (int size: sizes) {
            long now = System.nanoTime();

            bench.setSize(size);
            bench.run();

            double duration = (System.nanoTime() - now) / 1e9;
            System.out.printf("duration = %.3fms\n", duration * 1e3);
            bench.report(duration);
        }
        
    }

    private static class Jblas2Multiplication extends CubicBenchmark {
        @Override
        public long numOps(int size) {
            return 2*super.numOps(size);
        }
        
        public void run() {
            Matrix a = matrix(size, size);
            Matrix b = matrix(size, size);

            for (int i = 0; i < iters; i++)
                a.mmul(b);
        }
    }

    private static class JblasMultiplication extends CubicBenchmark {
        @Override
        public long numOps(int size) {
            return 2*super.numOps(size);
        }

        public void run() {
            org.jblas.DoubleMatrix a = new org.jblas.DoubleMatrix(size, size);
            org.jblas.DoubleMatrix b = new org.jblas.DoubleMatrix(size, size);

            for (int i = 0; i < iters; i++)
                a.mmul(b);
        }
    }
}
