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

public class ComplexFloatMatrix {
	
	public int rows;
	public int columns;
	public int length;
	public float[] data = null; // rows are contiguous

	/**************************************************************************
	 * 
	 * Constructors and factory functions
	 * 
	 **************************************************************************/

	/**
   * Create a new matrix with <i>newRows</i> rows, <i>newColumns</i> columns
	 * using <i>newData></i> as the data.
	 */
	public ComplexFloatMatrix(int newRows, int newColumns, float... newData) {
		rows = newRows;
		columns = newColumns;
		length = rows * columns;

    if (newData.length != 2 * newRows * newColumns)
			throw new IllegalArgumentException(
					"Passed data must match matrix dimensions.");
                data = newData;
	}
	
	/**
	 * Creates a new <i>n</i> times <i>m</i> <tt>ComplexFloatMatrix</tt>.
	 * @param newRows the number of rows (<i>n</i>) of the new matrix.
	 * @param newColumns the number of columns (<i>m</i>) of the new matrix.
	 */
	public ComplexFloatMatrix(int newRows, int newColumns) {
		this(newRows, newColumns, new float[2 * newRows * newColumns]);
	}
	
	/**
	 * Creates a new <tt>ComplexFloatMatrix</tt> of size 0 times 0.
	 */
	public ComplexFloatMatrix() {
		this(0, 0, null);
	}

	/**
	 * Create a Matrix of length <tt>len</tt>. By default, this creates a row vector.
	 * @param len
	 */
	public ComplexFloatMatrix(int len) {
		this(len, 1, new float[2 * len]);
	}
	
	public ComplexFloatMatrix(float[] newData) {
		this(newData.length/2);
				
		data = newData;
	}

	public ComplexFloatMatrix(ComplexFloat[] newData) {
		this(newData.length);
				
		for (int i = 0; i < newData.length; i++)
			put(i, newData[i]);
	}
		
        
  /** Construct a complex matrix from a real matrix. */
  public ComplexFloatMatrix(FloatMatrix m) {
    this(m.rows, m.columns);

    NativeBlas.scopy(m.length, m.data, 0, 1, data, 0, 2);
  }

  /** Construct a complex matrix from separate real and imaginary parts. Either
   * part can be set to null in which case it will be ignored.
   */
  public ComplexFloatMatrix(FloatMatrix real, FloatMatrix imag) {
      this(real.rows, real.columns);
      real.assertSameSize(imag);

      if (real != null)
          NativeBlas.scopy(length, real.data, 0, 1, data, 0, 2);
      if (imag != null)
          NativeBlas.scopy(length, imag.data, 0, 1, data, 1, 2);
  }
        
        /**
	 * Creates a new matrix by reading it from a file.
	 * @param filename the path and name of the file to read the matrix from
	 * @throws IOException 
	 */
	public ComplexFloatMatrix(String filename) throws IOException {
		load(filename);
	}
	
	/**
	 * Creates a new <i>n</i> times <i>m</i> <tt>ComplexFloatMatrix</tt> from
	 * the given <i>n</i> times <i>m</i> 2D data array. The first dimension of the array makes the
	 * rows (<i>n</i>) and the second dimension the columns (<i>m</i>). For example, the
	 * given code <br/><br/>
	 * <code>new ComplexFloatMatrix(new float[][]{{1d, 2d, 3d}, {4d, 5d, 6d}, {7d, 8d, 9d}}).print();</code><br/><br/>
	 * will constructs the following matrix:
	 * <pre>
	 * 1.0f	2.0f	3.0f
	 * 4.0f	5.0f	6.0f
	 * 7.0f	8.0f	9.0f
	 * </pre>.
	 * @param data <i>n</i> times <i>m</i> data array
	 */ 
	public ComplexFloatMatrix(float[][] data) {
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
	public static ComplexFloatMatrix zeros(int rows, int columns) {
		return new ComplexFloatMatrix(rows, columns);
	}
	
	public static ComplexFloatMatrix zeros(int length) {
		return zeros(length, 1);
	}

	/**
	 * Creates a new matrix in which all values are equal 1.
	 * @param rows number of rows
	 * @param columns number of columns
	 * @return new matrix
	 */
	public static ComplexFloatMatrix ones(int rows, int columns) {
		ComplexFloatMatrix m = new ComplexFloatMatrix(rows, columns);
		
		for (int i = 0; i < rows * columns; i++)
			m.put(i, 1.0f);
		
		return m;
	}
	
	public static ComplexFloatMatrix ones(int length) {
		return ones(length, 1);
	}
	
	/**
	 * Creates a new matrix where the values of the given vector are the diagonal values of
	 * the matrix.
	 * @param x the diagonal values
	 * @return new matrix
	 */
	public static ComplexFloatMatrix diag(ComplexFloatMatrix x) {
		ComplexFloatMatrix m = new ComplexFloatMatrix(x.length, x.length);
		
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
  public static ComplexFloatMatrix diag(ComplexFloatMatrix x, int rows, int columns) {
    if (x.length > rows || x.length > columns) {
      throw new SizeException("Length of diagonal matrix must be larger than both rows and columns.");
    }
    
    ComplexFloatMatrix m = new ComplexFloatMatrix(rows, columns);

    for (int i = 0; i < x.length; i++)
      m.put(i, i, x.get(i));

    return m;
  }
	
	/**
	 * Create a 1 * 1 - matrix. For many operations, this matrix functions like a
	 * normal float
	 * @param s value of the matrix
	 * @return the constructed ComplexFloatMatrix 
	 */
	public static ComplexFloatMatrix scalar(float s) {
		ComplexFloatMatrix m = new ComplexFloatMatrix(1, 1);
		m.put(0, 0, s);
		return m;
	}
	
	/** Test whether a matrix is scalar */
	public boolean isScalar() {
		return length == 1;
	}
	
	/** Return the first element of the matrix */
	public ComplexFloat scalar() {
		return get(0);
	}
	
	public static ComplexFloatMatrix concatHorizontally(ComplexFloatMatrix A, ComplexFloatMatrix B) {
		if (A.rows != B.rows)
			throw new SizeException("Matrices don't have same number of rows.");
		
		ComplexFloatMatrix result = new ComplexFloatMatrix(A.rows, A.columns + B.columns);
		SimpleBlas.copy(A, result);
		NativeBlas.ccopy(B.length, B.data, 0, 1, result.data, A.length, 1);
		return result;
	}

	public static ComplexFloatMatrix concatVertically(ComplexFloatMatrix A, ComplexFloatMatrix B) {
		if (A.columns != B.columns)
			throw new SizeException("Matrices don't have same number of columns.");
		
		ComplexFloatMatrix result = new ComplexFloatMatrix(A.rows + B.rows, A.columns);

		for (int i = 0; i < A.columns; i++) {
			NativeBlas.ccopy(A.rows, A.data, A.index(0, i), 1, result.data, result.index(0, i), 1);
			NativeBlas.ccopy(B.rows, B.data, B.index(0, i), 1, result.data, result.index(A.rows, i), 1);
		}
		
		return result;
	}
	
	/**************************************************************************
	 * Working with slices (Man! 30+ methods just to make this a bit flexible...) 
	 */

	public ComplexFloatMatrix get(int[] indices) {
		ComplexFloatMatrix result = new ComplexFloatMatrix(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			result.put(i, get(indices[i]));
		
		return result;
	}
	
	public ComplexFloatMatrix get(int r, int[] indices) {
		ComplexFloatMatrix result = new ComplexFloatMatrix(1, indices.length);
		
		for (int i = 0; i < indices.length; i++)
			result.put(i, get(r, indices[i]));
		
		return result;
	}
	
	public ComplexFloatMatrix get(int[] indices, int c) {
		ComplexFloatMatrix result = new ComplexFloatMatrix(indices.length, 1);
		
		for (int i = 0; i < indices.length; i++)
			result.put(i, get(indices[i], c));
		
		return result;
	}
	
	public ComplexFloatMatrix get(int[] rindices, int[] cindices) {
		ComplexFloatMatrix result = new ComplexFloatMatrix(rindices.length, cindices.length);
		
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				result.put(i, j, get(rindices[i], cindices[j]));
		
		return result;
	}
	
	public ComplexFloatMatrix get(ComplexFloatMatrix indices) {
		return get(indices.findIndices());
	}

	public ComplexFloatMatrix get(int r, ComplexFloatMatrix indices) {
		return get(r, indices.findIndices());
	}
	
	public ComplexFloatMatrix get(ComplexFloatMatrix indices, int c) {
		return get(indices.findIndices(), c);
	}

	public ComplexFloatMatrix get(ComplexFloatMatrix rindices, ComplexFloatMatrix cindices) {
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

	public ComplexFloatMatrix put(int[] indices, ComplexFloatMatrix x) {
		if (x.isScalar())
			return put(indices, x.scalar());
		x.checkLength(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			put(indices[i], x.get(i));
		
		return this;
	}
	
	public ComplexFloatMatrix put(int r, int[] indices, ComplexFloatMatrix x) {
		if (x.isScalar())
			return put(r, indices, x.scalar());
		x.checkColumns(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			put(r, indices[i], x.get(i));
		
		return this;
	}
	
	public ComplexFloatMatrix put(int[] indices, int c, ComplexFloatMatrix x) {
		if (x.isScalar())
			return put(indices, c, x.scalar());		
		x.checkRows(indices.length);
		
		for (int i = 0; i < indices.length; i++)
			put(indices[i], c, x.get(i));
		
		return this;
	}
	
	public ComplexFloatMatrix put(int[] rindices, int[] cindices, ComplexFloatMatrix x) {
		if (x.isScalar())
			return put(rindices, cindices, x.scalar());		
		x.checkRows(rindices.length);
		x.checkColumns(cindices.length);
		
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], x.get(i,j));
		
		return this;
	}
	
	public ComplexFloatMatrix put(int[] indices, float v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], v);
		
		return this;
	}

	public ComplexFloatMatrix putReal(int[] indices, float v) {
		return put(indices, v);
	}

	public ComplexFloatMatrix putImag(int[] indices, float v) {
		for (int i = 0; i < indices.length; i++)
			putImag(indices[i], v);
		
		return this;
	}

	public ComplexFloatMatrix put(int[] indices, ComplexFloat v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], v);
		
		return this;
	}

	public ComplexFloatMatrix put(int r, int[] indices, float v) {
		for (int i = 0; i < indices.length; i++)
			put(r, indices[i], v);
		
		return this;
	}

	public ComplexFloatMatrix putReal(int r, int[] indices, float v) {
		return put(r, indices, v);
	}

	public ComplexFloatMatrix putImag(int r, int[] indices, float v) {
		for (int i = 0; i < indices.length; i++)
			putImag(r, indices[i], v);
		
		return this;
	}

	public ComplexFloatMatrix put(int r, int[] indices, ComplexFloat v) {
		for (int i = 0; i < indices.length; i++)
			put(r, indices[i], v);
		
		return this;
	}

	public ComplexFloatMatrix put(int[] indices, int c, float v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], c, v);
		
		return this;
	}
	
	public ComplexFloatMatrix putReal(int[] indices, int c, float v) {
		return put(indices, c, v);
	}
	
	public ComplexFloatMatrix putImag(int[] indices, int c, float v) {
		for (int i = 0; i < indices.length; i++)
			putImag(indices[i], c, v);
		
		return this;
	}
	
	public ComplexFloatMatrix put(int[] indices, int c, ComplexFloat v) {
		for (int i = 0; i < indices.length; i++)
			put(indices[i], c, v);
		
		return this;
 	}
	
	public ComplexFloatMatrix put(int[] rindices, int[] cindices, float v) {
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], v);
		
		return this;
	}
	
	public ComplexFloatMatrix putReal(int[] rindices, int[] cindices, float v) {
		return put(rindices, cindices, v);
	}
	
	public ComplexFloatMatrix putImag(int[] rindices, int[] cindices, float v) {
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], v);
		
		return this;
	}

	public ComplexFloatMatrix put(int[] rindices, int[] cindices, ComplexFloat v) {
		for (int i = 0; i < rindices.length; i++)
			for (int j = 0; j < cindices.length; j++)
				put(rindices[i], cindices[j], v);
		
		return this;
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix indices, ComplexFloatMatrix v) {
		return put(indices.findIndices(), v);
	}

	public ComplexFloatMatrix put(int r, ComplexFloatMatrix indices, ComplexFloatMatrix v) {
		return put(r, indices.findIndices(), v);
	}
	
	public ComplexFloatMatrix put(ComplexFloatMatrix indices, int c, ComplexFloatMatrix v) {
		return put(indices.findIndices(), c, v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix rindices, ComplexFloatMatrix cindices, ComplexFloatMatrix v) {
		return put(rindices.findIndices(), cindices.findIndices(), v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix indices, float v) {
		return put(indices.findIndices(), v);
	}

	public ComplexFloatMatrix putReal(ComplexFloatMatrix indices, float v) {
		return put(indices, v);
	}

	public ComplexFloatMatrix putImag(ComplexFloatMatrix indices, float v) {
		return putImag(indices.findIndices(), v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix indices, ComplexFloat v) {
		return put(indices.findIndices(), v);
	}
	
	public ComplexFloatMatrix put(int r, ComplexFloatMatrix indices, float v) {
		return put(r, indices.findIndices(), v);
	}
	
	public ComplexFloatMatrix putReal(int r, ComplexFloatMatrix indices, float v) {
		return put(r, indices, v);
	}

	public ComplexFloatMatrix putImag(int r, ComplexFloatMatrix indices, float v) {
		return putImag(r, indices.findIndices(), v);
	}

	public ComplexFloatMatrix put(int r, ComplexFloatMatrix indices, ComplexFloat v) {
		return put(r, indices.findIndices(), v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix indices, int c, float v) {
		return put(indices.findIndices(), c, v);
	}

	public ComplexFloatMatrix putReal(ComplexFloatMatrix indices, int c, float v) {
		return put(indices, c, v);
	}

	public ComplexFloatMatrix putImag(ComplexFloatMatrix indices, int c, float v) {
		return putImag(indices.findIndices(), c, v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix indices, int c, ComplexFloat v) {
		return put(indices.findIndices(), c, v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix rindices, ComplexFloatMatrix cindices, float v) {
		return put(rindices.findIndices(), cindices.findIndices(), v);
	}

	public ComplexFloatMatrix putReal(ComplexFloatMatrix rindices, ComplexFloatMatrix cindices, float v) {
		return putReal(rindices, cindices, v);
	}

	public ComplexFloatMatrix putImag(ComplexFloatMatrix rindices, ComplexFloatMatrix cindices, float v) {
		return putImag(rindices.findIndices(), cindices.findIndices(), v);
	}

	public ComplexFloatMatrix put(ComplexFloatMatrix rindices, ComplexFloatMatrix cindices, ComplexFloat v) {
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
	public ComplexFloatMatrix transpose() {
		ComplexFloatMatrix result = new ComplexFloatMatrix(columns, rows);

                ComplexFloat c = new ComplexFloat(0);

		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
				result.put(j, i, get(i, j, c));
		
		return result;
	}

        public ComplexFloatMatrix hermitian() {
            ComplexFloatMatrix result = new ComplexFloatMatrix(columns, rows);

            ComplexFloat c = new ComplexFloat(0);

            for (int i = 0; i < rows; i++)
                for (int j = 0; j < columns; j++)
                    result.put(j, i, get(i, j, c).conji());
            return result;
        }

        /**
         * Compute complex conjugate (in-place).
         */
        public ComplexFloatMatrix conji() {
            ComplexFloat c = new ComplexFloat(0.0f);
            for (int i = 0; i < length; i++)
                put(i, get(i, c).conji());
            return this;
        }

        /**
         * Compute complex conjugate.
         */
        public ComplexFloatMatrix conj() {
            return dup().conji();
        }

		
	/** Compare two matrices.
	 * @param o Object to compare to
	 * @return true if and only if other is also a ComplexFloatMatrix which has the same size and the
	 * maximal absolute difference in matrix elements is smaller thatn 1e-6.  */
	public boolean equals(Object o) {
		if (!(o instanceof ComplexFloatMatrix))
			return false;

		ComplexFloatMatrix other = (ComplexFloatMatrix) o;

		if (!sameSize(other))
			return false;
		
		FloatMatrix diff = MatrixFunctions.absi(sub(other)).getReal();
		
		return diff.max() / (rows * columns) < 1e-6;
	}

	
	/** Resize the matrix. All elements will be set to zero. */
	public void resize(int newRows, int newColumns) {
		rows = newRows;
		columns = newColumns;
		length = newRows * newColumns;
		data = new float[2 * rows * columns];
	}

	
	/** Reshape the matrix. Number of elements must not change. */
	public ComplexFloatMatrix reshape(int newRows, int newColumns) {
		if (length != newRows * newColumns)
			throw new IllegalArgumentException(
					"Number of elements must not change.");

		rows = newRows;
		columns = newColumns;
		
		return this;
	}

	/** Checks whether two matrices have the same size. */
	public boolean sameSize(ComplexFloatMatrix a) {
		return rows == a.rows && columns == a.columns;
	}

	/** 
	 * Assert that two matrices have the same size.
	 * 
	 * @param a the other matrix
	 * @throws SizeException if matrix sizes don't match. 
	 * */
	public void assertSameSize(ComplexFloatMatrix a) {
		if (!sameSize(a))
			throw new SizeException("Matrices must have the same size.");
	}
	
	/** 
	 * Check whether this can be multiplied with a. 
	 * 
	 * @param a right-hand-side of the multiplication.
	 * @return true iff <tt>this.columns == a.rows</tt>
	 */
	public boolean multipliesWith(ComplexFloatMatrix a) {
		return columns == a.rows;
	}
	
	public void assertMultipliesWith(ComplexFloatMatrix a) {
		if (!multipliesWith(a))
			throw new SizeException("Number of columns of left matrix must be equal to number of rows of right matrix.");
	}
	
	public boolean sameLength(ComplexFloatMatrix a) {
		return length == a.length;
	}
	
	public void assertSameLength(ComplexFloatMatrix a) {
		if (!sameLength(a))
			throw new SizeException("Matrices must have same length (is: " + length + " and " + a.length + ")");
	}
	
	/** Copy ComplexFloatMatrix a to this. this a is resized if necessary. */
	public ComplexFloatMatrix copy(ComplexFloatMatrix a) {
		if (!sameSize(a))
			resize(a.rows, a.columns);
		
		SimpleBlas.copy(a, this);
		return a;
	}
	
	/** Returns a duplicate of this matrix. Geometry is the same (including offsets, transpose, etc.),
	 * but the buffer is not shared.
	 */
	public ComplexFloatMatrix dup() {
		ComplexFloatMatrix out = new ComplexFloatMatrix(rows, columns);

                JavaBlas.rcopy(2*length, data, 0, 1, out.data, 0, 1);
		
		return out;
	}
	
	public ComplexFloatMatrix swapColumns(int i, int j) {
		NativeBlas.cswap(rows, data, index(0, i), 1, data, index(0, j), 1);
		return this;
	}
	
	public ComplexFloatMatrix swapRows(int i, int j) {
		NativeBlas.cswap(columns, data, index(i, 0), rows, data, index(j, 0), rows);
		return this;
	}
		
	/** Set matrix element */
	public ComplexFloatMatrix put(int rowIndex, int columnIndex, float value) {
		data[2*index(rowIndex, columnIndex)] =  value;
		return this;
	}

	public ComplexFloatMatrix put(int rowIndex, int columnIndex, float realValue, float complexValue) {
		data[2*index(rowIndex, columnIndex)] =  realValue;
		data[2*index(rowIndex, columnIndex)+1] =  complexValue;
		return this;
	}

        public ComplexFloatMatrix put(int rowIndex, int columnIndex, ComplexFloat value) {
		int i = 2*index(rowIndex, columnIndex);
		data[i] = value.real(); data[i+1] = value.imag();
		return this;
	}

	public ComplexFloatMatrix putReal(int rowIndex, int columnIndex, float value) {
		data[2*index(rowIndex, columnIndex)] = value;
		return this;
	}

	public ComplexFloatMatrix putImag(int rowIndex, int columnIndex, float value) {
		data[2*index(rowIndex, columnIndex)+1] = value;
		return this;
	}
	
	/** Retrieve matrix element */
	public ComplexFloat get(int rowIndex, int columnIndex) {
            int i = 2*index(rowIndex, columnIndex);
            return new ComplexFloat(data[i], data[i+1]);
	}

        /** Get matrix element, passing the variable to store the result. */
        public ComplexFloat get(int rowIndex, int columnIndex, ComplexFloat result) {
            return get(index(rowIndex, columnIndex), result);
        }
	
	public FloatMatrix getReal() {
		FloatMatrix result = new FloatMatrix(rows, columns);
		
		NativeBlas.scopy(length, data, 0, 2, result.data, 0, 1);
		
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


	public ComplexFloat get(int i) {
		return new ComplexFloat(data[i * 2], data[i * 2 + 1]);
	}
	
        public ComplexFloat get(int i, ComplexFloat result) {
            return result.set(data[i * 2], data[i*2+1]);
        }
        
	public float getReal(int i) {
		return data[2*i];
	}
	
	public float getImag(int i) {
		return data[2*i + 1]; 
	}

	public ComplexFloatMatrix put(int i, float v) {
		data[2*i] = v;
		return this;
	}

        public ComplexFloatMatrix put(int i, float r, float c) {
            data[2*i] = r;
            data[2*i+1] = c;
            return this;
        }
	
	public ComplexFloatMatrix put(int i, ComplexFloat v) {
		data[2*i] = v.real();
		data[2*i+1] = v.imag();
		return this;
	}
	
	public ComplexFloatMatrix putReal(int i, float v) {
		return put(i, v);
	}
	
	public ComplexFloatMatrix putImag(int i, float v) {
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
	public ComplexFloatMatrix diag() {
		ComplexFloatMatrix d = new ComplexFloatMatrix(rows);
		NativeBlas.ccopy(rows, data, 0, rows + 1, d.data, 0, 1);
		return d;
	}
        
        /** Get real part of the matrix. */
        public FloatMatrix real() {
            FloatMatrix result = new FloatMatrix(rows, columns);
            NativeBlas.scopy(length, data, 0, 2, result.data, 0, 1);
            return result;
        }
        
        /** Get imaginary part of the matrix. */
        public FloatMatrix imag() {
            FloatMatrix result = new FloatMatrix(rows, columns);
            NativeBlas.scopy(length, data, 1, 2, result.data, 0, 1);
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

	public float[] toDoubleArray() {
		float[] array = new float[2*length];
		
		for (int i = 0; i < 2*length; i++)
			array[i] = data[i];
		
		return array;
	}
	
	public ComplexFloat[] toArray() {
		ComplexFloat[] array = new ComplexFloat[length];
		
		for (int i = 0; i < length; i++)
			array[i] = get(i);
		
		return array;		
	}
	
	public ComplexFloat[][] toArray2() {
		ComplexFloat[][] array = new ComplexFloat[rows][columns];
		
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
	private void ensureResultLength(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		if (!sameLength(result)) {
			if (result == this || result == other)
				throw new SizeException("Cannot resize result matrix because it is used in-place.");
			result.resize(rows, columns);
		}
	}

	/** Add two matrices. */
	public ComplexFloatMatrix addi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		if (other.isScalar())
			return addi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
		if (result == this)
			SimpleBlas.axpy(ComplexFloat.UNIT, other, result);
		else if (result == other)
			SimpleBlas.axpy(ComplexFloat.UNIT, this, result);
		else {
			SimpleBlas.copy(this, result);
			SimpleBlas.axpy(ComplexFloat.UNIT, other, result);
		}

		return result;
	}
	
	/** Add a scalar to a matrix. */
	public ComplexFloatMatrix addi(ComplexFloat v, ComplexFloatMatrix result) {
		ensureResultLength(null, result);
		
		for (int i = 0; i < length; i++)
			result.put(i, get(i).add(v));
		return result;
	}
	
	public ComplexFloatMatrix addi(float v, ComplexFloatMatrix result) {
		return addi(new ComplexFloat(v), result);
	}

	/** Subtract two matrices. */
	public ComplexFloatMatrix subi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		if (other.isScalar())
			return subi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
		if (result == this)
			SimpleBlas.axpy(ComplexFloat.NEG_UNIT, other, result);
		else if (result == other) {
			SimpleBlas.scal(ComplexFloat.NEG_UNIT, result);
			SimpleBlas.axpy(ComplexFloat.UNIT, this, result);
		}
		else {
			SimpleBlas.copy(this, result);
			SimpleBlas.axpy(ComplexFloat.NEG_UNIT, other, result);
		}
		return result;
	}
	
	/** Subtract a scalar from a matrix */
	public ComplexFloatMatrix subi(ComplexFloat v, ComplexFloatMatrix result) {
		ensureResultLength(null, result);
		
		for (int i = 0; i < length; i++)
			result.put(i, get(i).sub(v));
		return result;
	}
	
	public ComplexFloatMatrix subi(float v, ComplexFloatMatrix result) {
		return subi(new ComplexFloat(v), result);
	}

	/** 
	 * Subtract two matrices, but subtract first from second matrix, that is, 
	 * compute <em>result = other - this</em>. 
	 * */
	public ComplexFloatMatrix rsubi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		return other.subi(this, result);
	}
	
	/** Subtract a matrix from a scalar */
	public ComplexFloatMatrix rsubi(ComplexFloat a, ComplexFloatMatrix result) {
		ensureResultLength(null, result);
		
		for (int i = 0; i < length; i++)
			result.put(i, a.sub(get(i)));
		return result;
	}

	public ComplexFloatMatrix rsubi(float a, ComplexFloatMatrix result) {
		return rsubi(new ComplexFloat(a), result);
	}

	/** (Elementwise) Multiplication */ 
	public ComplexFloatMatrix muli(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		if (other.isScalar())
			return muli(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
                ComplexFloat c = new ComplexFloat(0.0f);
                ComplexFloat d = new ComplexFloat(0.0f);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c).muli(other.get(i, d)));
		return result;
	}
	
	/** (Elementwise) Multiplication with a scalar */
	public ComplexFloatMatrix muli(ComplexFloat v, ComplexFloatMatrix result) {
		ensureResultLength(null, result);
		
                ComplexFloat c = new ComplexFloat(0.0f);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c).muli(v));
		return result;
	}

	public ComplexFloatMatrix muli(float v, ComplexFloatMatrix result) {
		return muli(new ComplexFloat(v), result);
	}

	/** Matrix-Matrix Multiplication */
	public ComplexFloatMatrix mmuli(ComplexFloatMatrix other, ComplexFloatMatrix result) {
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
			ComplexFloatMatrix temp = new ComplexFloatMatrix(result.rows, result.columns);
			SimpleBlas.gemm(ComplexFloat.UNIT, this, other, ComplexFloat.ZERO, temp);
			SimpleBlas.copy(temp, result);
		}
		else {
			SimpleBlas.gemm(ComplexFloat.UNIT, this, other, ComplexFloat.ZERO, result);
		}		
		return result;
	}
	
	/** Matrix-Matrix Multiplication with a scalar (for symmetry, does the
	 * same as muli(scalar)
	 */
	public ComplexFloatMatrix mmuli(ComplexFloat v, ComplexFloatMatrix result) {
		return muli(v, result);
	}

	public ComplexFloatMatrix mmuli(float v, ComplexFloatMatrix result) {
		return muli(v, result);
	}
	
	/** (Elementwise) division */
	public ComplexFloatMatrix divi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		if (other.isScalar())
			return divi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);
		
                ComplexFloat c1 = new ComplexFloat(0.0f);
                ComplexFloat c2 = new ComplexFloat(0.0f);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c1).divi(other.get(i, c2)));
		return result;
	}
		
	/** (Elementwise) division with a scalar */
	public ComplexFloatMatrix divi(ComplexFloat a, ComplexFloatMatrix result) {
		ensureResultLength(null, result);
		
                ComplexFloat c = new ComplexFloat(0.0f);
                
		for (int i = 0; i < length; i++)
			result.put(i, get(i, c).divi(a));
		return result;
	}	

	public ComplexFloatMatrix divi(float a, ComplexFloatMatrix result) {
		return divi(new ComplexFloat(a), result);
	}

	/** 
	 * (Elementwise) division, with operands switched. Computes
	 * <em>result = other / this</em>. */
	public ComplexFloatMatrix rdivi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
		if (other.isScalar())
			return divi(other.scalar(), result);
		
		assertSameLength(other);
		ensureResultLength(other, result);

                ComplexFloat c1 = new ComplexFloat(0.0f);
                ComplexFloat c2 = new ComplexFloat(0.0f);

		for (int i = 0; i < length; i++)
			result.put(i, other.get(i, c1).divi(get(i, c2)));
		return result;
	}
		
	/** (Elementwise) division with a scalar, with operands switched. Computes
	 * <em>result = a / this</em>.*/
	public ComplexFloatMatrix rdivi(ComplexFloat a, ComplexFloatMatrix result) {
		ensureResultLength(null, result);

                ComplexFloat c1 = new ComplexFloat(0.0f);
                ComplexFloat c2 = new ComplexFloat(0.0f);

		for (int i = 0; i < length; i++) {
                    c1.copy(a);
                    result.put(i, c1.divi(get(i, c2)));                    
                }
		return result;
	}

	public ComplexFloatMatrix rdivi(float a, ComplexFloatMatrix result) {
		return rdivi(new ComplexFloat(a), result);
	}
	
	public ComplexFloatMatrix negi() {
                ComplexFloat c = new ComplexFloat(0.0f);
		for (int i = 0; i < length; i++)
			put(i, get(i, c).negi());
		return this;
	}
	
	public ComplexFloatMatrix neg() {
		return dup().negi();
	}

	public ComplexFloatMatrix noti() {
                ComplexFloat c = new ComplexFloat(0.0f);
		for (int i = 0; i < length; i++)
			put(i, get(i, c).isZero() ? 1.0f : 0.0f);
		return this;
	}
	
	public ComplexFloatMatrix not() {
		return dup().noti();
	}
	
	public ComplexFloatMatrix truthi() {
                ComplexFloat c = new ComplexFloat(0.0f);
		for (int i = 0; i < length; i++)
			put(i, get(i, c).isZero() ? 0.0f : 1.0f);
		return this;
	}
	
	public ComplexFloatMatrix truth() {
		return dup().truthi();
	}

	/****************************************************************
	 * Rank one-updates
	 */
	
	/** Computes a rank-1-update A = A + alpha * x * y'. */ 
	public ComplexFloatMatrix rankOneUpdate(ComplexFloat alpha, ComplexFloatMatrix x, ComplexFloatMatrix y) {
		if (rows != x.length)
			throw new SizeException("Vector x has wrong length (" + x.length + " != " + rows + ").");
		if (columns != y.length)
			throw new SizeException("Vector y has wrong length (" + x.length + " != " + columns + ").");			
		
		SimpleBlas.gerc(alpha, x, y, this);
		return this;
	}

	public ComplexFloatMatrix rankOneUpdate(float alpha, ComplexFloatMatrix x, ComplexFloatMatrix y) {
		return rankOneUpdate(new ComplexFloat(alpha), x, y);
	}

	/** Computes a rank-1-update A = A + alpha * x * x'. */ 
	public ComplexFloatMatrix rankOneUpdate(float alpha, ComplexFloatMatrix x) {
		return rankOneUpdate(new ComplexFloat(alpha), x, x);
	}

	/** Computes a rank-1-update A = A + alpha * x * x'. */ 
	public ComplexFloatMatrix rankOneUpdate(ComplexFloat alpha, ComplexFloatMatrix x) {
		return rankOneUpdate(alpha, x, x);
	}

	/** Computes a rank-1-update A = A + x * x'. */ 
	public ComplexFloatMatrix rankOneUpdate(ComplexFloatMatrix x) {
		return rankOneUpdate(1.0f, x, x);
	}

	/** Computes a rank-1-update A = A + x * y'. */ 
	public ComplexFloatMatrix rankOneUpdate(ComplexFloatMatrix x, ComplexFloatMatrix y) {
		return rankOneUpdate(1.0f, x, y);
	}

	/****************************************************************
	 * Logical operations
	 */
	
	public ComplexFloat sum() {
		ComplexFloat s = new ComplexFloat(0.0f);
                ComplexFloat c = new ComplexFloat(0.0f);
		for (int i = 0; i < length; i++)
			s.addi(get(i, c));
		return s;
	}
	
	public ComplexFloat mean() {
		return sum().div((float)length);
	}
	
	/** Computes this^T * other */
	public ComplexFloat dotc(ComplexFloatMatrix other) {
		return SimpleBlas.dotc(this, other);
	}
	
	/** Computes this^H * other */
	public ComplexFloat dotu(ComplexFloatMatrix other) {
		return SimpleBlas.dotu(this, other);
	}

	public float norm2() {
		return SimpleBlas.nrm2(this);
	}
	
	public float normmax() {
		int i = SimpleBlas.iamax(this);
		return get(i).abs();
	}

	public float norm1() {
		return SimpleBlas.asum(this);
	}
		
	/** Return a vector containing the sums of the columns (having number of columns many entries) */
	public ComplexFloatMatrix columnSums() {
		ComplexFloatMatrix v =
                        new ComplexFloatMatrix(1, columns);

		for (int c = 0; c < columns; c++)
			v.put(c, getColumn(c).sum());

		return v;
	}

	public ComplexFloatMatrix columnMeans() {
		return columnSums().divi(rows);
	}
	
	public ComplexFloatMatrix rowSums() {
		ComplexFloatMatrix v = new ComplexFloatMatrix(rows);

		for (int r = 0; r < rows; r++)
			v.put(r, getRow(r).sum());

		return v;
	}

	public ComplexFloatMatrix rowMeans() {
		return rowSums().divi(columns);
	}

	public ComplexFloatMatrix getColumn(int c) {
		ComplexFloatMatrix result = new ComplexFloatMatrix(rows, 1);
		NativeBlas.ccopy(rows, data, index(0, c), 1, result.data, 0, 1);
		return result;
	}
	
	public void putColumn(int c, ComplexFloatMatrix v) {
		NativeBlas.ccopy(rows, v.data, 0, 1, data, index(0, c), 1);
	}

	public ComplexFloatMatrix getRow(int r) {
		ComplexFloatMatrix result = new ComplexFloatMatrix(1, columns);
		NativeBlas.ccopy(columns, data, index(r, 0), rows, result.data, 0, 1);
		return result;
	}
	
	public void putRow(int r, ComplexFloatMatrix v) {
		NativeBlas.ccopy(columns, v.data, 0, 1, data, index(r, 0), rows);
	}

	/**************************************************************************
	 * Elementwise Functions
	 */

	/** Add a row vector to all rows of the matrix */
	public void addRowVector(ComplexFloatMatrix x) {
		for (int r = 0; r < rows; r++) {
			NativeBlas.caxpy(columns, ComplexFloat.UNIT, x.data, 0, 1, data, index(r, 0), rows);
		}
	}

	/** Add a vector to all columns of the matrix */
	public void addColumnVector(ComplexFloatMatrix x) {
		for (int c = 0; c < columns; c++) {
			NativeBlas.caxpy(rows, ComplexFloat.UNIT, x.data, 0, 1, data, index(0, c), 1);
		}
	}

       	/** Add a row vector to all rows of the matrix */
	public void subRowVector(ComplexFloatMatrix x) {
		for (int r = 0; r < rows; r++) {
			NativeBlas.caxpy(columns, ComplexFloat.NEG_UNIT, x.data, 0, 1, data, index(r, 0), rows);
		}
	}

	/** Add a vector to all columns of the matrix */
	public void subColumnVector(ComplexFloatMatrix x) {
		for (int c = 0; c < columns; c++) {
			NativeBlas.caxpy(rows, ComplexFloat.NEG_UNIT, x.data, 0, 1, data, index(0, c), 1);
		}
	}

	/**
	 * Writes out this matrix to the given data stream.
	 * @param dos the data output stream to write to.
	 * @throws IOException 
	 */
	public void out(DataOutputStream dos) throws IOException {
		dos.writeUTF("float");
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
		if(!dis.readUTF().equals("float")) 
			throw new IllegalStateException("The matrix in the specified file is not of the correct type!");
		
		this.columns	= dis.readInt();
		this.rows		= dis.readInt();

		final int MAX = dis.readInt();
		data = new float[MAX];
		for(int i=0; i < MAX;i++)
			data[i] = dis.readFloat();
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
	public ComplexFloatMatrix #{base}i(ComplexFloatMatrix other) {
		return #{base}i(other, this);
	}
	 	
	public ComplexFloatMatrix #{base}(ComplexFloatMatrix other) {
	  	return #{base}i(other, new ComplexFloatMatrix(#{result_rows}, #{result_cols}));
	}

	public ComplexFloatMatrix #{base}i(ComplexFloat v) {
		return #{base}i(v, this);
	}
	
	public ComplexFloatMatrix #{base}i(float v) {
		return #{base}i(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix #{base}(ComplexFloat v) {
		return #{base}i(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix #{base}(float v) {
		return #{base}i(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	
	 	EOS
	  end
	#*/

	/* Generating code for logical operators. This not only generates the stubs 
	 * but really all of the code.
	 */
	
	/*#
	 def gen_compare(name, op); <<-EOS
	 public ComplexFloatMatrix #{name}i(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	    if (other.isScalar())
	       return #{name}i(other.scalar(), result);
	       
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat c1 = new ComplexFloat(0.0f);
                ComplexFloat c2 = new ComplexFloat(0.0f);
          
                for (int i = 0; i < length; i++)
                    result.put(i, get(i, c1).#{op}(other.get(i, c2)) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix #{name}i(ComplexFloatMatrix other) {
	   return #{name}i(other, this);
	 }
	 
	 public ComplexFloatMatrix #{name}(ComplexFloatMatrix other) {
	   return #{name}i(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix #{name}i(ComplexFloat value, ComplexFloatMatrix result) {
	   ensureResultLength(null, result);
           ComplexFloat c = new ComplexFloat(0.0f);
	   for (int i = 0; i < length; i++)
	     result.put(i, get(i, c).#{op}(value) ? 1.0f : 0.0f);
	   return result;
	 }

	 public ComplexFloatMatrix #{name}i(float value, ComplexFloatMatrix result) {
	   return #{name}i(new ComplexFloat(value), result);
	 }

	 public ComplexFloatMatrix #{name}i(ComplexFloat value) {
	   return #{name}i(value, this);
	 }
	 
	 public ComplexFloatMatrix #{name}i(float value) {
	   return #{name}i(new ComplexFloat(value));
	 }
	 
	 public ComplexFloatMatrix #{name}(ComplexFloat value) {
	   return #{name}i(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix #{name}(float value) {
	   return #{name}i(new ComplexFloat(value));
	 }

	 EOS
	 end
	 #*/
	
	/*#
	 def gen_logical(name, op); <<-EOS
	 public ComplexFloatMatrix #{name}i(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat t1 = new ComplexFloat(0.0f);
                ComplexFloat t2 = new ComplexFloat(0.0f);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) #{op} (!other.get(i, t2).isZero()) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix #{name}i(ComplexFloatMatrix other) {
	   return #{name}i(other, this);
	 }
	 
	 public ComplexFloatMatrix #{name}(ComplexFloatMatrix other) {
	   return #{name}i(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix #{name}i(ComplexFloat value, ComplexFloatMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexFloat t = new ComplexFloat(0.0f);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() #{op} val ? 1.0f : 0.0f);
	   return result;
	 }

 	 public ComplexFloatMatrix #{name}i(float value, ComplexFloatMatrix result) {
 	   return #{name}i(new ComplexFloat(value), result);
 	 }

	 public ComplexFloatMatrix #{name}i(ComplexFloat value) {
	   return #{name}i(value, this);
	 }

 	 public ComplexFloatMatrix #{name}i(float value) {
 	   return #{name}i(new ComplexFloat(value), this);
 	 }

	 public ComplexFloatMatrix #{name}(ComplexFloat value) {
	   return #{name}i(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix #{name}(float value) {
	   return #{name}i(new ComplexFloat(value));
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
	public ComplexFloatMatrix addi(ComplexFloatMatrix other) {
		return addi(other, this);
	}
	 	
	public ComplexFloatMatrix add(ComplexFloatMatrix other) {
	  	return addi(other, new ComplexFloatMatrix(rows, columns));
	}

	public ComplexFloatMatrix addi(ComplexFloat v) {
		return addi(v, this);
	}
	
	public ComplexFloatMatrix addi(float v) {
		return addi(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix add(ComplexFloat v) {
		return addi(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix add(float v) {
		return addi(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	public ComplexFloatMatrix subi(ComplexFloatMatrix other) {
		return subi(other, this);
	}
	 	
	public ComplexFloatMatrix sub(ComplexFloatMatrix other) {
	  	return subi(other, new ComplexFloatMatrix(rows, columns));
	}

	public ComplexFloatMatrix subi(ComplexFloat v) {
		return subi(v, this);
	}
	
	public ComplexFloatMatrix subi(float v) {
		return subi(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix sub(ComplexFloat v) {
		return subi(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix sub(float v) {
		return subi(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	public ComplexFloatMatrix rsubi(ComplexFloatMatrix other) {
		return rsubi(other, this);
	}
	 	
	public ComplexFloatMatrix rsub(ComplexFloatMatrix other) {
	  	return rsubi(other, new ComplexFloatMatrix(rows, columns));
	}

	public ComplexFloatMatrix rsubi(ComplexFloat v) {
		return rsubi(v, this);
	}
	
	public ComplexFloatMatrix rsubi(float v) {
		return rsubi(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix rsub(ComplexFloat v) {
		return rsubi(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix rsub(float v) {
		return rsubi(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	public ComplexFloatMatrix divi(ComplexFloatMatrix other) {
		return divi(other, this);
	}
	 	
	public ComplexFloatMatrix div(ComplexFloatMatrix other) {
	  	return divi(other, new ComplexFloatMatrix(rows, columns));
	}

	public ComplexFloatMatrix divi(ComplexFloat v) {
		return divi(v, this);
	}
	
	public ComplexFloatMatrix divi(float v) {
		return divi(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix div(ComplexFloat v) {
		return divi(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix div(float v) {
		return divi(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	public ComplexFloatMatrix rdivi(ComplexFloatMatrix other) {
		return rdivi(other, this);
	}
	 	
	public ComplexFloatMatrix rdiv(ComplexFloatMatrix other) {
	  	return rdivi(other, new ComplexFloatMatrix(rows, columns));
	}

	public ComplexFloatMatrix rdivi(ComplexFloat v) {
		return rdivi(v, this);
	}
	
	public ComplexFloatMatrix rdivi(float v) {
		return rdivi(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix rdiv(ComplexFloat v) {
		return rdivi(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix rdiv(float v) {
		return rdivi(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	public ComplexFloatMatrix muli(ComplexFloatMatrix other) {
		return muli(other, this);
	}
	 	
	public ComplexFloatMatrix mul(ComplexFloatMatrix other) {
	  	return muli(other, new ComplexFloatMatrix(rows, columns));
	}

	public ComplexFloatMatrix muli(ComplexFloat v) {
		return muli(v, this);
	}
	
	public ComplexFloatMatrix muli(float v) {
		return muli(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix mul(ComplexFloat v) {
		return muli(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix mul(float v) {
		return muli(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	public ComplexFloatMatrix mmuli(ComplexFloatMatrix other) {
		return mmuli(other, this);
	}
	 	
	public ComplexFloatMatrix mmul(ComplexFloatMatrix other) {
	  	return mmuli(other, new ComplexFloatMatrix(rows, other.columns));
	}

	public ComplexFloatMatrix mmuli(ComplexFloat v) {
		return mmuli(v, this);
	}
	
	public ComplexFloatMatrix mmuli(float v) {
		return mmuli(new ComplexFloat(v), this);
	}

	public ComplexFloatMatrix mmul(ComplexFloat v) {
		return mmuli(v, new ComplexFloatMatrix(rows, columns));
	} 	

	public ComplexFloatMatrix mmul(float v) {
		return mmuli(new ComplexFloat(v), new ComplexFloatMatrix(rows, columns));
	} 	
	 	

	 public ComplexFloatMatrix eqi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	    if (other.isScalar())
	       return eqi(other.scalar(), result);
	       
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat c1 = new ComplexFloat(0.0f);
                ComplexFloat c2 = new ComplexFloat(0.0f);
          
                for (int i = 0; i < length; i++)
                    result.put(i, get(i, c1).eq(other.get(i, c2)) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix eqi(ComplexFloatMatrix other) {
	   return eqi(other, this);
	 }
	 
	 public ComplexFloatMatrix eq(ComplexFloatMatrix other) {
	   return eqi(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix eqi(ComplexFloat value, ComplexFloatMatrix result) {
	   ensureResultLength(null, result);
           ComplexFloat c = new ComplexFloat(0.0f);
	   for (int i = 0; i < length; i++)
	     result.put(i, get(i, c).eq(value) ? 1.0f : 0.0f);
	   return result;
	 }

	 public ComplexFloatMatrix eqi(float value, ComplexFloatMatrix result) {
	   return eqi(new ComplexFloat(value), result);
	 }

	 public ComplexFloatMatrix eqi(ComplexFloat value) {
	   return eqi(value, this);
	 }
	 
	 public ComplexFloatMatrix eqi(float value) {
	   return eqi(new ComplexFloat(value));
	 }
	 
	 public ComplexFloatMatrix eq(ComplexFloat value) {
	   return eqi(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix eq(float value) {
	   return eqi(new ComplexFloat(value));
	 }


	 public ComplexFloatMatrix nei(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	    if (other.isScalar())
	       return nei(other.scalar(), result);
	       
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat c1 = new ComplexFloat(0.0f);
                ComplexFloat c2 = new ComplexFloat(0.0f);
          
                for (int i = 0; i < length; i++)
                    result.put(i, get(i, c1).eq(other.get(i, c2)) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix nei(ComplexFloatMatrix other) {
	   return nei(other, this);
	 }
	 
	 public ComplexFloatMatrix ne(ComplexFloatMatrix other) {
	   return nei(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix nei(ComplexFloat value, ComplexFloatMatrix result) {
	   ensureResultLength(null, result);
           ComplexFloat c = new ComplexFloat(0.0f);
	   for (int i = 0; i < length; i++)
	     result.put(i, get(i, c).eq(value) ? 1.0f : 0.0f);
	   return result;
	 }

	 public ComplexFloatMatrix nei(float value, ComplexFloatMatrix result) {
	   return nei(new ComplexFloat(value), result);
	 }

	 public ComplexFloatMatrix nei(ComplexFloat value) {
	   return nei(value, this);
	 }
	 
	 public ComplexFloatMatrix nei(float value) {
	   return nei(new ComplexFloat(value));
	 }
	 
	 public ComplexFloatMatrix ne(ComplexFloat value) {
	   return nei(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix ne(float value) {
	   return nei(new ComplexFloat(value));
	 }


	 public ComplexFloatMatrix andi(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat t1 = new ComplexFloat(0.0f);
                ComplexFloat t2 = new ComplexFloat(0.0f);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) & (!other.get(i, t2).isZero()) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix andi(ComplexFloatMatrix other) {
	   return andi(other, this);
	 }
	 
	 public ComplexFloatMatrix and(ComplexFloatMatrix other) {
	   return andi(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix andi(ComplexFloat value, ComplexFloatMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexFloat t = new ComplexFloat(0.0f);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() & val ? 1.0f : 0.0f);
	   return result;
	 }

 	 public ComplexFloatMatrix andi(float value, ComplexFloatMatrix result) {
 	   return andi(new ComplexFloat(value), result);
 	 }

	 public ComplexFloatMatrix andi(ComplexFloat value) {
	   return andi(value, this);
	 }

 	 public ComplexFloatMatrix andi(float value) {
 	   return andi(new ComplexFloat(value), this);
 	 }

	 public ComplexFloatMatrix and(ComplexFloat value) {
	   return andi(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix and(float value) {
	   return andi(new ComplexFloat(value));
	 }

	 public ComplexFloatMatrix ori(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat t1 = new ComplexFloat(0.0f);
                ComplexFloat t2 = new ComplexFloat(0.0f);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) | (!other.get(i, t2).isZero()) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix ori(ComplexFloatMatrix other) {
	   return ori(other, this);
	 }
	 
	 public ComplexFloatMatrix or(ComplexFloatMatrix other) {
	   return ori(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix ori(ComplexFloat value, ComplexFloatMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexFloat t = new ComplexFloat(0.0f);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() | val ? 1.0f : 0.0f);
	   return result;
	 }

 	 public ComplexFloatMatrix ori(float value, ComplexFloatMatrix result) {
 	   return ori(new ComplexFloat(value), result);
 	 }

	 public ComplexFloatMatrix ori(ComplexFloat value) {
	   return ori(value, this);
	 }

 	 public ComplexFloatMatrix ori(float value) {
 	   return ori(new ComplexFloat(value), this);
 	 }

	 public ComplexFloatMatrix or(ComplexFloat value) {
	   return ori(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix or(float value) {
	   return ori(new ComplexFloat(value));
	 }

	 public ComplexFloatMatrix xori(ComplexFloatMatrix other, ComplexFloatMatrix result) {
	 	assertSameLength(other);
	 	ensureResultLength(other, result);
	 	
                ComplexFloat t1 = new ComplexFloat(0.0f);
                ComplexFloat t2 = new ComplexFloat(0.0f);
         
               for (int i = 0; i < length; i++)
                  result.put(i, (!get(i, t1).isZero()) ^ (!other.get(i, t2).isZero()) ? 1.0f : 0.0f);
	   return result;
	 }
	 
	 public ComplexFloatMatrix xori(ComplexFloatMatrix other) {
	   return xori(other, this);
	 }
	 
	 public ComplexFloatMatrix xor(ComplexFloatMatrix other) {
	   return xori(other, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix xori(ComplexFloat value, ComplexFloatMatrix result) {
	 	ensureResultLength(null, result);
	 	boolean val = !value.isZero();
                ComplexFloat t = new ComplexFloat(0.0f);
                for (int i = 0; i < length; i++)
                     result.put(i, !get(i, t).isZero() ^ val ? 1.0f : 0.0f);
	   return result;
	 }

 	 public ComplexFloatMatrix xori(float value, ComplexFloatMatrix result) {
 	   return xori(new ComplexFloat(value), result);
 	 }

	 public ComplexFloatMatrix xori(ComplexFloat value) {
	   return xori(value, this);
	 }

 	 public ComplexFloatMatrix xori(float value) {
 	   return xori(new ComplexFloat(value), this);
 	 }

	 public ComplexFloatMatrix xor(ComplexFloat value) {
	   return xori(value, new ComplexFloatMatrix(rows, columns));
	 }
	 
	 public ComplexFloatMatrix xor(float value) {
	   return xori(new ComplexFloat(value));
	 }
//RJPP-END--------------------------------------------------------------
}