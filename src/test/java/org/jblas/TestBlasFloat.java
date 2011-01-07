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

import junit.framework.TestCase;

import static org.jblas.MatrixFunctions.*;

public class TestBlasFloat extends TestCase {

	/** test sum of absolute values */
	public void testAsum() {
		float[] a = new float[]{1.0f, 2.0f, 3.0f, 4.0f};
		
		assertEquals(10.0f, NativeBlas.sasum(4, a, 0, 1));
		assertEquals(4.0f, NativeBlas.sasum(2, a, 0, 2));
		assertEquals(5.0f, NativeBlas.sasum(2, a, 1, 1));
	}
	
	/** test scalar product */
	public void testDot() {
		float[] a = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
		float[] b = new float[] { 4.0f, 5.0f, 6.0f, 7.0f };

		assertEquals(32.0f, NativeBlas.sdot(3, a, 0, 1, b, 0, 1));
		assertEquals(22.0f, NativeBlas.sdot(2, a, 0, 2, b, 0, 2));
		assertEquals(5.0f + 12.0f + 21.0f, NativeBlas.sdot(3, a, 0, 1, b, 1, 1));
	}
	
        public void testSwap() {
            float[] a = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
            float[] b = new float[] { 4.0f, 5.0f, 6.0f, 7.0f };
            float[] c = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
            float[] d = new float[] { 4.0f, 5.0f, 6.0f, 7.0f };
            
            System.out.println("dswap");
            NativeBlas.sswap(4, a, 0, 1, b, 0, 1);
            assertTrue(arraysEqual(a, d));
            assertTrue(arraysEqual(b, c));

            System.out.println("dswap same");
            NativeBlas.sswap(2, a, 0, 2, a, 1, 2);
            assertTrue(arraysEqual(a, 5.0f, 4.0f, 7.0f, 6.0f));
        }
        
	/* test vector addition */
	public void testAxpy() {
		float[] x = new float[] { 1.0f, 2.0f, 3.0f, 4.0f };
		float[] y = new float[] { 0.0f, 0.0f, 0.0f, 0.0f };
		
		NativeBlas.saxpy(4, 2.0f, x, 0, 1, y, 0, 1);
		
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
		
		NativeBlas.sgemv('N', 3, 3, 1.0f, A, 0, 3, x, 0, 1, 0.0f, y, 0, 1);
		
		//printMatrix(3, 3, A);
		//printMatrix(3, 1, x);
		//printMatrix(3, 1, y);
		
		assertTrue(arraysEqual(y, 62.0f, 73.0f, 84.0f));
		
		NativeBlas.sgemv('T', 3, 3, 1.0f, A, 0, 3, x, 0, 1, 0.5f, y, 0, 1);

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
