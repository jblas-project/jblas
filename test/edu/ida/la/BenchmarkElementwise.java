package edu.ida.la;

import junit.framework.TestCase;
import static edu.ida.la.TicToc.*;
import static edu.ida.la.DoubleMatrix.*;

public class BenchmarkElementwise extends TestCase {
	public void testMuli() {
		int SIZE = 1000;
		int ITERS = 1000000;
		DoubleMatrix x = rand(SIZE);
		DoubleMatrix y = rand(SIZE);
		DoubleMatrix z = zeros(SIZE);
		
		tic("muli():");
		for (int i = 0; i < ITERS; i++)
			x.muli(y, z);
		toc();

		double xa[]= new double[SIZE];
		double ya[]= new double[SIZE];
		double za[]= new double[SIZE];
		
		tic("muli (array):");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				za[j] = xa[j] * ya[j];
		toc();

		tic("muli (double buffer:");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				z.data.put(j, x.data.get(j) * y.data.get(j));
		toc();
		
		tic("divi():");
		for (int i = 0; i < ITERS/10; i++)
			x.divi(y, z);
		toc();

		tic("addi():");
		for (int i = 0; i < ITERS; i++)
			x.addi(y, z);
		toc();

		tic("addi() array:");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				za[j] = xa[j] + ya[j];
		toc();

		tic("addi() doublebuffer:");
		for (int i = 0; i < ITERS; i++)
			for (int j = 0; j < SIZE; j++)
				z.data.put(j, x.data.get(j) + y.data.get(j));
		toc();
		
		tic("subi():");
		for (int i = 0; i < ITERS; i++)
			x.subi(y, z);
		toc();
	}
}
