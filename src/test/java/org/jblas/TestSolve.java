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

import org.jblas.DoubleMatrix;
import org.jblas.FloatMatrix;
import org.jblas.Solve;
import org.jblas.util.Random;
import org.junit.*;
import static org.junit.Assert.*;

/**
 * Tests for methods in Solve
 *
 * Created: 1/18/13, 12:10 PM
 *
 * @author Mikio L. Braun
 */
public class TestSolve {
  @Test
  public void testLeastSquaresDouble() {
    DoubleMatrix A = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
    DoubleMatrix B = new DoubleMatrix(3, 1, 3.0, 2.0, -1.0);

    assertEquals(0.0, new DoubleMatrix(3, 1, -23.0 / 9, -2.0 / 3, 11.0 / 9).sub(Solve.solveLeastSquares(A, B)).normmax(), 1e-10);
  }

  @Test
  public void testLeastSquaresFloat() {
    FloatMatrix A = new FloatMatrix(3, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    FloatMatrix B = new FloatMatrix(3, 1, 3.0f, 2.0f, -1.0f);

    assertEquals(0.0f, new FloatMatrix(3, 1, -23.0f / 9, -2.0f / 3, 11.0f / 9).sub(Solve.solveLeastSquares(A, B)).normmax(), 1e-5f);
  }

  @Test
  public void testLeastSquaresWideMatrixDouble() {
    DoubleMatrix A = new DoubleMatrix(2, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
    DoubleMatrix B = new DoubleMatrix(2, 1, 1.0, -1.0);

    assertEquals(0.0, B.sub(A.mmul(Solve.solveLeastSquares(A, B))).normmax(), 1e-10);
  }

  @Test
  public void testPinvDouble() {
    DoubleMatrix A = new DoubleMatrix(3, 2, 1.0, 3.0, 5.0, 2.0, 4.0, 6.0);

    DoubleMatrix pinvA = Solve.pinv(A);
    assertEquals(0.0, A.mmul(pinvA).mmul(A).sub(A).normmax(), 1e-10);
    assertEquals(0.0, pinvA.mmul(A).mmul(pinvA).sub(pinvA).normmax(), 1e-10);

    DoubleMatrix At = A.transpose();
    DoubleMatrix pinvAt = Solve.pinv(At);

    assertEquals(0.0, At.mmul(pinvAt).mmul(At).sub(At).normmax(), 1e-10);
    assertEquals(0.0, pinvAt.mmul(At).mmul(pinvAt).sub(pinvAt).normmax(), 1e-10);
  }

  @Test
  public void testPinvFloat() {
    FloatMatrix A = new FloatMatrix(3, 2, 1.0f, 3.0f, 5.0f, 2.0f, 4.0f, 6.0f);

    FloatMatrix pinvA = Solve.pinv(A);
    assertEquals(0.0f, A.mmul(pinvA).mmul(A).sub(A).normmax(), 1e-5f);
    assertEquals(0.0f, pinvA.mmul(A).mmul(pinvA).sub(pinvA).normmax(), 1e-5f);
  }

  /*
  @Test
  public void randomSized() {
      for (int i = 0; i < 1000; i++) {
	  Random r = new Random();
	  int m = 362; // r.nextInt(1000);
	  int n = 8; // r.nextInt(1000);
	  DoubleMatrix A = DoubleMatrix.rand(m, n);
	  
	  System.out.printf("Pinv for %d * %d matrix...\n", A.rows, A.columns);
	  double t = System.nanoTime();
	  Solve.pinv(A);
	  System.out.printf("Pinv for %d * %d matrix took %.1fs\n", A.rows, A.columns, (System.nanoTime() - t) / 1e9);
      }
  }
  */
}
