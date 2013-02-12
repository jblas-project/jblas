// --- BEGIN LICENSE BLOCK ---
/*
 * Copyright (c) 2009, Mikio L. Braun
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *     * Neither the name of the Technische Universität Berlin nor the
 *       names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// --- END LICENSE BLOCK ---
package org.jblas.benchmark;

import java.io.PrintStream;
import org.jblas.util.Logger;

/**
 * A simple command-line style benchmarking program.
 * 
 * <p>Benchmarks matrix-matrix multiplication, and compares to a 
 * pure Java implementation</p>
 *
 * @author Mikio L. Braun
 */
public class Main {

    static Benchmark[] multiplicationBenchmarks = {
        new JavaDoubleMultiplicationBenchmark(),
        new JavaFloatMultiplicationBenchmark(),
        new ATLASDoubleMultiplicationBenchmark(),
        new ATLASFloatMultiplicationBenchmark(),};

    public static void printHelp() {
        System.out.printf("Usage: benchmark [opts]%n"
                + "%n"
                + "with options:%n"
                + "%n"
                + "  --arch-flavor=value     overriding arch flavor (e.g. --arch-flavor=sse2)%n"
                + "  --skip-java             don't run java benchmarks%n"
                + "  --help                  show this help%n"
                + "  --debug                 set config levels to debug%n"
                + "%njblas version " + org.jblas.Info.VERSION + "%n");
    }

    public static void main(String[] args) {
        int[] multiplicationSizes = {10, 100, 1000};
        PrintStream out = System.out;

        boolean skipJava = false;
        boolean unrecognizedOptions = false;

        Logger log = Logger.getLogger();

        log.info("jblas version is " + org.jblas.Info.VERSION);

        for (String arg : args) {
            if (arg.startsWith("--")) {
                int i = arg.indexOf('=');
                String value = null;
                if (i != -1) {
                    value = arg.substring(i + 1);
                    arg = arg.substring(0, i);
                }

                if (arg.equals("--arch-flavor")) {
                    Logger.getLogger().info("Setting arch flavor to " + value);
                    org.jblas.util.ArchFlavor.overrideArchFlavor(value);
                } else if (arg.equals("--skip-java")) {
                    skipJava = true;
                } else if (arg.equals("--help")) {
                    printHelp();
                    return;
                } else if (arg.equals("--debug")) {
                    Logger.getLogger().setLevel(Logger.DEBUG);
                } else {
                    Logger.getLogger().warning("Unrecognized option \"" + arg + "\"");
                    unrecognizedOptions = true;
                }
            }
        }
        if (unrecognizedOptions) {
            return;
        }

        out.println("Simple benchmark for jblas");
        out.println();

        out.println("Running sanity benchmarks.");
        out.println();
        org.jblas.util.SanityChecks.main(args);
        out.println();

        out.println("Each benchmark will take about 5 seconds...");

        for (Benchmark b : multiplicationBenchmarks) {
            if (skipJava) {
                if (b.getName().contains("Java")) {
                    continue;
                }
            }

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
