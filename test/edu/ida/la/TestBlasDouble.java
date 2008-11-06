package edu.ida.la;

import junit.framework.TestCase;

import edu.ida.core.BlasUtil;
import edu.ida.la.Blas;

import java.nio.DoubleBuffer;
import java.nio.IntBuffer;
import static java.lang.Math.abs;

import static edu.ida.core.BlasUtil.*;
import static edu.ida.la.MatrixFunctions.*;

import edu.ida.la.DoubleMatrix;

public class TestBlasDouble extends TestCase {

	/** test sum of absolute values */
	public void testAsum() {
		DoubleBuffer a = createDoubleBufferFrom(1.0, 2.0, 3.0, 4.0);
		
		assertEquals(10.0, Blas.dasum(4, a, 0, 1));
		assertEquals(4.0, Blas.dasum(2, a, 0, 2));
		assertEquals(5.0, Blas.dasum(2, a, 1, 1));
	}
	
	/** test scalar product */
	public void testDot() {
		DoubleBuffer a = BlasUtil.createDoubleBufferFrom(1.0, 2.0, 3.0, 4.0);
		DoubleBuffer b = BlasUtil.createDoubleBufferFrom(4.0, 5.0, 6.0, 7.0);

		assertEquals(32.0, Blas.ddot(3, a, 0, 1, b, 0, 1));
		assertEquals(22.0, Blas.ddot(2, a, 0, 2, b, 0, 2));
		assertEquals(5.0 + 12.0 + 21.0, Blas.ddot(3, a, 0, 1, b, 1, 1));
	}
	
	/* test vector addition */
	public void testAxpy() {
		DoubleBuffer x = createDoubleBufferFrom(1.0, 2.0, 3.0, 4.0);
		DoubleBuffer y = createDoubleBufferFrom(0.0, 0.0, 0.0, 0.0);
		
		Blas.daxpy(4, 2.0, x, 0, 1, y, 0, 1);
		
		for(int i = 0; i < 4; i++)
			assertEquals(2*x.get(i), y.get(i));
	}
	
	/* test matric-vector multiplication */
	public void testGemv() {
		DoubleBuffer A = createDoubleBufferFrom(1.0, 2.0, 3.0,
										  4.0, 5.0, 6.0,
										  7.0, 8.0, 9.0);
		
		DoubleBuffer x = createDoubleBufferFrom(1.0, 3.0, 7.0);
		DoubleBuffer y = createDoubleBufferFrom(0.0, 0.0, 0.0);
		
		Blas.dgemv('N', 3, 3, 1.0, A, 0, 3, x, 0, 1, 0.0, y, 0, 1);
		
		//printMatrix(3, 3, A);
		//printMatrix(3, 1, x);
		//printMatrix(3, 1, y);
		
		assertTrue(arraysEqual(y, 62.0, 73.0, 84.0));
		
		Blas.dgemv('T', 3, 3, 1.0, A, 0, 3, x, 0, 1, 0.5, y, 0, 1);

		//printMatrix(3, 1, y);
		assertTrue(arraysEqual(y, 59.0, 97.5, 136.0));
	}
		
	/** Compare double buffer against an array of doubles */
	private boolean arraysEqual(DoubleBuffer a, double... b) {
		if (a.capacity() != b.length)
			return false;
		else { 
			double diff = 0.0;
			for (int i = 0; i < b.length; i++)
				diff += abs(a.get(i) - b[i]);
			return diff < 1e-6;
		}
	}
	
	public static void main(String[] args) {
		TestBlasDouble t = new TestBlasDouble();
		
		t.testAsum();
	}
	
	public static void testSolve() {
		DoubleMatrix A = new DoubleMatrix(3, 3, 3.0, 5.0, 6.0, 1.0, 0.0, 0.0, 2.0, 4.0, 0.0);
		DoubleMatrix X = new DoubleMatrix(3, 1, 1.0, 2.0, 3.0);
		IntBuffer p = createIntBuffer(3);
		SimpleBlas.gesv(A, p, X);
		A.print();
		X.print();
		// De-shuffle X
		for (int i = 2; i >= 0; i--) {
			int perm = p.get(i) - 1;
			double t = X.get(i); X.put(i, X.get(perm)); X.put(perm, t);
		}
		System.out.println();
		X.print();
	}
	
	public static void testSymmetricSolve() {
		System.out.println("--- Symmetric solve");
		DoubleMatrix A = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
		DoubleMatrix x = new DoubleMatrix(3, 1, 1.0, 2.0, 3.0);
		IntBuffer p = createIntBuffer(3);
		SimpleBlas.sysv('U', A, p, x);
		A.print();
		x.print();
	}
	
	public static void testSYEV() {
		System.out.println("--- Symmetric eigenvalues");
		int n = 10;
		DoubleMatrix x = DoubleMatrix.randn(n).sort();
		
		//DoubleMatrix A = new DoubleMatrix(new double[][] {{1.0, 0.5, 0.1}, {0.5, 1.0, 0.5}, {0.1, 0.5, 1.0}});
		DoubleMatrix A = expi(Geometry.pairwiseSquaredDistances(x, x).muli(-2.0));
		DoubleMatrix w = new DoubleMatrix(n);
		
		DoubleMatrix B = A.dup();
		System.out.println("Computing eigenvalues with SYEV");
		SimpleBlas.syev('V', 'U', B, w);
		System.out.println("Eigenvalues: ");
		w.print();
		System.out.println("Eigenvectors: ");
		B.print();

		B = A.dup();
		System.out.println("Computing eigenvalues with SYEVD");
		SimpleBlas.syevd('V', 'U', B, w);
		System.out.println("Eigenvalues: ");
		w.print();
		System.out.println("Eigenvectors: ");
		B.print();
	}
}
