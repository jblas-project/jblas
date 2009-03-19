package org.jblas.core;

public class Functions {
	public static double sinc(double x) {
		if (x == 0)
			return 1.0;
		else
			return Math.sin(Math.PI * x) / (Math.PI * x);
	}
}
