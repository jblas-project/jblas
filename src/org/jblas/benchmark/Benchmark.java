/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas.benchmark;

/**
 *
 * @author mikio
 */
public interface Benchmark {

    public String getName();

    public BenchmarkResult run(int size, double seconds);
}
