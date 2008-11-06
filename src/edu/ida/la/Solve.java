package edu.ida.la;

import static edu.ida.core.BlasUtil.createIntBuffer;

import java.nio.IntBuffer;

/**
 * Solving linear equations.
 */
public class Solve {
	/* Solves the linear equation A*X = B. */
	public static DoubleMatrix solve(DoubleMatrix A, DoubleMatrix B) {
		A.assertSquare();
		DoubleMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.gesv(A.dup(), ipiv, X);
		return X;
	}

	/* Solves the linear equation A*X = B for symmetric A. */
	public static DoubleMatrix solveSymmetric(DoubleMatrix A, DoubleMatrix B) {
		A.assertSquare();
		DoubleMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.sysv('U', A.dup(), ipiv, X);
		return X;
	}

	
	/* Solves the linear equation A*X = B for symmetric and positive definite A. */
	public static DoubleMatrix solvePositive(DoubleMatrix A, DoubleMatrix B) {
		A.assertSquare();
		DoubleMatrix X = B.dup();
		SimpleBlas.posv('U', A.dup(), X);
		return X;
	}

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
	/* Solves the linear equation A*X = B. */
	public static FloatMatrix solve(FloatMatrix A, FloatMatrix B) {
		A.assertSquare();
		FloatMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.gesv(A.dup(), ipiv, X);
		return X;
	}

	/* Solves the linear equation A*X = B for symmetric A. */
	public static FloatMatrix solveSymmetric(FloatMatrix A, FloatMatrix B) {
		A.assertSquare();
		FloatMatrix X = B.dup();
		int[] ipiv = new int[B.rows];
		SimpleBlas.sysv('U', A.dup(), ipiv, X);
		return X;
	}

	
	/* Solves the linear equation A*X = B for symmetric and positive definite A. */
	public static FloatMatrix solvePositive(FloatMatrix A, FloatMatrix B) {
		A.assertSquare();
		FloatMatrix X = B.dup();
		SimpleBlas.posv('U', A.dup(), X);
		return X;
	}

//END
}
