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
 * <p>General functions which are geometric in nature.</p>
 * 
 * <p>For example, computing all pairwise squared distances between all columns of a matrix.</p>
 */
public class Geometry {
	
	/**
	 * <p>Compute the pairwise squared distances between all columns of the two
	 * matrices.</p>
	 * 
	 * <p>An efficient way to do this is to observe that <i>(x-y)^2 = x^2 - 2xy - y^2</i>
	 * and to then properly carry out the computation with matrices.</p>
	 */
	public static DoubleMatrix pairwiseSquaredDistances(DoubleMatrix X, DoubleMatrix Y) {
		if (X.rows != Y.rows)
			throw new IllegalArgumentException(
					"Matrices must have same number of rows");
	
		DoubleMatrix XX = X.mul(X).columnSums();
		DoubleMatrix YY = Y.mul(Y).columnSums();
	
		DoubleMatrix Z = X.transpose().mmul(Y);
		Z.muli(-2.0); //Z.print();
		Z.addiColumnVector(XX);
		Z.addiRowVector(YY);
	
		return Z;
	}

        /** Center a vector (subtract mean from all elements (in-place). */
	public static DoubleMatrix center(DoubleMatrix x) {
		return x.subi(x.mean());
	}
	
        /** Center the rows of a matrix (in-place). */
	public static DoubleMatrix centerRows(DoubleMatrix x) {
                DoubleMatrix temp = new DoubleMatrix(x.columns);
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, center(x.getRow(r, temp)));
		return x;
	}
	
        /** Center the columns of a matrix (in-place). */
	public static DoubleMatrix centerColumns(DoubleMatrix x) {
                DoubleMatrix temp = new DoubleMatrix(x.rows);
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, center(x.getColumn(c, temp)));
		return x;
	}
	
        /** Normalize a vector (scale such that its Euclidean norm is 1) (in-place). */
	public static DoubleMatrix normalize(DoubleMatrix x) {
		return x.divi(x.norm2());
	}

        /** Normalize the rows of a matrix (in-place). */
	public static DoubleMatrix normalizeRows(DoubleMatrix x) {
                DoubleMatrix temp = new DoubleMatrix(x.columns);
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, normalize(x.getRow(r, temp)));
		return x;
	}
	
        /** Normalize the columns of a matrix (in-place). */
	public static DoubleMatrix normalizeColumns(DoubleMatrix x) {
                DoubleMatrix temp = new DoubleMatrix(x.rows);
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, normalize(x.getColumn(c, temp)));
		return x;
	}

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
	
	/**
	 * <p>Compute the pairwise squared distances between all columns of the two
	 * matrices.</p>
	 * 
	 * <p>An efficient way to do this is to observe that <i>(x-y)^2 = x^2 - 2xy - y^2</i>
	 * and to then properly carry out the computation with matrices.</p>
	 */
	public static FloatMatrix pairwiseSquaredDistances(FloatMatrix X, FloatMatrix Y) {
		if (X.rows != Y.rows)
			throw new IllegalArgumentException(
					"Matrices must have same number of rows");
	
		FloatMatrix XX = X.mul(X).columnSums();
		FloatMatrix YY = Y.mul(Y).columnSums();
	
		FloatMatrix Z = X.transpose().mmul(Y);
		Z.muli(-2.0f); //Z.print();
		Z.addiColumnVector(XX);
		Z.addiRowVector(YY);
	
		return Z;
	}

        /** Center a vector (subtract mean from all elements (in-place). */
	public static FloatMatrix center(FloatMatrix x) {
		return x.subi(x.mean());
	}
	
        /** Center the rows of a matrix (in-place). */
	public static FloatMatrix centerRows(FloatMatrix x) {
                FloatMatrix temp = new FloatMatrix(x.columns);
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, center(x.getRow(r, temp)));
		return x;
	}
	
        /** Center the columns of a matrix (in-place). */
	public static FloatMatrix centerColumns(FloatMatrix x) {
                FloatMatrix temp = new FloatMatrix(x.rows);
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, center(x.getColumn(c, temp)));
		return x;
	}
	
        /** Normalize a vector (scale such that its Euclidean norm is 1) (in-place). */
	public static FloatMatrix normalize(FloatMatrix x) {
		return x.divi(x.norm2());
	}

        /** Normalize the rows of a matrix (in-place). */
	public static FloatMatrix normalizeRows(FloatMatrix x) {
                FloatMatrix temp = new FloatMatrix(x.columns);
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, normalize(x.getRow(r, temp)));
		return x;
	}
	
        /** Normalize the columns of a matrix (in-place). */
	public static FloatMatrix normalizeColumns(FloatMatrix x) {
                FloatMatrix temp = new FloatMatrix(x.rows);
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, normalize(x.getColumn(c, temp)));
		return x;
	}

//END
}
