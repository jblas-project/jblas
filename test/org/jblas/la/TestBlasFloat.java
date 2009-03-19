package org.jblas.la;

import org.jblas.la.SimpleBlas;
import org.jblas.la.Geometry;
import junit.framework.TestCase;

import org.jblas.core.BlasUtil;

import org.jblas.la.Blas;
import static java.lang.Math.abs;

import static org.jblas.core.BlasUtil.*;

import static org.jblas.la.MatrixFunctions.*;
import org.jblas.la.FloatMatrix;

public class TestBlasFloat extends TestCase {

	/** test sum of absolute values */
	public void testAsum() {
		float[] a = new float[]{1.0f, 2.0f, 3.0f, 4.0f};
		
		assertEquals(10.0f, Blas.sasum(4, a, 0, 1));
		assertEquals(4.0f, Blas.sasum(2, a, 0, 2));
		assertEquals(5.0f, Blas.sasum(2, a, 1, 1));
	}
	
	/** test scalar product */
	public void testDot() {
		float[] a = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
		float[] b = new float[] { 4.0f, 5.0f, 6.0f, 7.0f };

		assertEquals(32.0f, Blas.sdot(3, a, 0, 1, b, 0, 1));
		assertEquals(22.0f, Blas.sdot(2, a, 0, 2, b, 0, 2));
		assertEquals(5.0f + 12.0f + 21.0f, Blas.sdot(3, a, 0, 1, b, 1, 1));
	}
	
        public void testSwap() {
            float[] a = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
            float[] b = new float[] { 4.0f, 5.0f, 6.0f, 7.0f };
            float[] c = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
            float[] d = new float[] { 4.0f, 5.0f, 6.0f, 7.0f };
            
            System.out.println("dswap");
            Blas.sswap(4, a, 0, 1, b, 0, 1);
            assertTrue(arraysEqual(a, d));
            assertTrue(arraysEqual(b, c));

            System.out.println("dswap same");
            Blas.sswap(2, a, 0, 2, a, 1, 2);
            assertTrue(arraysEqual(a, 5.0f, 4.0f, 7.0f, 6.0f));
        }
        
	/* test vector addition */
	public void testAxpy() {
		float[] x = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
		float[] y = new float[] { 0.0f, 0.0f, 0.0f, 0.0f };
		
		Blas.saxpy(4, 2.0f, x, 0, 1, y, 0, 1);
		
		for(int i = 0; i < 4; i++)
			assertEquals(2*x[i], y[i]);
	}
	
	/* test matric-vector multiplication */
	public void testGemv() {
		float[] A = new float[] { 1.0f, 2.0f, 3.0f,
										  4.0f, 5.0f, 6.0f,
										  7.0f, 8.0f, 9.0f };
		
		float[] x = new float[] {1.0f, 3.0f, 7.0f };
		float[] y = new float[] { 0.0f, 0.0f, 0.0f };
		
		Blas.sgemv('N', 3, 3, 1.0f, A, 0, 3, x, 0, 1, 0.0f, y, 0, 1);
		
		//printMatrix(3, 3, A);
		//printMatrix(3, 1, x);
		//printMatrix(3, 1, y);
		
		assertTrue(arraysEqual(y, 62.0f, 73.0f, 84.0f));
		
		Blas.sgemv('T', 3, 3, 1.0f, A, 0, 3, x, 0, 1, 0.5f, y, 0, 1);

		//printMatrix(3, 1, y);
		assertTrue(arraysEqual(y, 59.0f, 97.5f, 136.0f));
	}
		
	/** Compare float buffer against an array of floats */
	private boolean arraysEqual(float[] a, float... b) {
		if (a.length != b.length)
			return false;
		else { 
			float diff = 0.0f;
			for (int i = 0; i < b.length; i++)
				diff += abs(a[i] - b[i]);
			return diff < 1e-6;
		}
	}
	
	public static void main(String[] args) {
		TestBlasFloat t = new TestBlasFloat();
		
		t.testAsum();
	}
	
	public static void testSolve() {
		FloatMatrix A = new FloatMatrix(3, 3, 3.0f, 5.0f, 6.0f, 1.0f, 0.0f, 0.0f, 2.0f, 4.0f, 0.0f);
		FloatMatrix X = new FloatMatrix(3, 1, 1.0f, 2.0f, 3.0f);
		int[] p = new int[3];
		SimpleBlas.gesv(A, p, X);
		A.print();
		X.print();
		// De-shuffle X
		for (int i = 2; i >= 0; i--) {
			int perm = p[i] - 1;
			float t = X.get(i); X.put(i, X.get(perm)); X.put(perm, t);
		}
		System.out.println();
		X.print();
	}
	
	public static void testSymmetricSolve() {
		System.out.println("--- Symmetric solve");
		FloatMatrix A = new FloatMatrix(3, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		FloatMatrix x = new FloatMatrix(3, 1, 1.0f, 2.0f, 3.0f);
		int[] p = new int[3];
		SimpleBlas.sysv('U', A, p, x);
		A.print();
		x.print();
	}
	
	public static void testSYEV() {
		System.out.println("--- Symmetric eigenvalues");
		int n = 10;
		FloatMatrix x = FloatMatrix.randn(n).sort();
		
		//FloatMatrix A = new FloatMatrix(new float[][] {{1.0f, 0.5f, 0.1f}, {0.5f, 1.0f, 0.5f}, {0.1f, 0.5f, 1.0f}});
		FloatMatrix A = expi(Geometry.pairwiseSquaredDistances(x, x).muli(-2.0f));
		FloatMatrix w = new FloatMatrix(n);
		
		FloatMatrix B = A.dup();
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
