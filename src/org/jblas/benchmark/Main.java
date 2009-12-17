/*  
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas.benchmark;

import org.jblas.FloatMatrix;
import static org.jblas.FloatMatrix.*;

/**
 *
 * @author mikio
 */
public class Main {

    public static void main(String[] args) {
        int[] ns = {10, 100, 1000};
        int counter = 0;
        long ops = 0;
        
        int n = 1000;
        FloatMatrix A = randn(n, n);
        FloatMatrix B = randn(n, n);
        FloatMatrix C = randn(n, n);

        Timer t = new Timer();
        t.start();
        while (!t.ranFor(10.0)) {
            A.mmuli(B, C);
            counter++;
            ops += 2L * n * n * n;
        }
        t.stop();
        System.out.printf("counter = %d, mflops = %.3f, seconds = %.3f\n",
                counter,
                ops / t.elapsedSeconds() / 1e6,
                t.elapsedSeconds());
    }
}
