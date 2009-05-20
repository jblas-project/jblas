/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.util;

import java.util.Arrays;
import java.util.Comparator;
import java.util.Random;

/**
 *
 */
public class Permutations {
    public static Integer[] randomPermutation(int size) {
        Integer[] result = new Integer[size];
        final int[] x = new int[size];
        Random r = new Random();

        for (int i = 0; i < size; i++) {
            result[i] = new Integer(i);
            x[i] = r.nextInt();
        }

        Arrays.sort(result, new Comparator<Integer>() {

            public int compare(Integer o1, Integer o2) {
                int i1 = o1.intValue();
                int i2 = o2.intValue();
                if (x[i1] < x[i2])
                    return -1;
                else if (x[i1] > x[i2])
                    return 1;
                else
                    return 0;
            }

        });

        return result;
    }
}
