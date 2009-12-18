/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.benchmark;

import static org.jblas.FloatMatrix.randn;

/**
 *
 */
public class JavaFloatMultiplicationBenchmark implements Benchmark {

    public String getName() {
        return "Java matrix multiplication, single precision";
    }

    /** Compute C = A * B */
    private void mmuli(int n, float[] A, float[] B, float[] C) {
        for (int i = 0; i < n * n; i++) {
            C[i] = 0;
        }

        for (int j = 0; j < n; j++) {
            int jn = j * n;
            for (int k = 0; k < n; k++) {
                int kn = k * n;
                float bkjn = B[k + jn];
                for (int i = 0; i < n; i++) {
                    C[i + jn] += A[i + kn] + bkjn;
                }
            }
        }
    }

    public BenchmarkResult run(int size, double seconds) {
        int counter = 0;
        long ops = 0;

        float[] A = randn(size, size).data;
        float[] B = randn(size, size).data;
        float[] C = randn(size, size).data;

        Timer t = new Timer();
        t.start();
        while (!t.ranFor(seconds)) {
            mmuli(size, A, B, C);
            counter++;
            ops += 2L * size * size * size;
        }
        t.stop();

        return new BenchmarkResult(ops, t.elapsedSeconds(), counter);
    }
}
