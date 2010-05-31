/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2.benchmark;

/**
 * Benchmark for operations which are linear in the size
 */
public abstract class LinearBenchmark implements Benchmark {

    protected int size;
    protected int iters;

    public void setSize(int size) {
        this.size = size;
        iters = (int)(100000000L / size);
    }

    public void report(double duration) {
        System.out.printf("  size = %d, MFLOPS = %.3f\n", size, (long) size * iters / 1e6 / duration);
    }
}
