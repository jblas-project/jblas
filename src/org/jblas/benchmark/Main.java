/*  
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas.benchmark;

import java.io.PrintStream;

/**
 *
 * @author mikio
 */
public class Main {

    public static Benchmark[] multiplicationBenchmarks = {
        new JavaDoubleMultiplicationBenchmark(),
        new JavaFloatMultiplicationBenchmark(),
        new ATLASDoubleMultiplicationBenchmark(),
        new ATLASFloatMultiplicationBenchmark(),
    };

    public static void main(String[] args) {
        int[] multiplicationSizes = {10, 100, 1000};
        PrintStream out = System.out;

        out.println("Simple benchmark for jblas");
        out.println();
        out.println("Each benchmark will take about 5 seconds...");

        for (Benchmark b : multiplicationBenchmarks) {
            out.println();
            out.println("Running benchmark \"" + b.getName() + "\".");
            for (int n : multiplicationSizes) {
                out.printf("n = %-5d: ", n);
                out.flush();

                BenchmarkResult result = b.run(n, 5.0);

                result.printResult();
            }
        }
    }
}
