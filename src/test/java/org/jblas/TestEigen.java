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

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas;

import junit.framework.TestCase;
import org.jblas.util.Logger;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 * Test Class for org.jblas.Eigen
 *
 * @author mikio
 */

public class TestEigen {

  private final double eps = 1e-10;

  @Test
  public void testEigenvalues() {
    DoubleMatrix A = new DoubleMatrix(2, 2, 3.0, -3.0, 1.0, 1.0);

    ComplexDoubleMatrix E = Eigen.eigenvalues(A);

    ComplexDoubleMatrix[] EV = Eigen.eigenvectors(A);

    ComplexDoubleMatrix X = EV[0];
    ComplexDoubleMatrix L = EV[1];

    assertEquals(0.0, A.toComplex().mmul(X).sub(X.mmul(L)).norm2(), eps);
  }

  @Test
  public void testSymmetricEigenvalues() {
    DoubleMatrix A = new DoubleMatrix(new double[][]{
        {3.0, 1.0, 0.5},
        {1.0, 3.0, 1.0},
        {0.5, 1.0, 3.0}
    });

    DoubleMatrix B = new DoubleMatrix(new double[][]{
        {2.0, 0.1, 0.0},
        {0.1, 2.0, 0.1},
        {0.0, 0.1, 2.0}
    });

    DoubleMatrix[] results = Eigen.symmetricGeneralizedEigenvectors(A, B);

    DoubleMatrix V = results[0];
    DoubleMatrix L = results[1];

    DoubleMatrix LHS = A.mmul(V);
    DoubleMatrix RHS = B.mmul(V).mmul(DoubleMatrix.diag(L));

    assertEquals(0.0, LHS.sub(RHS).normmax(), eps);

    DoubleMatrix eigenvalues = Eigen.symmetricGeneralizedEigenvalues(A, B);

    assertEquals(0.0, eigenvalues.sub(L).normmax(), eps);
  }
}
