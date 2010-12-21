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

//END
}
