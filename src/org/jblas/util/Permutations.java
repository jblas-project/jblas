/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.util;

import java.util.Random;
import org.jblas.DoubleMatrix;

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
            int temp = result[j];
            result[j] = result[k];
            result[k] = temp;
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

    /**
     * Create a permutation matrix from a LAPACK-style 'ipiv' vector.
     *
     * @param ipiv row i was interchanged with row ipiv[i]
     */
    public static DoubleMatrix permutationMatrixFromPivotIndices(int size, int[] ipiv) {
        int n = ipiv.length;
        //System.out.printf("size = %d n = %d\n", size, n);
        int indices[] = new int[size];
        for (int i = 0; i < size; i++)
            indices[i] = i;

        //for (int i = 0; i < n; i++)
        //    System.out.printf("ipiv[%d] = %d\n", i, ipiv[i]);

        for (int i = 0; i < n; i++) {
            int j = ipiv[i] - 1;
            int t = indices[i];
            indices[i] = indices[j];
            indices[j] = t;
        }
        DoubleMatrix result = new DoubleMatrix(size, size);
        for (int i = 0; i < size; i++)
            result.put(indices[i], i, 1.0);
        return result;
    }
}
