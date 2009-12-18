/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.benchmark;

/**
 *
 */
public class BenchmarkResult {
    public long numOps;
    public double duration;
    public int iterations;

    public BenchmarkResult(long numOps, double duration, int iterations) {
        this.numOps = numOps;
        this.duration = duration;
        this.iterations = iterations;
    }

    public void printResult() {
        System.out.printf("%6.1f MFLOPS (%d iterations in %.1f seconds)%n",
                numOps / duration / 1e6,
                iterations,
                duration);
    }
}
