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

import org.jblas.util.Functions;

/**
 * Solving linear equations.
 */
public class Solve {
	/** Solves the linear equation A*X = B. */
	public static DoubleMatrix solve(DoubleMatrix A, DoubleMatrix B) {
		A.assertSquare();
		DoubleMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.gesv(A.dup(), ipiv, X);
		return X;
	}

	/** Solves the linear equation A*X = B for symmetric A. */
	public static DoubleMatrix solveSymmetric(DoubleMatrix A, DoubleMatrix B) {
		A.assertSquare();
		DoubleMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.sysv('U', A.dup(), ipiv, X);
		return X;
	}

	
	/** Solves the linear equation A*X = B for symmetric and positive definite A. */
	public static DoubleMatrix solvePositive(DoubleMatrix A, DoubleMatrix B) {
		A.assertSquare();
		DoubleMatrix X = B.dup();
		SimpleBlas.posv('U', A.dup(), X);
		return X;
	}

  /** Computes the Least Squares solution for over or underdetermined
   * linear equations A*X = B
   *
   * In the overdetermined case, when m > n, that is, there are more equations than
   * variables, it computes the least squares solution of X -> ||A*X - B ||_2.
   *
   * In the underdetermined case, when m < n (less equations than variables), there are infinitely
   * many solutions and it computes the minimum norm solution.
   *
   * @param A an (m,n) matrix
   * @param B a (m,k) matrix
   * @return either the minimum norm or least squares solution.
   */
  public static DoubleMatrix solveLeastSquares(DoubleMatrix A, DoubleMatrix B) {
    if (B.rows < A.columns) {
      DoubleMatrix X = DoubleMatrix.concatVertically(B, new DoubleMatrix(A.columns - B.rows, B.columns));
      SimpleBlas.gelsd(A.dup(), X);
      return X;
    } else {
      DoubleMatrix X = B.dup();
      SimpleBlas.gelsd(A.dup(), X);
      return X.getRange(0, A.columns, 0, B.columns);
    }
  }

  /**
   * Computes the pseudo-inverse.
   *
   * Note, this function uses the solveLeastSquares and might produce different numerical
   * solutions for the underdetermined case than matlab.
   *
   * @param A rectangular matrix
   * @return matrix P such that A*P*A = A and P*A*P = P.
   */
  public static DoubleMatrix pinv(DoubleMatrix A) {
    return solveLeastSquares(A, DoubleMatrix.eye(A.rows));
  }

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
	/** Solves the linear equation A*X = B. */
	public static FloatMatrix solve(FloatMatrix A, FloatMatrix B) {
		A.assertSquare();
		FloatMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.gesv(A.dup(), ipiv, X);
		return X;
	}

	/** Solves the linear equation A*X = B for symmetric A. */
	public static FloatMatrix solveSymmetric(FloatMatrix A, FloatMatrix B) {
		A.assertSquare();
		FloatMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.sysv('U', A.dup(), ipiv, X);
		return X;
	}

	
	/** Solves the linear equation A*X = B for symmetric and positive definite A. */
	public static FloatMatrix solvePositive(FloatMatrix A, FloatMatrix B) {
		A.assertSquare();
		FloatMatrix X = B.dup();
		SimpleBlas.posv('U', A.dup(), X);
		return X;
	}

  /** Computes the Least Squares solution for over or underdetermined
   * linear equations A*X = B
   *
   * In the overdetermined case, when m > n, that is, there are more equations than
   * variables, it computes the least squares solution of X -> ||A*X - B ||_2.
   *
   * In the underdetermined case, when m < n (less equations than variables), there are infinitely
   * many solutions and it computes the minimum norm solution.
   *
   * @param A an (m,n) matrix
   * @param B a (m,k) matrix
   * @return either the minimum norm or least squares solution.
   */
  public static FloatMatrix solveLeastSquares(FloatMatrix A, FloatMatrix B) {
    if (B.rows < A.columns) {
      FloatMatrix X = FloatMatrix.concatVertically(B, new FloatMatrix(A.columns - B.rows, B.columns));
      SimpleBlas.gelsd(A.dup(), X);
      return X;
    } else {
      FloatMatrix X = B.dup();
      SimpleBlas.gelsd(A.dup(), X);
      return X.getRange(0, A.columns, 0, B.columns);
    }
  }

  /**
   * Computes the pseudo-inverse.
   *
   * Note, this function uses the solveLeastSquares and might produce different numerical
   * solutions for the underdetermined case than matlab.
   *
   * @param A rectangular matrix
   * @return matrix P such that A*P*A = A and P*A*P = P.
   */
  public static FloatMatrix pinv(FloatMatrix A) {
    return solveLeastSquares(A, FloatMatrix.eye(A.rows));
  }

//END
}
