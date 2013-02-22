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

import org.jblas.exceptions.SizeException;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class ComplexDoubleMatrix {
	
	public int rows;
	public int columns;
	public int length;
	public double[] data = null; // rows are contiguous

	/**************************************************************************
	 * 
	 * Constructors and factory functions
	 * 
	 **************************************************************************/

	/**
   * Create a new matrix with <i>newRows</i> rows, <i>newColumns</i> columns
	 * using <i>newData></i> as the data.
	 */
	public ComplexDoubleMatrix(int newRows, int newColumns, double... newData) {
		rows = newRows;
		columns = newColumns;
		length = rows * columns;

    if (newData.length != 2 * newRows * newColumns)
			throw new IllegalArgumentException(
					"Passed data must match matrix dimensions.");
                data = newData;
	}
	
	/**
	 * Creates a new <i>n</i> times <i>m</i> <tt>ComplexDoubleMatrix</tt>.
	 * @param newRows the number of rows (<i>n</i>) of the new matrix.
	 * @param newColumns the number of columns (<i>m</i>) of the new matrix.
	 */
	public ComplexDoubleMatrix(int newRows, int newColumns) {
		this(newRows, newColumns, new double[2 * newRows * newColumns]);
	}
	
	/**
	 * Creates a new <tt>ComplexDoubleMatrix</tt> of size 0 times 0.
	 */
	public ComplexDoubleMatrix() {
		this(0, 0, null);
	}

	/**
	 * Create a Matrix of length <tt>len</tt>. By default, this creates a row vector.
	 * @param len
	 */
	public ComplexDoubleMatrix(int len) {
		this(len, 1, new double[2 * len]);
	}
	
	public ComplexDoubleMatrix(double[] newData) {
		this(newData.length/2);
				
		data = newData;
	}

	public ComplexDoubleMatrix(ComplexDouble[] newData) {
		this(newData.length);
				
		for (int i = 0; i < newData.length; i++)
			put(i, newData[i]);
	}
		
        
  /** Construct a complex matrix from a real matrix. */
  public ComplexDoubleMatrix(DoubleMatrix m) {
    this(m.rows, m.columns);

    NativeBlas.dcopy(m.length, m.data, 0, 1, data, 0, 2);
  }

  /** Construct a complex matrix from separate real and imaginary parts. Either
   * part can be set to null in which case it will be ignored.
   */
  public ComplexDoubleMatrix(DoubleMatrix real, DoubleMatrix imag) {
      this(real.rows, real.columns);
      real.assertSameSize(imag);

      if (real != null)
          NativeBlas.dcopy(length, real.data, 0, 1, data, 0, 2);
      if (imag != null)
          NativeBlas.dcopy(length, imag.data, 0, 1, data, 1, 2);
  }
        
        /**
	 * Creates a new matrix by reading it from a file.
	 * @param filename the path and name of the file to read the matrix from
	 * @throws IOException 
	 */
	public ComplexDoubleMatrix(String filename) throws IOException {
		load(filename);
	}
	
	/**
	 * Creates a new <i>n</i> times <i>m</i> <tt>ComplexDoubleMatrix</tt> from
	 * the given <i>n</i> times <i>m</i> 2D data array. The first dimension of the array makes the
	 * rows (<i>n</i>) and the second dimension the columns (<i>m</i>). For example, the
	 * given code <br/><br/>
	 * <code>new ComplexDoubleMatrix(new double[][]{{1d, 2d, 3d}, {4d, 5d, 6d}, {7d, 8d, 9d}}).print();</code><br/><br/>
	 * will constructs the following matrix:
	 * <pre>
	 * 1.0	2.0	3.0
	 * 4.0	5.0	6.0
	 * 7.0	8.0	9.0
	 * </pre>.
	 * @param data <i>n</i> times <i>m</i> data array
	 */ 
	public ComplexDoubleMatrix(double[][] data) {
		this(data.length, data[0].length);
						
		for (int r = 0; r < rows; r++)
			assert(data[r].length == columns);
		
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				put(r, c, data[r][c]);
	}
	
	/**
	 * Creates a new matrix in which all values are equal 0.
	 * @param rows number of rows
	 * @param columns number of columns
	 * @return new matrix
	 */
	public static ComplexDoubleMatrix zeros(int rows, int columns) {
		return new ComplexDoubleMatrix(rows, columns);
	}
	
	public static ComplexDoubleMatrix zeros(int length) {
		return zeros(length, 1);
	}

	/**
	 * Creates a new matrix in which all values are equal 1.
	 * @param rows number of rows
	 * @param columns number of columns
	 * @return new matrix
	 */
	public static ComplexDoubleMatrix ones(int rows, int columns) {
		ComplexDoubleMatrix m = new ComplexDoubleMatrix(rows, columns);
		
		for (int i = 0; i < rows * columns; i++)
			m.put(i, 1.0);
		
		return m;
	}
	
	public static ComplexDoubleMatrix ones(int length) {
		return ones(length, 1);
	}
	
	/**
	 * Creates a new matrix where the values of the given vector are the diagonal values of
	 * the matrix.
	 * @param x the diagonal values
	 * @return new matrix
	 */
	public static ComplexDoubleMatrix diag(ComplexDoubleMatrix x) {
		ComplexDoubleMatrix m = new ComplexDoubleMatrix(x.length, x.length);
		
		for (int i = 0; i < x.length; i++)
			m.put(i, i, x.get(i));
		
		return m;
	}

  /**
   * Construct a matrix of arbitrary shape and set the diagonal according
   * to a passed vector.
   *
   * length of needs to be smaller than rows or columns.
   *
   * @param x vector to fill the diagonal with
   * @param rows number of rows of the resulting matrix
   * @param columns number of columns of the resulting matrix
   * @return a matrix with dimensions rows * columns whose diagonal elements are filled by x
   */
  public static ComplexDoubleMatrix diag(ComplexDoubleMatrix x, int rows, int columns) {
    if (x.length > rows || x.length > columns) {
      throw new SizeException("Length of diagonal matrix must be larger than both rows and columns.");
    }
    
    ComplexDoubleMatrix m = new ComplexDoubleMatrix(rows, columns);

    for (int i = 0; i < x.length; i++)
      m.put(i, i, x.get(i));

    return m;
  }
	
	/**
	 * Create a 1 * 1 - matrix. For many operations, this matrix functions like a
	 * normal double
	 * @param s value of the matrix
	 * @return the constructed ComplexDoubleMatrix 
	 */
	public static ComplexDoubleMatrix scalar(double s) {
		ComplexDoubleMatrix m = new ComplexDoubleMatrix(1, 1);
		m.put(0, 0, s);
		return m;
	}
	
	/** Test whether a matrix is scalar */
	public boolean isScalar() {
		return length == 1;
	}
	
	/** Return the first element of the matrix */
	public ComplexDouble scalar() {
		return get(0);
	}
	
	public static ComplexDoubleMatrix concatHorizontally(ComplexDoubleMatrix A, ComplexDoubleMatrix B) {
		if (A.rows != B.rows)
			throw new SizeException("Matrices don't have same number of rows.");
		
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(A.rows, A.columns + B.columns);
		SimpleBlas.copy(A, result);
		NativeBlas.zcopy(B.length, B.data, 0, 1, result.data, A.length, 1);
		return result;
	}

	public static ComplexDoubleMatrix concatVertically(ComplexDoubleMatrix A, ComplexDoubleMatrix B) {
		if (A.columns != B.columns)
			throw new SizeException("Matrices don't have same number of columns.");
		
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(A.rows + B.rows, A.columns);

		for (int i = 0; i < A.columns; i++) {
			NativeBlas.zcopy(A.rows, A.data, A.index(0, i), 1, result.data, result.index(0, i), 1);
			NativeBlas.zcopy(B.rows, B.data, B.index(0, i), 1, result.data, result.index(A.rows, i), 1);
		}
		
		return result;
	}
	
	/**************************************************************************
	 * Working with slices (Man! 30+ methods just to make this a bit flexible...) 
	 */

	public ComplexDoubleMatrix get(int[] indices) {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			result.put(i, get(indices[i]));
		
		return result;
	}
	
	public ComplexDoubleMatrix get(int r, int[] indices) {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(1, indices.length);
		
		for (int i = 0; i < indices.length; i++)
			result.put(i, get(r, indices[i]));
		
		return result;
	}
	
	public ComplexDoubleMatrix get(int[] indices, int c) {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(indices.length, 1);
		
		for (int i = 0; i < indices.length; i++)
			result.put(i, get(indices[i], c));
		
		return result;
	}
	
	public ComplexDoubleMatrix get(int[] rindices, int[] cindices) {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(rindices.length, cindices.length);
		
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				result.put(i, j, get(rindices[i], cindices[j]));
		
		return result;
	}
	
	public ComplexDoubleMatrix get(ComplexDoubleMatrix indices) {
		return get(indices.findIndices());
	}

	public ComplexDoubleMatrix get(int r, ComplexDoubleMatrix indices) {
		return get(r, indices.findIndices());
	}
	
	public ComplexDoubleMatrix get(ComplexDoubleMatrix indices, int c) {
		return get(indices.findIndices(), c);
	}

	public ComplexDoubleMatrix get(ComplexDoubleMatrix rindices, ComplexDoubleMatrix cindices) {
		return get(rindices.findIndices(), cindices.findIndices());
	}
	
	private void checkLength(int l) {
		if (length != l)
			throw new SizeException("Matrix does not have the necessary length (" + length + " != " + l + ").");
	}

	private void checkRows(int r) {
		if (rows != r)
			throw new SizeException("Matrix does not have the necessary length (" + length + " != " + r + ").");
	}
	
	private void checkColumns(int c) {
		if (columns != c)
			throw new SizeException("Matrix does not have the necessary length (" + length + " != " + c + ").");
	}

	public ComplexDoubleMatrix put(int[] indices, ComplexDoubleMatrix x) {
		if (x.isScalar())
			return put(indices, x.scalar());
		x.checkLength(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			put(indices[i], x.get(i));
		
		return this;
	}
	
	public ComplexDoubleMatrix put(int r, int[] indices, ComplexDoubleMatrix x) {
		if (x.isScalar())
			return put(r, indices, x.scalar());
		x.checkColumns(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			put(r, indices[i], x.get(i));
		
		return this;
	}
	
	public ComplexDoubleMatrix put(int[] indices, int c, ComplexDoubleMatrix x) {
		if (x.isScalar())
			return put(indices, c, x.scalar());		
		x.checkRows(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			put(indices[i], c, x.get(i));
		
		return this;
	}
	
	public ComplexDoubleMatrix put(int[] rindices, int[] cindices, ComplexDoubleMatrix x) {
		if (x.isScalar())
			return put(rindices, cindices, x.scalar());		
		x.checkRows(rindices.length);
		x.checkColumns(cindices.length);
		
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], x.get(i,j));
		
		return this;
	}
	
	public ComplexDoubleMatrix put(int[] indices, double v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], v);
		
		return this;
	}

	public ComplexDoubleMatrix putReal(int[] indices, double v) {
		return put(indices, v);
	}

	public ComplexDoubleMatrix putImag(int[] indices, double v) {
		for (int i = 0; i < indices.length; i++)
			putImag(indices[i], v);
		
		return this;
	}

	public ComplexDoubleMatrix put(int[] indices, ComplexDouble v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], v);
		
		return this;
	}

	public ComplexDoubleMatrix put(int r, int[] indices, double v) {
		for (int i = 0; i < indices.length; i++)
			put(r, indices[i], v);
		
		return this;
	}

	public ComplexDoubleMatrix putReal(int r, int[] indices, double v) {
		return put(r, indices, v);
	}

	public ComplexDoubleMatrix putImag(int r, int[] indices, double v) {
		for (int i = 0; i < indices.length; i++)
			putImag(r, indices[i], v);
		
		return this;
	}

	public ComplexDoubleMatrix put(int r, int[] indices, ComplexDouble v) {
		for (int i = 0; i < indices.length; i++)
			put(r, indices[i], v);
		
		return this;
	}

	public ComplexDoubleMatrix put(int[] indices, int c, double v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], c, v);
		
		return this;
	}
	
	public ComplexDoubleMatrix putReal(int[] indices, int c, double v) {
		return put(indices, c, v);
	}
	
	public ComplexDoubleMatrix putImag(int[] indices, int c, double v) {
		for (int i = 0; i < indices.length; i++)
			putImag(indices[i], c, v);
		
		return this;
	}
	
	public ComplexDoubleMatrix put(int[] indices, int c, ComplexDouble v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], c, v);
		
		return this;
 	}
	
	public ComplexDoubleMatrix put(int[] rindices, int[] cindices, double v) {
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], v);
		
		return this;
	}
	
	public ComplexDoubleMatrix putReal(int[] rindices, int[] cindices, double v) {
		return put(rindices, cindices, v);
	}
	
	public ComplexDoubleMatrix putImag(int[] rindices, int[] cindices, double v) {
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], v);
		
		return this;
	}

	public ComplexDoubleMatrix put(int[] rindices, int[] cindices, ComplexDouble v) {
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], v);
		
		return this;
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix indices, ComplexDoubleMatrix v) {
		return put(indices.findIndices(), v);
	}

	public ComplexDoubleMatrix put(int r, ComplexDoubleMatrix indices, ComplexDoubleMatrix v) {
		return put(r, indices.findIndices(), v);
	}
	
	public ComplexDoubleMatrix put(ComplexDoubleMatrix indices, int c, ComplexDoubleMatrix v) {
		return put(indices.findIndices(), c, v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix rindices, ComplexDoubleMatrix cindices, ComplexDoubleMatrix v) {
		return put(rindices.findIndices(), cindices.findIndices(), v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix indices, double v) {
		return put(indices.findIndices(), v);
	}

	public ComplexDoubleMatrix putReal(ComplexDoubleMatrix indices, double v) {
		return put(indices, v);
	}

	public ComplexDoubleMatrix putImag(ComplexDoubleMatrix indices, double v) {
		return putImag(indices.findIndices(), v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix indices, ComplexDouble v) {
		return put(indices.findIndices(), v);
	}
	
	public ComplexDoubleMatrix put(int r, ComplexDoubleMatrix indices, double v) {
		return put(r, indices.findIndices(), v);
	}
	
	public ComplexDoubleMatrix putReal(int r, ComplexDoubleMatrix indices, double v) {
		return put(r, indices, v);
	}

	public ComplexDoubleMatrix putImag(int r, ComplexDoubleMatrix indices, double v) {
		return putImag(r, indices.findIndices(), v);
	}

	public ComplexDoubleMatrix put(int r, ComplexDoubleMatrix indices, ComplexDouble v) {
		return put(r, indices.findIndices(), v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix indices, int c, double v) {
		return put(indices.findIndices(), c, v);
	}

	public ComplexDoubleMatrix putReal(ComplexDoubleMatrix indices, int c, double v) {
		return put(indices, c, v);
	}

	public ComplexDoubleMatrix putImag(ComplexDoubleMatrix indices, int c, double v) {
		return putImag(indices.findIndices(), c, v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix indices, int c, ComplexDouble v) {
		return put(indices.findIndices(), c, v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix rindices, ComplexDoubleMatrix cindices, double v) {
		return put(rindices.findIndices(), cindices.findIndices(), v);
	}

	public ComplexDoubleMatrix putReal(ComplexDoubleMatrix rindices, ComplexDoubleMatrix cindices, double v) {
		return putReal(rindices, cindices, v);
	}

	public ComplexDoubleMatrix putImag(ComplexDoubleMatrix rindices, ComplexDoubleMatrix cindices, double v) {
		return putImag(rindices.findIndices(), cindices.findIndices(), v);
	}

	public ComplexDoubleMatrix put(ComplexDoubleMatrix rindices, ComplexDoubleMatrix cindices, ComplexDouble v) {
		return put(rindices.findIndices(), cindices.findIndices(), v);
	}

	
	public int[] findIndices() {
		int len = 0;
		for (int i = 0; i < length; i++)
			if (!get(i).isZero())
				len++;
		
		int[] indices = new int[len];
		int c = 0;
		
		for (int i = 0; i < length; i++)
			if (!get(i).isZero())
				indices[c++] = i;
		
		return indices;
	}
	
	/**************************************************************************
	 * Basic operations (copying, resizing, element access)
	 */
	
	/** Return transposed copy of this matrix */
	public ComplexDoubleMatrix transpose() {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(columns, rows);

                ComplexDouble c = new ComplexDouble(0);

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
				result.put(j, i, get(i, j, c));
		
		return result;
	}

        public ComplexDoubleMatrix hermitian() {
            ComplexDoubleMatrix result = new ComplexDoubleMatrix(columns, rows);

            ComplexDouble c = new ComplexDouble(0);

            for (int i = 0; i < rows; i++)
                for (int j = 0; j < columns; j++)
                    result.put(j, i, get(i, j, c).conji());
            return result;
        }

        /**
         * Compute complex conjugate (in-place).
         */
        public ComplexDoubleMatrix conji() {
            ComplexDouble c = new ComplexDouble(0.0);
            for (int i = 0; i < length; i++)
                put(i, get(i, c).conji());
            return this;
        }

        /**
         * Compute complex conjugate.
         */
        public ComplexDoubleMatrix conj() {
            return dup().conji();
        }

		
	/** Compare two matrices.
	 * @param o Object to compare to
	 * @return true if and only if other is also a ComplexDoubleMatrix which has the same size and the
	 * maximal absolute difference in matrix elements is smaller thatn 1e-6.  */
	public boolean equals(Object o) {
		if (!(o instanceof ComplexDoubleMatrix))
			return false;

		ComplexDoubleMatrix other = (ComplexDoubleMatrix) o;

		if (!sameSize(other))
			return false;
		
		DoubleMatrix diff = MatrixFunctions.absi(sub(other)).getReal();
		
		return diff.max() / (rows * columns) < 1e-6;
	}

	
	/** Resize the matrix. All elements will be set to zero. */
	public void resize(int newRows, int newColumns) {
		rows = newRows;
		columns = newColumns;
		length = newRows * newColumns;
		data = new double[2 * rows * columns];
	}

	
	/** Reshape the matrix. Number of elements must not change. */
	public ComplexDoubleMatrix reshape(int newRows, int newColumns) {
		if (length != newRows * newColumns)
			throw new IllegalArgumentException(
					"Number of elements must not change.");

		rows = newRows;
		columns = newColumns;
		
		return this;
	}

	/** Checks whether two matrices have the same size. */
	public boolean sameSize(ComplexDoubleMatrix a) {
		return rows == a.rows && columns == a.columns;
	}

	/** 
	 * Assert that two matrices have the same size.
	 * 
	 * @param a the other matrix
	 * @throws SizeException if matrix sizes don't match. 
	 * */
	public void assertSameSize(ComplexDoubleMatrix a) {
		if (!sameSize(a))
			throw new SizeException("Matrices must have the same size.");
	}
	
	/** 
	 * Check whether this can be multiplied with a. 
	 * 
	 * @param a right-hand-side of the multiplication.
	 * @return true iff <tt>this.columns == a.rows</tt>
	 */
	public boolean multipliesWith(ComplexDoubleMatrix a) {
		return columns == a.rows;
	}
	
	public void assertMultipliesWith(ComplexDoubleMatrix a) {
		if (!multipliesWith(a))
			throw new SizeException("Number of columns of left matrix must be equal to number of rows of right matrix.");
	}
	
	public boolean sameLength(ComplexDoubleMatrix a) {
		return length == a.length;
	}
	
	public void assertSameLength(ComplexDoubleMatrix a) {
		if (!sameLength(a))
			throw new SizeException("Matrices must have same length (is: " + length + " and " + a.length + ")");
	}
	
	/** Copy ComplexDoubleMatrix a to this. this a is resized if necessary. */
	public ComplexDoubleMatrix copy(ComplexDoubleMatrix a) {
		if (!sameSize(a))
			resize(a.rows, a.columns);
		
		SimpleBlas.copy(a, this);
		return a;
	}
	
	/** Returns a duplicate of this matrix. Geometry is the same (including offsets, transpose, etc.),
	 * but the buffer is not shared.
	 */
	public ComplexDoubleMatrix dup() {
		ComplexDoubleMatrix out = new ComplexDoubleMatrix(rows, columns);

                JavaBlas.rcopy(2*length, data, 0, 1, out.data, 0, 1);
		
		return out;
	}
	
	public ComplexDoubleMatrix swapColumns(int i, int j) {
		NativeBlas.zswap(rows, data, index(0, i), 1, data, index(0, j), 1);
		return this;
	}
	
	public ComplexDoubleMatrix swapRows(int i, int j) {
		NativeBlas.zswap(columns, data, index(i, 0), rows, data, index(j, 0), rows);
		return this;
	}
		
	/** Set matrix element */
	public ComplexDoubleMatrix put(int rowIndex, int columnIndex, double value) {
		data[2*index(rowIndex, columnIndex)] =  value;
		return this;
	}

	public ComplexDoubleMatrix put(int rowIndex, int columnIndex, double realValue, double complexValue) {
		data[2*index(rowIndex, columnIndex)] =  realValue;
		data[2*index(rowIndex, columnIndex)+1] =  complexValue;
		return this;
	}

        public ComplexDoubleMatrix put(int rowIndex, int columnIndex, ComplexDouble value) {
		int i = 2*index(rowIndex, columnIndex);
		data[i] = value.real(); data[i+1] = value.imag();
		return this;
	}

	public ComplexDoubleMatrix putReal(int rowIndex, int columnIndex, double value) {
		data[2*index(rowIndex, columnIndex)] = value;
		return this;
	}

	public ComplexDoubleMatrix putImag(int rowIndex, int columnIndex, double value) {
		data[2*index(rowIndex, columnIndex)+1] = value;
		return this;
	}
	
	/** Retrieve matrix element */
	public ComplexDouble get(int rowIndex, int columnIndex) {
            int i = 2*index(rowIndex, columnIndex);
            return new ComplexDouble(data[i], data[i+1]);
	}

        /** Get matrix element, passing the variable to store the result. */
        public ComplexDouble get(int rowIndex, int columnIndex, ComplexDouble result) {
            return get(index(rowIndex, columnIndex), result);
        }
	
	public DoubleMatrix getReal() {
		DoubleMatrix result = new DoubleMatrix(rows, columns);
		
		NativeBlas.dcopy(length, data, 0, 2, result.data, 0, 1);
		
		return result;
	}

	/** Get index of an element */
	public int index(int rowIndex, int columnIndex) {
		return rows * columnIndex + rowIndex;
	}

  /** Compute the row index of a linear index. */
  public int indexRows(int i) {
    return i - indexColumns(i) * rows;
  }

  /** Compute the column index of a linear index. */
  public int indexColumns(int i) {
    return i / rows;
  }


	public ComplexDouble get(int i) {
		return new ComplexDouble(data[i * 2], data[i * 2 + 1]);
	}
	
        public ComplexDouble get(int i, ComplexDouble result) {
            return result.set(data[i * 2], data[i*2+1]);
        }
        
	public double getReal(int i) {
		return data[2*i];
	}
	
	public double getImag(int i) {
		return data[2*i + 1]; 
	}

	public ComplexDoubleMatrix put(int i, double v) {
		data[2*i] = v;
		return this;
	}

        public ComplexDoubleMatrix put(int i, double r, double c) {
            data[2*i] = r;
            data[2*i+1] = c;
            return this;
        }
	
	public ComplexDoubleMatrix put(int i, ComplexDouble v) {
		data[2*i] = v.real();
		data[2*i+1] = v.imag();
		return this;
	}
	
	public ComplexDoubleMatrix putReal(int i, double v) {
		return put(i, v);
	}
	
	public ComplexDoubleMatrix putImag(int i, double v) {
		data[2*i+1] = v;
		return this;
	}

	public int getRows() {
		return rows;
	}
	
	public int getColumns() {
		return columns;
	}
	
	public int getLength() {
		return length;
	}
	
	/** Checks whether the matrix is empty. */
	public boolean isEmpty() {
		return columns == 0 || rows == 0;
	}
	
	/** Checks whether the matrix is square. */
	public boolean isSquare() {
		return columns == rows;
	}
	
	public void assertSquare() {
		if (!isSquare())
			throw new SizeException("Matrix must be square!");
	}
	
	/** Checks whether the matrix is a vector. */
	public boolean isVector() {
		return columns == 1 || rows == 1;
	}
	
	public boolean isRowVector() {
		return columns == 1;
	}
	
	public boolean isColumnVector() {
		return rows == 1;
	}
		
        /** Get diagonal of the matrix. */
	public ComplexDoubleMatrix diag() {
		ComplexDoubleMatrix d = new ComplexDoubleMatrix(rows);
		NativeBlas.zcopy(rows, data, 0, rows + 1, d.data, 0, 1);
		return d;
	}
        
        /** Get real part of the matrix. */
        public DoubleMatrix real() {
            DoubleMatrix result = new DoubleMatrix(rows, columns);
            NativeBlas.dcopy(length, data, 0, 2, result.data, 0, 1);
            return result;
        }
        
        /** Get imaginary part of the matrix. */
        public DoubleMatrix imag() {
            DoubleMatrix result = new DoubleMatrix(rows, columns);
            NativeBlas.dcopy(length, data, 1, 2, result.data, 0, 1);
            return result;            
        }

	
	/** 
	 * Pretty-print this matrix to <tt>System.out</tt>. 
	 * */
	public void print() {
		System.out.println(toString());
	}

	/** 
	 * Generate string representation of this matrix 
	 * (multi-line).
	 * */
	public String toString() {
		StringBuilder s = new StringBuilder();

		s.append("[");
		
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				s.append(get(i, j));
				if (j < columns - 1)
					s.append(", ");
			}
			if (i < rows - 1)
				s.append("; ");
		}

		s.append("]");
		
		return s.toString();
	}

	public double[] toDoubleArray() {
		double[] array = new double[2*length];
		
		for (int i = 0; i < 2*length; i++)
			array[i] = data[i];
		
		return array;
	}
	
	public ComplexDouble[] toArray() {
		ComplexDouble[] array = new ComplexDouble[length];
		
		for (int i = 0; i < length; i++)
			array[i] = get(i);
		
		return array;		
	}
	
	public ComplexDouble[][] toArray2() {
		ComplexDouble[][] array = new ComplexDouble[rows][columns];
		
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				array[r][c] = get(r, c);
				
		return array;
	}
	
	public boolean[] toBooleanArray() {
		boolean[] array = new boolean[length];
		
		for (int i = 0; i < length; i++)
			array[i] = get(i).isZero() ? false : true;
		
		return array;
	}
	
	public boolean[][] toBooleanArray2() {
		boolean[][] array = new boolean[rows][columns];
		
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++)
				array[r][c] = get(r, c).isZero() ? false : true;
				
		return array;
	}

	/**************************************************************************
	 * Arithmetic Operations
	 */

	/** 
	 * Ensures that the result vector has the same length as this. If not,
	 * resizing result is tried, which fails if result == this or result == other.
	 */
	private void ensureResultLength(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (!sameLength(result)) {
			if (result == this || result == other)
				throw new SizeException("Cannot resize result matrix because it is used in-place.");
			result.resize(rows, columns);
		}
	}

	/** Add two matrices. */
	public ComplexDoubleMatrix addi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (other.isScalar())
			return addi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
		if (result == this)
			SimpleBlas.axpy(ComplexDouble.UNIT, other, result);
		else if (result == other)
			SimpleBlas.axpy(ComplexDouble.UNIT, this, result);
		else {
			SimpleBlas.copy(this, result);
			SimpleBlas.axpy(ComplexDouble.UNIT, other, result);
		}

		return result;
	}
	
	/** Add a scalar to a matrix. */
	public ComplexDoubleMatrix addi(ComplexDouble v, ComplexDoubleMatrix result) {
		ensureResultLength(null, result);
		
		for (int i = 0; i < length; i++)
			result.put(i, get(i).add(v));
		return result;
	}
	
	public ComplexDoubleMatrix addi(double v, ComplexDoubleMatrix result) {
		return addi(new ComplexDouble(v), result);
	}

	/** Subtract two matrices. */
	public ComplexDoubleMatrix subi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (other.isScalar())
			return subi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
		if (result == this)
			SimpleBlas.axpy(ComplexDouble.NEG_UNIT, other, result);
		else if (result == other) {
			SimpleBlas.scal(ComplexDouble.NEG_UNIT, result);
			SimpleBlas.axpy(ComplexDouble.UNIT, this, result);
		}
		else {
			SimpleBlas.copy(this, result);
			SimpleBlas.axpy(ComplexDouble.NEG_UNIT, other, result);
		}
		return result;
	}
	
	/** Subtract a scalar from a matrix */
	public ComplexDoubleMatrix subi(ComplexDouble v, ComplexDoubleMatrix result) {
		ensureResultLength(null, result);
		
		for (int i = 0; i < length; i++)
			result.put(i, get(i).sub(v));
		return result;
	}
	
	public ComplexDoubleMatrix subi(double v, ComplexDoubleMatrix result) {
		return subi(new ComplexDouble(v), result);
	}

	/** 
	 * Subtract two matrices, but subtract first from second matrix, that is, 
	 * compute <em>result = other - this</em>. 
	 * */
	public ComplexDoubleMatrix rsubi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		return other.subi(this, result);
	}
	
	/** Subtract a matrix from a scalar */
	public ComplexDoubleMatrix rsubi(ComplexDouble a, ComplexDoubleMatrix result) {
		ensureResultLength(null, result);
		
		for (int i = 0; i < length; i++)
			result.put(i, a.sub(get(i)));
		return result;
	}

	public ComplexDoubleMatrix rsubi(double a, ComplexDoubleMatrix result) {
		return rsubi(new ComplexDouble(a), result);
	}

	/** (Elementwise) Multiplication */ 
	public ComplexDoubleMatrix muli(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (other.isScalar())
			return muli(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
                ComplexDouble c = new ComplexDouble(0.0);
                ComplexDouble d = new ComplexDouble(0.0);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c).muli(other.get(i, d)));
		return result;
	}
	
	/** (Elementwise) Multiplication with a scalar */
	public ComplexDoubleMatrix muli(ComplexDouble v, ComplexDoubleMatrix result) {
		ensureResultLength(null, result);
		
                ComplexDouble c = new ComplexDouble(0.0);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c).muli(v));
		return result;
	}

	public ComplexDoubleMatrix muli(double v, ComplexDoubleMatrix result) {
		return muli(new ComplexDouble(v), result);
	}

	/** Matrix-Matrix Multiplication */
	public ComplexDoubleMatrix mmuli(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (other.isScalar())
			return muli(other.scalar(), result);

		/* check sizes and resize if necessary */
		assertMultipliesWith(other);
		if (result.rows != rows || result.columns != other.columns) {
			if (result != this && result != other)
				result.resize(rows, other.columns);
			else
				throw new SizeException("Cannot resize result matrix because it is used in-place.");
		}
		
		if (result == this || result == other) {
			/* actually, blas cannot do multiplications in-place. Therefore, we will fake by
			 * allocating a temporary object on the side and copy the result later.
			 */
			ComplexDoubleMatrix temp = new ComplexDoubleMatrix(result.rows, result.columns);
			SimpleBlas.gemm(ComplexDouble.UNIT, this, other, ComplexDouble.ZERO, temp);
			SimpleBlas.copy(temp, result);
		}
		else {
			SimpleBlas.gemm(ComplexDouble.UNIT, this, other, ComplexDouble.ZERO, result);
		}		
		return result;
	}
	
	/** Matrix-Matrix Multiplication with a scalar (for symmetry, does the
	 * same as muli(scalar)
	 */
	public ComplexDoubleMatrix mmuli(ComplexDouble v, ComplexDoubleMatrix result) {
		return muli(v, result);
	}

	public ComplexDoubleMatrix mmuli(double v, ComplexDoubleMatrix result) {
		return muli(v, result);
	}
	
	/** (Elementwise) division */
	public ComplexDoubleMatrix divi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (other.isScalar())
			return divi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
                ComplexDouble c1 = new ComplexDouble(0.0);
                ComplexDouble c2 = new ComplexDouble(0.0);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c1).divi(other.get(i, c2)));
		return result;
	}
		
	/** (Elementwise) division with a scalar */
	public ComplexDoubleMatrix divi(ComplexDouble a, ComplexDoubleMatrix result) {
		ensureResultLength(null, result);
		
                ComplexDouble c = new ComplexDouble(0.0);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c).divi(a));
		return result;
	}	

	public ComplexDoubleMatrix divi(double a, ComplexDoubleMatrix result) {
		return divi(new ComplexDouble(a), result);
	}

	/** 
	 * (Elementwise) division, with operands switched. Computes
	 * <em>result = other / this</em>. */
	public ComplexDoubleMatrix rdivi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
		if (other.isScalar())
			return divi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);

                ComplexDouble c1 = new ComplexDouble(0.0);
                ComplexDouble c2 = new ComplexDouble(0.0);

		for (int i = 0; i < length; i++)
			result.put(i, other.get(i, c1).divi(get(i, c2)));
		return result;
	}
		
	/** (Elementwise) division with a scalar, with operands switched. Computes
	 * <em>result = a / this</em>.*/
	public ComplexDoubleMatrix rdivi(ComplexDouble a, ComplexDoubleMatrix result) {
		ensureResultLength(null, result);

                ComplexDouble c1 = new ComplexDouble(0.0);
                ComplexDouble c2 = new ComplexDouble(0.0);

		for (int i = 0; i < length; i++) {
                    c1.copy(a);
                    result.put(i, c1.divi(get(i, c2)));                    
                }
		return result;
	}

	public ComplexDoubleMatrix rdivi(double a, ComplexDoubleMatrix result) {
		return rdivi(new ComplexDouble(a), result);
	}
	
	public ComplexDoubleMatrix negi() {
                ComplexDouble c = new ComplexDouble(0.0);
		for (int i = 0; i < length; i++)
			put(i, get(i, c).negi());
		return this;
	}
	
	public ComplexDoubleMatrix neg() {
		return dup().negi();
	}

	public ComplexDoubleMatrix noti() {
                ComplexDouble c = new ComplexDouble(0.0);
		for (int i = 0; i < length; i++)
			put(i, get(i, c).isZero() ? 1.0 : 0.0);
		return this;
	}
	
	public ComplexDoubleMatrix not() {
		return dup().noti();
	}
	
	public ComplexDoubleMatrix truthi() {
                ComplexDouble c = new ComplexDouble(0.0);
		for (int i = 0; i < length; i++)
			put(i, get(i, c).isZero() ? 0.0 : 1.0);
		return this;
	}
	
	public ComplexDoubleMatrix truth() {
		return dup().truthi();
	}

	/****************************************************************
	 * Rank one-updates
	 */
	
	/** Computes a rank-1-update A = A + alpha * x * y'. */ 
	public ComplexDoubleMatrix rankOneUpdate(ComplexDouble alpha, ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		if (rows != x.length)
			throw new SizeException("Vector x has wrong length (" + x.length + " != " + rows + ").");
		if (columns != y.length)
			throw new SizeException("Vector y has wrong length (" + x.length + " != " + columns + ").");			
		
		SimpleBlas.gerc(alpha, x, y, this);
		return this;
	}

	public ComplexDoubleMatrix rankOneUpdate(double alpha, ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		return rankOneUpdate(new ComplexDouble(alpha), x, y);
	}

	/** Computes a rank-1-update A = A + alpha * x * x'. */ 
	public ComplexDoubleMatrix rankOneUpdate(double alpha, ComplexDoubleMatrix x) {
		return rankOneUpdate(new ComplexDouble(alpha), x, x);
	}

	/** Computes a rank-1-update A = A + alpha * x * x'. */ 
	public ComplexDoubleMatrix rankOneUpdate(ComplexDouble alpha, ComplexDoubleMatrix x) {
		return rankOneUpdate(alpha, x, x);
	}

	/** Computes a rank-1-update A = A + x * x'. */ 
	public ComplexDoubleMatrix rankOneUpdate(ComplexDoubleMatrix x) {
		return rankOneUpdate(1.0, x, x);
	}

	/** Computes a rank-1-update A = A + x * y'. */ 
	public ComplexDoubleMatrix rankOneUpdate(ComplexDoubleMatrix x, ComplexDoubleMatrix y) {
		return rankOneUpdate(1.0, x, y);
	}

	/****************************************************************
	 * Logical operations
	 */
	
	public ComplexDouble sum() {
		ComplexDouble s = new ComplexDouble(0.0);
                ComplexDouble c = new ComplexDouble(0.0);
		for (int i = 0; i < length; i++)
			s.addi(get(i, c));
		return s;
	}
	
	public ComplexDouble mean() {
		return sum().div((double)length);
	}
	
	/** Computes this^T * other */
	public ComplexDouble dotc(ComplexDoubleMatrix other) {
		return SimpleBlas.dotc(this, other);
	}
	
	/** Computes this^H * other */
	public ComplexDouble dotu(ComplexDoubleMatrix other) {
		return SimpleBlas.dotu(this, other);
	}

	public double norm2() {
		return SimpleBlas.nrm2(this);
	}
	
	public double normmax() {
		int i = SimpleBlas.iamax(this);
		return get(i).abs();
	}

	public double norm1() {
		return SimpleBlas.asum(this);
	}
		
	/** Return a vector containing the sums of the columns (having number of columns many entries) */
	public ComplexDoubleMatrix columnSums() {
		ComplexDoubleMatrix v =
                        new ComplexDoubleMatrix(1, columns);

		for (int c = 0; c < columns; c++)
			v.put(c, getColumn(c).sum());

		return v;
	}

	public ComplexDoubleMatrix columnMeans() {
		return columnSums().divi(rows);
	}
	
	public ComplexDoubleMatrix rowSums() {
		ComplexDoubleMatrix v = new ComplexDoubleMatrix(rows);

		for (int r = 0; r < rows; r++)
			v.put(r, getRow(r).sum());

		return v;
	}

	public ComplexDoubleMatrix rowMeans() {
		return rowSums().divi(columns);
	}

	public ComplexDoubleMatrix getColumn(int c) {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(rows, 1);
		NativeBlas.zcopy(rows, data, index(0, c), 1, result.data, 0, 1);
		return result;
	}
	
	public void putColumn(int c, ComplexDoubleMatrix v) {
		NativeBlas.zcopy(rows, v.data, 0, 1, data, index(0, c), 1);
	}

	public ComplexDoubleMatrix getRow(int r) {
		ComplexDoubleMatrix result = new ComplexDoubleMatrix(1, columns);
		NativeBlas.zcopy(columns, data, index(r, 0), rows, result.data, 0, 1);
		return result;
	}
	
	public void putRow(int r, ComplexDoubleMatrix v) {
		NativeBlas.zcopy(columns, v.data, 0, 1, data, index(r, 0), rows);
	}

	/**************************************************************************
	 * Elementwise Functions
	 */

	/** Add a row vector to all rows of the matrix */
	public void addRowVector(ComplexDoubleMatrix x) {
		for (int r = 0; r < rows; r++) {
			NativeBlas.zaxpy(columns, ComplexDouble.UNIT, x.data, 0, 1, data, index(r, 0), rows);
		}
	}

	/** Add a vector to all columns of the matrix */
	public void addColumnVector(ComplexDoubleMatrix x) {
		for (int c = 0; c < columns; c++) {
			NativeBlas.zaxpy(rows, ComplexDouble.UNIT, x.data, 0, 1, data, index(0, c), 1);
		}
	}

       	/** Add a row vector to all rows of the matrix */
	public void subRowVector(ComplexDoubleMatrix x) {
		for (int r = 0; r < rows; r++) {
			NativeBlas.zaxpy(columns, ComplexDouble.NEG_UNIT, x.data, 0, 1, data, index(r, 0), rows);
		}
	}

	/** Add a vector to all columns of the matrix */
	public void subColumnVector(ComplexDoubleMatrix x) {
		for (int c = 0; c < columns; c++) {
			NativeBlas.zaxpy(rows, ComplexDouble.NEG_UNIT, x.data, 0, 1, data, index(0, c), 1);
		}
	}

	/**
	 * Writes out this matrix to the given data stream.
	 * @param dos the data output stream to write to.
	 * @throws IOException 
	 */
	public void out(DataOutputStream dos) throws IOException {
		dos.writeUTF("double");
		dos.writeInt(columns);
		dos.writeInt(rows);
		
		dos.writeInt(data.length);
		for(int i=0; i < data.length;i++)
			dos.writeDouble(data[i]);
	}
	
	/**
	 * Reads in a matrix from the given data stream. Note
	 * that the old data of this matrix will be discarded.
	 * @param dis the data input stream to read from.
	 * @throws IOException 
	 */
	public void in(DataInputStream dis) throws IOException {
		if(!dis.readUTF().equals("double")) 
			throw new IllegalStateException("The matrix in the specified file is not of the correct type!");
		
		this.columns	= dis.readInt();
		this.rows		= dis.readInt();

		final int MAX = dis.readInt();
		data = new double[MAX];
		for(int i=0; i < MAX;i++)
			data[i] = dis.readDouble();
	}	
	
	/**
	 * Saves this matrix to the specified file.
	 * @param filename the file to write the matrix in.
	 * @throws IOException thrown on errors while writing the matrix to the file
	 */
	public void save(String filename) throws IOException {
		DataOutputStream dos = new DataOutputStream(new FileOutputStream(filename, false));
		this.out(dos);
	}
	
	/**
	 * Loads a matrix from a file into this matrix. Note that the old data
	 * of this matrix will be discarded.
	 * @param filename the file to read the matrix from
	 * @throws IOException thrown on errors while reading the matrix
	 */
	public void load(String filename) throws IOException {
		DataInputStream dis = new DataInputStream(new FileInputStream(filename));
		this.in(dis);
	}

	/****************************************************************
	 * Autogenerated code
	 */
	
	/***** Code for operators ***************************************/ 

	/* Overloads for the usual arithmetic operations */
	/*#
	 def gen_overloads(base, result_rows, result_cols); <<-EOS
	public ComplexDoubleMatrix #{base}i(ComplexDoubleMatrix other) {
		return #{base}i(other, this);
	}
	 	
	public ComplexDoubleMatrix #{base}(ComplexDoubleMatrix other) {
	  	return #{base}i(other, new ComplexDoubleMatrix(#{result_rows}, #{result_cols}));
	}

	public ComplexDoubleMatrix #{base}i(ComplexDouble v) {
		return #{base}i(v, this);
	}
	
	public ComplexDoubleMatrix #{base}i(double v) {
		return #{base}i(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix #{base}(ComplexDouble v) {
		return #{base}i(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix #{base}(double v) {
		return #{base}i(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	
	 	EOS
	  end
	#*/

	/* Generating code for logical operators. This not only generates the stubs 
	 * but really all of the code.
	 */
	
	/*#
	 def gen_compare(name, op); <<-EOS
	 public ComplexDoubleMatrix #{name}i(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	    if (other.isScalar())
	       return #{name}i(other.scalar(), result);
	       
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble c1 = new ComplexDouble(0.0);
                ComplexDouble c2 = new ComplexDouble(0.0);
          
                for (int i = 0; i < length; i++)
                    result.put(i, get(i, c1).#{op}(other.get(i, c2)) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix #{name}i(ComplexDoubleMatrix other) {
	   return #{name}i(other, this);
	 }
	 
	 public ComplexDoubleMatrix #{name}(ComplexDoubleMatrix other) {
	   return #{name}i(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix #{name}i(ComplexDouble value, ComplexDoubleMatrix result) {
	   ensureResultLength(null, result);
           ComplexDouble c = new ComplexDouble(0.0);
	   for (int i = 0; i < length; i++)
	     result.put(i, get(i, c).#{op}(value) ? 1.0 : 0.0);
	   return result;
	 }

	 public ComplexDoubleMatrix #{name}i(double value, ComplexDoubleMatrix result) {
	   return #{name}i(new ComplexDouble(value), result);
	 }

	 public ComplexDoubleMatrix #{name}i(ComplexDouble value) {
	   return #{name}i(value, this);
	 }
	 
	 public ComplexDoubleMatrix #{name}i(double value) {
	   return #{name}i(new ComplexDouble(value));
	 }
	 
	 public ComplexDoubleMatrix #{name}(ComplexDouble value) {
	   return #{name}i(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix #{name}(double value) {
	   return #{name}i(new ComplexDouble(value));
	 }

	 EOS
	 end
	 #*/
	
	/*#
	 def gen_logical(name, op); <<-EOS
	 public ComplexDoubleMatrix #{name}i(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble t1 = new ComplexDouble(0.0);
                ComplexDouble t2 = new ComplexDouble(0.0);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) #{op} (!other.get(i, t2).isZero()) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix #{name}i(ComplexDoubleMatrix other) {
	   return #{name}i(other, this);
	 }
	 
	 public ComplexDoubleMatrix #{name}(ComplexDoubleMatrix other) {
	   return #{name}i(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix #{name}i(ComplexDouble value, ComplexDoubleMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexDouble t = new ComplexDouble(0.0);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() #{op} val ? 1.0 : 0.0);
	   return result;
	 }

 	 public ComplexDoubleMatrix #{name}i(double value, ComplexDoubleMatrix result) {
 	   return #{name}i(new ComplexDouble(value), result);
 	 }

	 public ComplexDoubleMatrix #{name}i(ComplexDouble value) {
	   return #{name}i(value, this);
	 }

 	 public ComplexDoubleMatrix #{name}i(double value) {
 	   return #{name}i(new ComplexDouble(value), this);
 	 }

	 public ComplexDoubleMatrix #{name}(ComplexDouble value) {
	   return #{name}i(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix #{name}(double value) {
	   return #{name}i(new ComplexDouble(value));
	 }
	 EOS
	 end
	 #*/

	/*# collect(gen_overloads('add', 'rows', 'columns'),
	  gen_overloads('sub', 'rows', 'columns'),
	  gen_overloads('rsub', 'rows', 'columns'),
	  gen_overloads('div', 'rows', 'columns'),
	  gen_overloads('rdiv', 'rows', 'columns'),
	  gen_overloads('mul', 'rows', 'columns'),
	  gen_overloads('mmul', 'rows', 'other.columns'),
	  gen_compare('eq', 'eq'),
	  gen_compare('ne', 'eq'),
	  gen_logical('and', '&'),
	  gen_logical('or', '|'),
	  gen_logical('xor', '^'))
	 #*/
//RJPP-BEGIN------------------------------------------------------------
	public ComplexDoubleMatrix addi(ComplexDoubleMatrix other) {
		return addi(other, this);
	}
	 	
	public ComplexDoubleMatrix add(ComplexDoubleMatrix other) {
	  	return addi(other, new ComplexDoubleMatrix(rows, columns));
	}

	public ComplexDoubleMatrix addi(ComplexDouble v) {
		return addi(v, this);
	}
	
	public ComplexDoubleMatrix addi(double v) {
		return addi(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix add(ComplexDouble v) {
		return addi(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix add(double v) {
		return addi(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	public ComplexDoubleMatrix subi(ComplexDoubleMatrix other) {
		return subi(other, this);
	}
	 	
	public ComplexDoubleMatrix sub(ComplexDoubleMatrix other) {
	  	return subi(other, new ComplexDoubleMatrix(rows, columns));
	}

	public ComplexDoubleMatrix subi(ComplexDouble v) {
		return subi(v, this);
	}
	
	public ComplexDoubleMatrix subi(double v) {
		return subi(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix sub(ComplexDouble v) {
		return subi(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix sub(double v) {
		return subi(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	public ComplexDoubleMatrix rsubi(ComplexDoubleMatrix other) {
		return rsubi(other, this);
	}
	 	
	public ComplexDoubleMatrix rsub(ComplexDoubleMatrix other) {
	  	return rsubi(other, new ComplexDoubleMatrix(rows, columns));
	}

	public ComplexDoubleMatrix rsubi(ComplexDouble v) {
		return rsubi(v, this);
	}
	
	public ComplexDoubleMatrix rsubi(double v) {
		return rsubi(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix rsub(ComplexDouble v) {
		return rsubi(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix rsub(double v) {
		return rsubi(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	public ComplexDoubleMatrix divi(ComplexDoubleMatrix other) {
		return divi(other, this);
	}
	 	
	public ComplexDoubleMatrix div(ComplexDoubleMatrix other) {
	  	return divi(other, new ComplexDoubleMatrix(rows, columns));
	}

	public ComplexDoubleMatrix divi(ComplexDouble v) {
		return divi(v, this);
	}
	
	public ComplexDoubleMatrix divi(double v) {
		return divi(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix div(ComplexDouble v) {
		return divi(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix div(double v) {
		return divi(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	public ComplexDoubleMatrix rdivi(ComplexDoubleMatrix other) {
		return rdivi(other, this);
	}
	 	
	public ComplexDoubleMatrix rdiv(ComplexDoubleMatrix other) {
	  	return rdivi(other, new ComplexDoubleMatrix(rows, columns));
	}

	public ComplexDoubleMatrix rdivi(ComplexDouble v) {
		return rdivi(v, this);
	}
	
	public ComplexDoubleMatrix rdivi(double v) {
		return rdivi(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix rdiv(ComplexDouble v) {
		return rdivi(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix rdiv(double v) {
		return rdivi(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	public ComplexDoubleMatrix muli(ComplexDoubleMatrix other) {
		return muli(other, this);
	}
	 	
	public ComplexDoubleMatrix mul(ComplexDoubleMatrix other) {
	  	return muli(other, new ComplexDoubleMatrix(rows, columns));
	}

	public ComplexDoubleMatrix muli(ComplexDouble v) {
		return muli(v, this);
	}
	
	public ComplexDoubleMatrix muli(double v) {
		return muli(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix mul(ComplexDouble v) {
		return muli(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix mul(double v) {
		return muli(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	public ComplexDoubleMatrix mmuli(ComplexDoubleMatrix other) {
		return mmuli(other, this);
	}
	 	
	public ComplexDoubleMatrix mmul(ComplexDoubleMatrix other) {
	  	return mmuli(other, new ComplexDoubleMatrix(rows, other.columns));
	}

	public ComplexDoubleMatrix mmuli(ComplexDouble v) {
		return mmuli(v, this);
	}
	
	public ComplexDoubleMatrix mmuli(double v) {
		return mmuli(new ComplexDouble(v), this);
	}

	public ComplexDoubleMatrix mmul(ComplexDouble v) {
		return mmuli(v, new ComplexDoubleMatrix(rows, columns));
	} 	

	public ComplexDoubleMatrix mmul(double v) {
		return mmuli(new ComplexDouble(v), new ComplexDoubleMatrix(rows, columns));
	} 	
	 	

	 public ComplexDoubleMatrix eqi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	    if (other.isScalar())
	       return eqi(other.scalar(), result);
	       
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble c1 = new ComplexDouble(0.0);
                ComplexDouble c2 = new ComplexDouble(0.0);
          
                for (int i = 0; i < length; i++)
                    result.put(i, get(i, c1).eq(other.get(i, c2)) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix eqi(ComplexDoubleMatrix other) {
	   return eqi(other, this);
	 }
	 
	 public ComplexDoubleMatrix eq(ComplexDoubleMatrix other) {
	   return eqi(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix eqi(ComplexDouble value, ComplexDoubleMatrix result) {
	   ensureResultLength(null, result);
           ComplexDouble c = new ComplexDouble(0.0);
	   for (int i = 0; i < length; i++)
	     result.put(i, get(i, c).eq(value) ? 1.0 : 0.0);
	   return result;
	 }

	 public ComplexDoubleMatrix eqi(double value, ComplexDoubleMatrix result) {
	   return eqi(new ComplexDouble(value), result);
	 }

	 public ComplexDoubleMatrix eqi(ComplexDouble value) {
	   return eqi(value, this);
	 }
	 
	 public ComplexDoubleMatrix eqi(double value) {
	   return eqi(new ComplexDouble(value));
	 }
	 
	 public ComplexDoubleMatrix eq(ComplexDouble value) {
	   return eqi(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix eq(double value) {
	   return eqi(new ComplexDouble(value));
	 }


	 public ComplexDoubleMatrix nei(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	    if (other.isScalar())
	       return nei(other.scalar(), result);
	       
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble c1 = new ComplexDouble(0.0);
                ComplexDouble c2 = new ComplexDouble(0.0);
          
                for (int i = 0; i < length; i++)
                    result.put(i, get(i, c1).eq(other.get(i, c2)) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix nei(ComplexDoubleMatrix other) {
	   return nei(other, this);
	 }
	 
	 public ComplexDoubleMatrix ne(ComplexDoubleMatrix other) {
	   return nei(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix nei(ComplexDouble value, ComplexDoubleMatrix result) {
	   ensureResultLength(null, result);
           ComplexDouble c = new ComplexDouble(0.0);
	   for (int i = 0; i < length; i++)
	     result.put(i, get(i, c).eq(value) ? 1.0 : 0.0);
	   return result;
	 }

	 public ComplexDoubleMatrix nei(double value, ComplexDoubleMatrix result) {
	   return nei(new ComplexDouble(value), result);
	 }

	 public ComplexDoubleMatrix nei(ComplexDouble value) {
	   return nei(value, this);
	 }
	 
	 public ComplexDoubleMatrix nei(double value) {
	   return nei(new ComplexDouble(value));
	 }
	 
	 public ComplexDoubleMatrix ne(ComplexDouble value) {
	   return nei(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix ne(double value) {
	   return nei(new ComplexDouble(value));
	 }


	 public ComplexDoubleMatrix andi(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble t1 = new ComplexDouble(0.0);
                ComplexDouble t2 = new ComplexDouble(0.0);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) & (!other.get(i, t2).isZero()) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix andi(ComplexDoubleMatrix other) {
	   return andi(other, this);
	 }
	 
	 public ComplexDoubleMatrix and(ComplexDoubleMatrix other) {
	   return andi(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix andi(ComplexDouble value, ComplexDoubleMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexDouble t = new ComplexDouble(0.0);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() & val ? 1.0 : 0.0);
	   return result;
	 }

 	 public ComplexDoubleMatrix andi(double value, ComplexDoubleMatrix result) {
 	   return andi(new ComplexDouble(value), result);
 	 }

	 public ComplexDoubleMatrix andi(ComplexDouble value) {
	   return andi(value, this);
	 }

 	 public ComplexDoubleMatrix andi(double value) {
 	   return andi(new ComplexDouble(value), this);
 	 }

	 public ComplexDoubleMatrix and(ComplexDouble value) {
	   return andi(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix and(double value) {
	   return andi(new ComplexDouble(value));
	 }

	 public ComplexDoubleMatrix ori(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble t1 = new ComplexDouble(0.0);
                ComplexDouble t2 = new ComplexDouble(0.0);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) | (!other.get(i, t2).isZero()) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix ori(ComplexDoubleMatrix other) {
	   return ori(other, this);
	 }
	 
	 public ComplexDoubleMatrix or(ComplexDoubleMatrix other) {
	   return ori(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix ori(ComplexDouble value, ComplexDoubleMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexDouble t = new ComplexDouble(0.0);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() | val ? 1.0 : 0.0);
	   return result;
	 }

 	 public ComplexDoubleMatrix ori(double value, ComplexDoubleMatrix result) {
 	   return ori(new ComplexDouble(value), result);
 	 }

	 public ComplexDoubleMatrix ori(ComplexDouble value) {
	   return ori(value, this);
	 }

 	 public ComplexDoubleMatrix ori(double value) {
 	   return ori(new ComplexDouble(value), this);
 	 }

	 public ComplexDoubleMatrix or(ComplexDouble value) {
	   return ori(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix or(double value) {
	   return ori(new ComplexDouble(value));
	 }

	 public ComplexDoubleMatrix xori(ComplexDoubleMatrix other, ComplexDoubleMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexDouble t1 = new ComplexDouble(0.0);
                ComplexDouble t2 = new ComplexDouble(0.0);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) ^ (!other.get(i, t2).isZero()) ? 1.0 : 0.0);
	   return result;
	 }
	 
	 public ComplexDoubleMatrix xori(ComplexDoubleMatrix other) {
	   return xori(other, this);
	 }
	 
	 public ComplexDoubleMatrix xor(ComplexDoubleMatrix other) {
	   return xori(other, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix xori(ComplexDouble value, ComplexDoubleMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexDouble t = new ComplexDouble(0.0);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() ^ val ? 1.0 : 0.0);
	   return result;
	 }

 	 public ComplexDoubleMatrix xori(double value, ComplexDoubleMatrix result) {
 	   return xori(new ComplexDouble(value), result);
 	 }

	 public ComplexDoubleMatrix xori(ComplexDouble value) {
	   return xori(value, this);
	 }

 	 public ComplexDoubleMatrix xori(double value) {
 	   return xori(new ComplexDouble(value), this);
 	 }

	 public ComplexDoubleMatrix xor(ComplexDouble value) {
	   return xori(value, new ComplexDoubleMatrix(rows, columns));
	 }
	 
	 public ComplexDoubleMatrix xor(double value) {
	   return xori(new ComplexDouble(value));
	 }
//RJPP-END--------------------------------------------------------------
}
