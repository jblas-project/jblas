/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas2.benchmark;

import org.jblas2.Add;
import org.jblas2.Binop;
import org.jblas2.DoubleMatrix;
import org.jblas2.JblasMatrix;
import org.jblas2.Matrix;
import org.jblas2.Type;
import static org.jblas2.Functions.*;

/**
 *
 */
public class Addition {

    public static void main(String[] args) {
        LinearBenchmark bench = null;
        if (args.length > 0 && args[0].equals("jblas")) {
            bench = new JblasAdditionBenchmark();
        } else {
            bench = new Jblas2AdditionBenchmark();
            ((Jblas2AdditionBenchmark)bench).setBinop(new Add());
        }

        int[] sizes = {10, 1000, 100000, 10000000};

        System.out.println("Burning in...");
        bench.setSize(sizes[0]);
        bench.run();
        bench.run();
        bench.run();
        bench.run();
        bench.run();

        for (int size : sizes) {

            long now = System.nanoTime();

            bench.setSize(size);
            ((Jblas2AdditionBenchmark)bench).setBinop(new Add());
            bench.run();

            double duration = (System.nanoTime() - now) / 1e9;
            System.out.printf("duration = %.3fms\n", duration * 1000);
            bench.report(duration);
        }
    }

    private static class Jblas2AdditionBenchmark extends LinearBenchmark {

        Binop binop;

        public void setBinop(Binop op) {
            binop = op;
        }

        public void run() {
            Matrix a = matrix(size);
            Matrix b = matrix(size);
            Matrix c = matrix(size);

            //Binop add = new Add();

            for (int i = 0; i < iters; i++) {
                a.addi(b, c);
                /*Type commonType = a.type().commonType(b.type());
                Matrix aa = a.convertTo(commonType);
                Matrix bb = b.convertTo(commonType);
                Matrix cc = c.convertTo(commonType);
                aa.computeBinop(binop, bb, cc); /**/
                //JblasMatrix.binop(a, b, c, binop);
            }
        }
    }

    private static class JblasAdditionBenchmark extends LinearBenchmark {

        public void run() {
            org.jblas.DoubleMatrix a = new org.jblas.DoubleMatrix(size);
            org.jblas.DoubleMatrix b = new org.jblas.DoubleMatrix(size);
            org.jblas.DoubleMatrix c = new org.jblas.DoubleMatrix(size);

            for (int i = 0; i < iters; i++) {
                a.addi(b, c);
            }
        }
    }
}
