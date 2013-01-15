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

package org.jblas.util;

import java.util.Random;
import org.jblas.DoubleMatrix;
import org.jblas.FloatMatrix;

/**
 * Functions which generate random permutations.
 *
 * @author Mikio L. Braun
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
    public static DoubleMatrix permutationDoubleMatrixFromPivotIndices(int size, int[] ipiv) {
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

  /**
   * Create a permutation matrix from a LAPACK-style 'ipiv' vector.
   *
   * @param ipiv row i was interchanged with row ipiv[i]
   */
  public static FloatMatrix permutationFloatMatrixFromPivotIndices(int size, int[] ipiv) {
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
      FloatMatrix result = new FloatMatrix(size, size);
      for (int i = 0; i < size; i++)
          result.put(indices[i], i, 1.0f);
      return result;
  }
}
