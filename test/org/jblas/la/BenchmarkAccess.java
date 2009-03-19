package org.jblas.la;

import org.jblas.la.DoubleMatrix;
import org.jblas.la.Blas;
import junit.framework.TestCase;

import static org.jblas.la.TicToc.*;

public class BenchmarkAccess extends TestCase {
	public void testArrayVsDirectBuffer() {
		System.out.println("Testing array access versus direct buffer access");
		
		int SIZE = 100;
		int ITERS = 10000000;
		DoubleMatrix ma = new DoubleMatrix(1, SIZE);
		double[] aa = new double[SIZE];
		DoubleMatrix mb = new DoubleMatrix(1, SIZE);
		double[] ab = new double[SIZE];
		
		tic("double[]:");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				aa[i] = ab[i];
			}
		toc();
		
		/*tic("DoubleBuffer.put()");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				mb.data.put(i, ma.data.get(i));
			}
		toc();*/

		tic("DoubleMatrix.put()");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				mb.put(i, ma.get(i));
			}
		toc();
		
		tic("DoubleMatrix.put() (two-dim)");
		for (int j = 0; j < ITERS; j++)
			for (int i = 0; i < SIZE; i++) {
				mb.put(0, i, ma.get(0, i));
			}
		toc();
		
		tic("Blas.dcopy");
		for (int j = 0; j < ITERS; j++)
			Blas.dcopy(SIZE, mb.data, 0, 1, ma.data, 0, 1);
		toc();
		
		System.out.println("Done");
	}
}
