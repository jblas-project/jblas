/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2.benchmark;

/**
 *
 */
public abstract class CubicBenchmark implements Benchmark {

    protected int size;
    protected int iters;

    public void setSize(int size) {
        this.size = size;
        iters = (int)(10000000000L / numOps(size));

        if (iters == 0) iters = 1;
    }

    public long numOps(int s) {
        return (long) s * s * s;
    }

    public void report(double duration) {
        System.out.printf("  size = %d, MFLOPS = %.3f\n", size, numOps(size) * iters / 1e6 / duration);
    }
}
