/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2.benchmark;

/**
 *
 */
public interface Benchmark {

    public void run();

    public void report(double duration);
}
