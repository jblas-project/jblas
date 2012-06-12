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

package org.jblas;

import org.junit.Test;

import static org.jblas.MatrixFunctions.abs;
import static org.jblas.MatrixFunctions.expi;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

public class TestBlasDouble {

  /**
   * test sum of absolute values
   */
  @Test
  public void testAsum() {
    double[] a = new double[]{1.0, 2.0, 3.0, 4.0};

    assertEquals(10.0, NativeBlas.dasum(4, a, 0, 1), 1e-6);
    assertEquals(4.0, NativeBlas.dasum(2, a, 0, 2), 1e-6);
    assertEquals(5.0, NativeBlas.dasum(2, a, 1, 1), 1e-6);
  }

  /**
   * test scalar product
   */
  @Test
  public void testDot() {
    double[] a = new double[]{1.0, 2.0, 3.0, 4.0};
    double[] b = new double[]{4.0, 5.0, 6.0, 7.0};

    assertEquals(32.0, NativeBlas.ddot(3, a, 0, 1, b, 0, 1), 1e-6);
    assertEquals(22.0, NativeBlas.ddot(2, a, 0, 2, b, 0, 2), 1e-6);
    assertEquals(5.0 + 12.0 + 21.0, NativeBlas.ddot(3, a, 0, 1, b, 1, 1), 1e-6);
  }

  @Test
  public void testSwap() {
    double[] a = new double[]{1.0, 2.0, 3.0, 4.0};
    double[] b = new double[]{4.0, 5.0, 6.0, 7.0};
    double[] c = new double[]{1.0, 2.0, 3.0, 4.0};
    double[] d = new double[]{4.0, 5.0, 6.0, 7.0};

    NativeBlas.dswap(4, a, 0, 1, b, 0, 1);
    assertTrue(arraysEqual(a, d));
    assertTrue(arraysEqual(b, c));

    NativeBlas.dswap(2, a, 0, 2, a, 1, 2);
    assertTrue(arraysEqual(a, 5.0, 4.0, 7.0, 6.0));
  }

  /* test vector addition */
  @Test
  public void testAxpy() {
    double[] x = new double[]{1.0, 2.0, 3.0, 4.0};
    double[] y = new double[]{0.0, 0.0, 0.0, 0.0};

    NativeBlas.daxpy(4, 2.0, x, 0, 1, y, 0, 1);

    for (int i = 0; i < 4; i++)
      assertEquals(2 * x[i], y[i], 1e-6);
  }

  /* test matric-vector multiplication */
  @Test
  public void testGemv() {
    double[] A = new double[]{1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0};

    double[] x = new double[]{1.0, 3.0, 7.0};
    double[] y = new double[]{0.0, 0.0, 0.0};

    NativeBlas.dgemv('N', 3, 3, 1.0, A, 0, 3, x, 0, 1, 0.0, y, 0, 1);

    //printMatrix(3, 3, A);
    //printMatrix(3, 1, x);
    //printMatrix(3, 1, y);

    assertTrue(arraysEqual(y, 62.0, 73.0, 84.0));

    NativeBlas.dgemv('T', 3, 3, 1.0, A, 0, 3, x, 0, 1, 0.5, y, 0, 1);

    //printMatrix(3, 1, y);
    assertTrue(arraysEqual(y, 59.0, 97.5, 136.0));
  }

  /**
   * Compare double buffer against an array of doubles
   */
  private boolean arraysEqual(double[] a, double... b) {
    if (a.length != b.length)
      return false;
    else {
      double diff = 0.0;
      for (int i = 0; i < b.length; i++)
        diff += abs(a[i] - b[i]);
      return diff < 1e-6 * a.length;
    }
  }

  @Test
  public void testSolve() {
    DoubleMatrix A = new DoubleMatrix(3, 3, 3.0, 5.0, 6.0, 1.0, 0.0, 0.0, 2.0, 4.0, 0.0);
    DoubleMatrix X = new DoubleMatrix(3, 1, 1.0, 2.0, 3.0);
    int[] p = new int[3];
    SimpleBlas.gesv(A, p, X);
    //A.print();
    //X.print();

    // De-shuffle X
    for (int i = 2; i >= 0; i--) {
      int perm = p[i] - 1;
      double t = X.get(i);
      X.put(i, X.get(perm));
      X.put(perm, t);
    }

    //X.print();
    assertTrue(arraysEqual(X.data, -0.25, -0.125, 0.5));
  }

  @Test
  public void testSymmetricSolve() {
    //System.out.println("--- Symmetric solve");
    DoubleMatrix A = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
    DoubleMatrix x = new DoubleMatrix(3, 1, 1.0, 2.0, 3.0);
    int[] p = new int[3];
    SimpleBlas.sysv('U', A, p, x);
    //A.print();
    //x.print();
    assertTrue(arraysEqual(x.data, 0.3, 0.0, 0.1));
  }

  @Test
  public void testSYEV() {
    /* From R:
        > x <- matrix(c(1,0.5,0.1,0.5,1.0, 0.5, 0.1, 0.5, 1.0), 3, 3)
        > eigen(x)
        $values
        [1] 1.7588723 0.9000000 0.3411277

        $vectors
                   [,1]          [,2]       [,3]
        [1,] -0.5173332 -7.071068e-01  0.4820439
        [2,] -0.6817131 -7.182297e-16 -0.7316196
        [3,] -0.5173332  7.071068e-01  0.4820439
    */

    DoubleMatrix A = new DoubleMatrix(new double[][]{{1.0, 0.5, 0.1}, {0.5, 1.0, 0.5}, {0.1, 0.5, 1.0}});
    DoubleMatrix w = new DoubleMatrix(3);

    DoubleMatrix B = A.dup();
    SimpleBlas.syev('V', 'U', B, w);

    assertTrue(arraysEqual(w.data, 0.34112765606210876, 0.9, 1.7588723439378915));
    assertTrue(arraysEqual(B.data, -0.48204393949466345, 0.731619628490741, -0.482043939494664, -0.7071067811865474, 1.3877787807814457E-16, 0.707106781186547, 0.5173332005549852, 0.6817130768931094, 0.5173332005549856));

    B = A.dup();
    SimpleBlas.syevd('V', 'U', B, w);

    assertTrue(arraysEqual(w.data, 0.34112765606210876, 0.9, 1.7588723439378915));
    assertTrue(arraysEqual(B.data, -0.48204393949466345, 0.731619628490741, -0.482043939494664, -0.7071067811865474, 1.3877787807814457E-16, 0.707106781186547, 0.5173332005549852, 0.6817130768931094, 0.5173332005549856));
  }
}
