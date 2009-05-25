/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.util;

import java.util.Random;

/**
 *
 */
public class Permutations {
    /**
     * Create a random permutation of the numbers 0, ..., size - 1.
     *
     * see Algorithm P, D.E. Knuth: The Art of Computer Programming, Vol. 2, p. 145
     */
    public static int[] randomPermutation(int size) {
        Random r = new Random();
        int[] result = new int[size];

        for (int j = 0; j < size; j++) {
            result[j] = j;
        }
        
        for (int j = size - 1; j > 0; j--) {
            int k = r.nextInt(j);
            result[j] = result[k];
            result[k] = j;
        }

        return result;
    }

    /**
     * Get a random sample of k out of n elements.
     *
     * See Algorithm S, D. E. Knuth, The Art of Computer Programming, Vol. 2, p.142.
     */
    public static int[] randomSubset(int k, int n) {
        assert(0 < k && k <= n);
        Random r = new Random();
        int t = 0, m = 0;
        int[] result = new int[k];

        while (m < k) {
            double u = r.nextDouble();
            if ( (n - t) * u < k - m ) {
                result[m] = t;
                m++;
            }
            t++;
        }
        return result;
    }
}
