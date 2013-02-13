// --- BEGIN LICENSE BLOCK ---
/* 
 * Copyright (c) 2009, Mikio L. Braun
 * Copyright (c) 2008, Johannes Schaback
 * Copyright (c) 2009, Jan Saputra Müller
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
import org.jblas.ranges.Range;
import org.jblas.util.Random;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;

import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.PrintWriter;
import java.io.Serializable;
import java.io.StringWriter;
import java.util.AbstractList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

/**
 * A general matrix class for <tt>float</tt> typed values.
 * 
 * Don't be intimidated by the large number of methods this function defines. Most
 * are overloads provided for ease of use. For example, for each arithmetic operation,
 * up to six overloaded versions exist to handle in-place computations, and
 * scalar arguments.
 * 
 * <h3>Construction</h3>
 * 
 * <p>To construct a two-dimensional matrices, you can use the following constructors
 * and static methods.</p>
 * 
 * <table class="my">
 * <tr><th>Method<th>Description
 * <tr><td>FloatMatrix(m,n, [value1, value2, value3...])<td>Values are filled in row by row.
 * <tr><td>FloatMatrix(new float[][] {{value1, value2, ...}, ...}<td>Inner arrays are columns.
 * <tr><td>FloatMatrix.zeros(m,n) <td>Initial values set to 0.0f.
 * <tr><td>FloatMatrix.ones(m,n) <td>Initial values set to 1.0f.
 * <tr><td>FloatMatrix.rand(m,n) <td>Values drawn at random between 0.0f and 1.0f.
 * <tr><td>FloatMatrix.randn(m,n) <td>Values drawn from normal distribution.
 * <tr><td>FloatMatrix.eye(n) <td>Unit matrix (values 0.0f except for 1.0f on the diagonal).
 * <tr><td>FloatMatrix.diag(array) <td>Diagonal matrix with given diagonal elements.
 * </table>
 * 
 * <p>Alternatively, you can construct (column) vectors, if you just supply the length
 * using the following constructors and static methods.</p>
 * 
 * <table class="my">
 * <tr><th>Method<th>Description
 * <tr><td>FloatMatrix(m)<td>Constructs a column vector.
 * <tr><td>FloatMatrix(new float[] {value1, value2, ...})<td>Constructs a column vector.
 * <tr><td>FloatMatrix.zeros(m) <td>Initial values set to 0.0f.
 * <tr><td>FloatMatrix.ones(m) <td>Initial values set to 1.0f.
 * <tr><td>FloatMatrix.rand(m) <td>Values drawn at random between 0.0f and 1.0f.
 * <tr><td>FloatMatrix.randn(m) <td>Values drawn from normal distribution.
 * </table>
 * 
 * <p>You can also construct new matrices by concatenating matrices either horziontally
 * or vertically:</p>
 * 
 * <table class="my">
 * <tr><th>Method<th>Description
 * <tr><td>x.concatHorizontally(y)<td>New matrix will be x next to y.
 * <tr><td>x.concatVertically(y)<td>New matrix will be x atop y.
 * </table>
 * 
 * <h3>Element Access, Copying and Duplication</h3>
 * 
 * <p>To access individual elements, or whole rows and columns, use the following
 * methods:<p>
 * 
 * <table class="my">
 * <tr><th>x.Method<th>Description
 * <tr><td>x.get(i,j)<td>Get element in row i and column j.
 * <tr><td>x.put(i, j, v)<td>Set element in row i and column j to value v
 * <tr><td>x.get(i)<td>Get the ith element of the matrix (traversing rows first).
 * <tr><td>x.put(i, v)<td>Set the ith element of the matrix (traversing rows first).
 * <tr><td>x.getColumn(i)<td>Get a copy of column i.
 * <tr><td>x.putColumn(i, c)<td>Put matrix c into column i.
 * <tr><td>x.getRow(i)<td>Get a copy of row i.
 * <tr><td>x.putRow(i, c)<td>Put matrix c into row i.
 * <tr><td>x.swapColumns(i, j)<td>Swap the contents of columns i and j.
 * <tr><td>x.swapRows(i, j)<td>Swap the contents of columns i and j.
 * </table>
 * 
 * <p>For <tt>get</tt> and <tt>put</tt>, you can also pass integer arrays,
 * FloatMatrix objects, or Range objects, which then specify the indices used 
 * as follows:
 * 
 * <ul>
 * <li><em>integer array:</em> the elements will be used as indices.
 * <li><em>FloatMatrix object:</em> non-zero entries specify the indices.
 * <li><em>Range object:</em> see below.
 * </ul>
 * 
 * <p>When using <tt>put</tt> with multiple indices, the assigned object must
 * have the correct size or be a scalar.</p>
 *
 * <p>There exist the following Range objects. The Class <tt>RangeUtils</tt> also
 * contains the a number of handy helper methods for constructing these ranges.</p>
 * <table class="my">
 * <tr><th>Class <th>RangeUtils method <th>Indices
 * <tr><td>AllRange <td>all() <td>All legal indices.
 * <tr><td>PointRange <td>point(i) <td> A single point.
 * <tr><td>IntervalRange <td>interval(a, b)<td> All indices from a to b (inclusive)
 * <tr><td rowspan=3>IndicesRange <td>indices(int[])<td> The specified indices.
 * <tr><td>indices(FloatMatrix)<td>The specified indices.
 * <tr><td>find(FloatMatrix)<td>The non-zero entries of the matrix.
 * </table>
 * 
 * <p>The following methods can be used for duplicating and copying matrices.</p>
 * 
 * <table class="my">
 * <tr><th>Method<th>Description
 * <tr><td>x.dup()<td>Get a copy of x.
 * <tr><td>x.copy(y)<td>Copy the contents of y to x (possible resizing x).
 * </table>
 *    
 * <h3>Size and Shape</h3>
 * 
 * <p>The following methods permit to acces the size of a matrix and change its size or shape.</p>
 * 
 * <table class="my">
 * <tr><th>x.Method<th>Description
 * <tr><td>x.rows<td>Number of rows.
 * <tr><td>x.columns<td>Number of columns.
 * <tr><td>x.length<td>Total number of elements.
 * <tr><td>x.isEmpty()<td>Checks whether rows == 0 and columns == 0.
 * <tr><td>x.isRowVector()<td>Checks whether rows == 1.
 * <tr><td>x.isColumnVector()<td>Checks whether columns == 1.
 * <tr><td>x.isVector()<td>Checks whether rows == 1 or columns == 1.
 * <tr><td>x.isSquare()<td>Checks whether rows == columns.
 * <tr><td>x.isScalar()<td>Checks whether length == 1.
 * <tr><td>x.resize(r, c)<td>Resize the matrix to r rows and c columns, discarding the content.
 * <tr><td>x.reshape(r, c)<td>Resize the matrix to r rows and c columns.<br> Number of elements must not change.
 * </table>
 * 
 * <p>The size is stored in the <tt>rows</tt> and <tt>columns</tt> member variables.
 * The total number of elements is stored in <tt>length</tt>. Do not change these
 * values unless you know what you're doing!</p>
 * 
 * <h3>Arithmetics</h3>
 * 
 * <p>The usual arithmetic operations are implemented. Each operation exists in a
 * in-place version, recognizable by the suffix <tt>"i"</tt>, to which you can supply
 * the result matrix (or <tt>this</tt> is used, if missing). Using in-place operations
 * can also lead to a smaller memory footprint, as the number of temporary objects
 * which are directly garbage collected again is reduced.</p>
 * 
 * <p>Whenever you specify a result vector, the result vector must already have the
 * correct dimensions.</p>
 * 
 * <p>For example, you can add two matrices using the <tt>add</tt> method. If you want
 * to store the result in of <tt>x + y</tt> in <tt>z</tt>, type
 * <span class=code>
 * x.addi(y, z)   // computes x = y + z.
 * </span>
 * Even in-place methods return the result, such that you can easily chain in-place methods,
 * for example:
 * <span class=code>
 * x.addi(y).addi(z) // computes x += y; x += z
 * </span></p> 
 *
 * <p>Methods which operate element-wise only make sure that the length of the matrices
 * is correct. Therefore, you can add a 3 * 3 matrix to a 1 * 9 matrix, for example.</p>
 * 
 * <p>Finally, there exist versions which take floats instead of FloatMatrix Objects
 * as arguments. These then compute the operation with the same value as the
 * right-hand-side. The same effect can be achieved by passing a FloatMatrix with
 * exactly one element.</p>
 * 
 * <table class="my">
 * <tr><th>Operation <th>Method <th>Comment
 * <tr><td>x + y <td>x.add(y)			<td>
 * <tr><td>x - y <td>x.sub(y), y.rsub(x) <td>rsub subtracts left from right hand side
 * <tr><td rowspan=3>x * y 	<td>x.mul(y) <td>element-wise multiplication 
 * <tr>						<td>x.mmul(y)<td>matrix-matrix multiplication
 * <tr>						<td>x.dot(y) <td>scalar-product
 * <tr><td>x / y <td>x.div(y), y.rdiv(x) <td>rdiv divides right hand side by left hand side.
 * <tr><td>- x	<td>x.neg()				<td>
 * </table>
 * 
 * <p>There also exist operations which work on whole columns or rows.</p>
 * 
 * <table class="my">
 * <tr><th>Method <th>Description
 * <tr><td>x.addRowVector<td>adds a vector to each row (addiRowVector works in-place)
 * <tr><td>x.addColumnVector<td>adds a vector to each column
 * <tr><td>x.subRowVector<td>subtracts a vector from each row
 * <tr><td>x.subColumnVector<td>subtracts a vector from each column
 * <tr><td>x.mulRow<td>Multiplies a row by a scalar
 * <tr><td>x.mulColumn<td>multiplies a row by a column
 * </table>
 * 
 * <p>In principle, you could achieve the same result by first calling getColumn(), 
 * adding, and then calling putColumn, but these methods are much faster.</p>
 * 
 * <p>The following comparison operations are available</p>
 *  
 * <table class="my">
 * <tr><th>Operation <th>Method
 * <tr><td>x &lt; y		<td>x.lt(y)
 * <tr><td>x &lt;= y	<td>x.le(y)
 * <tr><td>x &gt; y		<td>x.gt(y)
 * <tr><td>x &gt;= y	<td>x.ge(y)
 * <tr><td>x == y		<td>x.eq(y)
 * <tr><td>x != y		<td>x.ne(y)
 * </table>
 *
 * <p> Logical operations are also supported. For these operations, a value different from
 * zero is treated as "true" and zero is treated as "false". All operations are carried
 * out elementwise.</p>
 * 
 * <table class="my">
 * <tr><th>Operation <th>Method
 * <tr><td>x & y 	<td>x.and(y)
 * <tr><td>x | y 	<td>x.or(y)
 * <tr><td>x ^ y	<td>x.xor(y)
 * <tr><td>! x		<td>x.not()
 * </table>
 * 
 * <p>Finally, there are a few more methods to compute various things:</p>
 * 
 * <table class="my">
 * <tr><th>Method <th>Description
 * <tr><td>x.max() <td>Return maximal element
 * <tr><td>x.argmax() <td>Return index of largest element
 * <tr><td>x.min() <td>Return minimal element
 * <tr><td>x.argmin() <td>Return index of largest element
 * <tr><td>x.columnMins() <td>Return column-wise minima
 * <tr><td>x.columnArgmins() <td>Return column-wise index of minima
 * <tr><td>x.columnMaxs() <td>Return column-wise maxima
 * <tr><td>x.columnArgmaxs() <td>Return column-wise index of maxima
 * </table>
 * 
 * @author Mikio Braun, Johannes Schaback
 */
public class FloatMatrix implements Serializable {

    /** Number of rows. */
    public int rows;
    /** Number of columns. */
    public int columns;
    /** Total number of elements (for convenience). */
    public int length;
    /** The actual data stored by rows (that is, row 0, row 1...). */
    public float[] data = null; // rows are contiguous
    public static final FloatMatrix EMPTY = new FloatMatrix();
    static final long serialVersionUID = -1249281332731183060L;

    /**************************************************************************
     *
     * Constructors and factory functions
     *
     **************************************************************************/
    /** Create a new matrix with <i>newRows</i> rows, <i>newColumns</i> columns
     * using <i>newData></i> as the data. The length of the data is not checked!
     */
    public FloatMatrix(int newRows, int newColumns, float... newData) {
        rows = newRows;
        columns = newColumns;
        length = rows * columns;

        if (newData != null && newData.length != newRows * newColumns) {
            throw new IllegalArgumentException(
                    "Passed data must match matrix dimensions.");
        }

        data = newData;
        //System.err.printf("%d * %d matrix created\n", rows, columns);
    }

    /**
     * Creates a new <i>n</i> times <i>m</i> <tt>FloatMatrix</tt>.
     * @param newRows the number of rows (<i>n</i>) of the new matrix.
     * @param newColumns the number of columns (<i>m</i>) of the new matrix.
     */
    public FloatMatrix(int newRows, int newColumns) {
        this(newRows, newColumns, new float[newRows * newColumns]);
    }

    /**
     * Creates a new <tt>FloatMatrix</tt> of size 0 times 0.
     */
    public FloatMatrix() {
        this(0, 0, (float[]) null);
    }

    /**
     * Create a Matrix of length <tt>len</tt>. By default, this creates a row vector.
     * @param len
     */
    public FloatMatrix(int len) {
        this(len, 1, new float[len]);
    }

    public FloatMatrix(float[] newData) {
        this(newData.length);
        data = newData;
    }

    /**
     * Creates a new matrix by reading it from a file.
     * @param filename the path and name of the file to read the matrix from
     * @throws IOException
     */
    public FloatMatrix(String filename) throws IOException {
        load(filename);
    }

    /**
     * Creates a new <i>n</i> times <i>m</i> <tt>FloatMatrix</tt> from
     * the given <i>n</i> times <i>m</i> 2D data array. The first dimension of the array makes the
     * rows (<i>n</i>) and the second dimension the columns (<i>m</i>). For example, the
     * given code <br/><br/>
     * <code>new FloatMatrix(new float[][]{{1d, 2d, 3d}, {4d, 5d, 6d}, {7d, 8d, 9d}}).print();</code><br/><br/>
     * will constructs the following matrix:
     * <pre>
     * 1.0f	2.0f	3.0f
     * 4.0f	5.0f	6.0f
     * 7.0f	8.0f	9.0f
     * </pre>.
     * @param data <i>n</i> times <i>m</i> data array
     */
    public FloatMatrix(float[][] data) {
        this(data.length, data[0].length);

        for (int r = 0; r < rows; r++) {
            assert (data[r].length == columns);
        }

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                put(r, c, data[r][c]);
            }
        }
    }

    public FloatMatrix(List<Float> data) {
        this(data.size());

        int c = 0;
        for (java.lang.Float d : data) {
            put(c++, d);
        }
    }

    /**
     * Construct FloatMatrix from ASCII representation.
     *
     * This is not very fast, but can be quiet useful when
     * you want to "just" construct a matrix, for example
     * when testing.
     *
     * The format is semicolon separated rows of space separated values,
     * for example "1 2 3; 4 5 6; 7 8 9".
     */
    public static FloatMatrix valueOf(String text) {
        String[] rowValues = text.split(";");

        // process first line
        String[] columnValues = rowValues[0].trim().split("\\s+");

        FloatMatrix result = null;

        // process rest
        for (int r = 0; r < rowValues.length; r++) {
            columnValues = rowValues[r].trim().split("\\s+");

            if (r == 0) {
                result = new FloatMatrix(rowValues.length, columnValues.length);
            }

            for (int c = 0; c < columnValues.length; c++) {
                result.put(r, c, Float.valueOf(columnValues[c]));
            }
        }

        return result;
    }

    /**
     * Serialization
     */
    private void writeObject(ObjectOutputStream s) throws IOException {
        s.defaultWriteObject();
    }

    private void readObject(ObjectInputStream s) throws IOException, ClassNotFoundException {
        s.defaultReadObject();
    }

    /** Create matrix with random values uniformly in 0..1. */
    public static FloatMatrix rand(int rows, int columns) {
        FloatMatrix m = new FloatMatrix(rows, columns);

        for (int i = 0; i < rows * columns; i++) {
            m.data[i] = (float) Random.nextFloat();
        }

        return m;
    }

    /** Creates a row vector with random values uniformly in 0..1. */
    public static FloatMatrix rand(int len) {
        return rand(len, 1);
    }

    /** Create matrix with normally distributed random values. */
    public static FloatMatrix randn(int rows, int columns) {
        FloatMatrix m = new FloatMatrix(rows, columns);

        for (int i = 0; i < rows * columns; i++) {
            m.data[i] = (float) Random.nextGaussian();
        }

        return m;
    }

    /** Create row vector with normally distributed random values. */
    public static FloatMatrix randn(int len) {
        return randn(len, 1);
    }

    /** Creates a new matrix in which all values are equal 0. */
    public static FloatMatrix zeros(int rows, int columns) {
        return new FloatMatrix(rows, columns);
    }

    /** Creates a row vector of given length. */
    public static FloatMatrix zeros(int length) {
        return zeros(length, 1);
    }

    /** Creates a new matrix in which all values are equal 1. */
    public static FloatMatrix ones(int rows, int columns) {
        FloatMatrix m = new FloatMatrix(rows, columns);

        for (int i = 0; i < rows * columns; i++) {
            m.put(i, 1.0f);
        }

        return m;
    }

    /** Creates a row vector with all elements equal to 1. */
    public static FloatMatrix ones(int length) {
        return ones(length, 1);
    }

    /** Construct a new n-by-n identity matrix. */
    public static FloatMatrix eye(int n) {
        FloatMatrix m = new FloatMatrix(n, n);

        for (int i = 0; i < n; i++) {
            m.put(i, i, 1.0f);
        }

        return m;
    }

    /**
     * Creates a new matrix where the values of the given vector are the diagonal values of
     * the matrix.
     */
    public static FloatMatrix diag(FloatMatrix x) {
        FloatMatrix m = new FloatMatrix(x.length, x.length);

        for (int i = 0; i < x.length; i++) {
            m.put(i, i, x.get(i));
        }

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
    public static FloatMatrix diag(FloatMatrix x, int rows, int columns) {
      FloatMatrix m = new FloatMatrix(rows, columns);

      for (int i = 0; i < x.length; i++) {
          m.put(i, i, x.get(i));
      }

      return m;
    }

    /**
     * Create a 1-by-1 matrix. For many operations, this matrix functions like a
     * normal float.
     */
    public static FloatMatrix scalar(float s) {
        FloatMatrix m = new FloatMatrix(1, 1);
        m.put(0, 0, s);
        return m;
    }

    /** Test whether a matrix is scalar. */
    public boolean isScalar() {
        return length == 1;
    }

    /** Return the first element of the matrix. */
    public float scalar() {
        return get(0);
    }

    public static FloatMatrix logspace(float lower, float upper, int size) {
        FloatMatrix result = new FloatMatrix(size);
        for (int i = 0; i < size; i++) {
            float t = (float) i / (size - 1);
            float e = lower * (1 - t) + t * upper;
            result.put(i, (float) Math.pow(10.0f, e));
        }
        return result;
    }

    public static FloatMatrix linspace(int lower, int upper, int size) {
        FloatMatrix result = new FloatMatrix(size);
        for (int i = 0; i < size; i++) {
            float t = (float) i / (size - 1);
            result.put(i, lower * (1 - t) + t * upper);
        }
        return result;
    }

    /**
     * Concatenates two matrices horizontally. Matrices must have identical
     * numbers of rows.
     */
    public static FloatMatrix concatHorizontally(FloatMatrix A, FloatMatrix B) {
        if (A.rows != B.rows) {
            throw new SizeException("Matrices don't have same number of rows.");
        }

        FloatMatrix result = new FloatMatrix(A.rows, A.columns + B.columns);
        SimpleBlas.copy(A, result);
        JavaBlas.rcopy(B.length, B.data, 0, 1, result.data, A.length, 1);
        return result;
    }

    /**
     * Concatenates two matrices vertically. Matrices must have identical
     * numbers of columns.
     */
    public static FloatMatrix concatVertically(FloatMatrix A, FloatMatrix B) {
        if (A.columns != B.columns) {
            throw new SizeException("Matrices don't have same number of columns (" + A.columns + " != " + B.columns + ".");
        }

        FloatMatrix result = new FloatMatrix(A.rows + B.rows, A.columns);

        for (int i = 0; i < A.columns; i++) {
            JavaBlas.rcopy(A.rows, A.data, A.index(0, i), 1, result.data, result.index(0, i), 1);
            JavaBlas.rcopy(B.rows, B.data, B.index(0, i), 1, result.data, result.index(A.rows, i), 1);
        }

        return result;
    }

    /**************************************************************************
     * Working with slices (Man! 30+ methods just to make this a bit flexible...)
     */
    /** Get all elements specified by the linear indices. */
    public FloatMatrix get(int[] indices) {
        FloatMatrix result = new FloatMatrix(indices.length);

        for (int i = 0; i < indices.length; i++) {
            result.put(i, get(indices[i]));
        }

        return result;
    }

    /** Get all elements for a given row and the specified columns. */
    public FloatMatrix get(int r, int[] indices) {
        FloatMatrix result = new FloatMatrix(1, indices.length);

        for (int i = 0; i < indices.length; i++) {
            result.put(i, get(r, indices[i]));
        }

        return result;
    }

    /** Get all elements for a given column and the specified rows. */
    public FloatMatrix get(int[] indices, int c) {
        FloatMatrix result = new FloatMatrix(indices.length, 1);

        for (int i = 0; i < indices.length; i++) {
            result.put(i, get(indices[i], c));
        }

        return result;
    }

    /** Get all elements from the specified rows and columns. */
    public FloatMatrix get(int[] rindices, int[] cindices) {
        FloatMatrix result = new FloatMatrix(rindices.length, cindices.length);

        for (int i = 0; i < rindices.length; i++) {
            for (int j = 0; j < cindices.length; j++) {
                result.put(i, j, get(rindices[i], cindices[j]));
            }
        }

        return result;
    }

    /** Get elements from specified rows and columns. */
    public FloatMatrix get(Range rs, Range cs) {
        rs.init(0, rows);
        cs.init(0, columns);
        FloatMatrix result = new FloatMatrix(rs.length(), cs.length());

        for (; rs.hasMore(); rs.next()) {
            cs.init(0, columns);
            for (; cs.hasMore(); cs.next()) {
                result.put(rs.index(), cs.index(), get(rs.value(), cs.value()));
            }
        }

        return result;
    }

    public FloatMatrix get(Range rs, int c) {
        rs.init(0, rows);
        FloatMatrix result = new FloatMatrix(rs.length(), 1);

        for (; rs.hasMore(); rs.next()) {
            result.put(rs.index(), 0, get(rs.value(), c));
        }

        return result;
    }

    public FloatMatrix get(int r, Range cs) {
        cs.init(0, columns);
        FloatMatrix result = new FloatMatrix(1, cs.length());

        for (; cs.hasMore(); cs.next()) {
            result.put(0, cs.index(), get(r, cs.value()));
        }

        return result;

    }

    /** Get elements specified by the non-zero entries of the passed matrix. */
    public FloatMatrix get(FloatMatrix indices) {
        return get(indices.findIndices());
    }

    /**
     * Get elements from a row and columns as specified by the non-zero entries of
     * a matrix.
     */
    public FloatMatrix get(int r, FloatMatrix indices) {
        return get(r, indices.findIndices());
    }

    /**
     * Get elements from a column and rows as specified by the non-zero entries of
     * a matrix.
     */
    public FloatMatrix get(FloatMatrix indices, int c) {
        return get(indices.findIndices(), c);
    }

    /**
     * Get elements from columns and rows as specified by the non-zero entries of
     * the passed matrices.
     */
    public FloatMatrix get(FloatMatrix rindices, FloatMatrix cindices) {
        return get(rindices.findIndices(), cindices.findIndices());
    }

    /** Return all elements with linear index a, a + 1, ..., b - 1.*/
    public FloatMatrix getRange(int a, int b) {
        FloatMatrix result = new FloatMatrix(b - a);

        for (int k = 0; k < b - a; k++) {
            result.put(k, get(a + k));
        }

        return result;
    }

    /** Get elements from a row and columns <tt>a</tt> to <tt>b</tt>. */
    public FloatMatrix getColumnRange(int r, int a, int b) {
        FloatMatrix result = new FloatMatrix(1, b - a);

        for (int k = 0; k < b - a; k++) {
            result.put(k, get(r, a + k));
        }

        return result;
    }

    /** Get elements from a column and rows <tt>a/tt> to <tt>b</tt>. */
    public FloatMatrix getRowRange(int a, int b, int c) {
        FloatMatrix result = new FloatMatrix(b - a);

        for (int k = 0; k < b - a; k++) {
            result.put(k, get(a + k, c));
        }

        return result;
    }

    /**
     * Get elements from rows <tt>ra</tt> to <tt>rb</tt> and
     * columns <tt>ca</tt> to <tt>cb</tt>.
     */
    public FloatMatrix getRange(int ra, int rb, int ca, int cb) {
        FloatMatrix result = new FloatMatrix(rb - ra, cb - ca);

        for (int i = 0; i < rb - ra; i++) {
            for (int j = 0; j < cb - ca; j++) {
                result.put(i, j, get(ra + i, ca + j));
            }
        }

        return result;
    }

    /** Get whole rows from the passed indices. */
    public FloatMatrix getRows(int[] rindices) {
        FloatMatrix result = new FloatMatrix(rindices.length, columns);
        for (int i = 0; i < rindices.length; i++) {
            JavaBlas.rcopy(columns, data, index(rindices[i], 0), rows, result.data, result.index(i, 0), result.rows);
        }
        return result;
    }

    /** Get whole rows as specified by the non-zero entries of a matrix. */
    public FloatMatrix getRows(FloatMatrix rindices) {
        return getRows(rindices.findIndices());
    }

    public FloatMatrix getRows(Range indices, FloatMatrix result) {
        indices.init(0, rows);
        if (result.rows < indices.length()) {
            throw new SizeException("Result matrix does not have enough rows (" + result.rows + " < " + indices.length() + ")");
        }
        result.checkColumns(columns);

        for (int c = 0; c < columns; c++) {
            indices.init(0, rows);
            for (int r = 0; indices.hasMore(); indices.next(), r++) {
                result.put(r, c, get(indices.index(), c));
            }
        }
        return result;
    }

    public FloatMatrix getRows(Range indices) {
        indices.init(0, rows);
        FloatMatrix result = new FloatMatrix(indices.length(), columns);
        return getRows(indices, result);
    }

    /** Get whole columns from the passed indices. */
    public FloatMatrix getColumns(int[] cindices) {
        FloatMatrix result = new FloatMatrix(rows, cindices.length);
        for (int i = 0; i < cindices.length; i++) {
            JavaBlas.rcopy(rows, data, index(0, cindices[i]), 1, result.data, result.index(0, i), 1);
        }
        return result;
    }

    /** Get whole columns as specified by the non-zero entries of a matrix. */
    public FloatMatrix getColumns(FloatMatrix cindices) {
        return getColumns(cindices.findIndices());
    }

    /**
     * Assert that the matrix has a certain length.
     * @throws SizeException
     */
    public void checkLength(int l) {
        if (length != l) {
            throw new SizeException("Matrix does not have the necessary length (" + length + " != " + l + ").");
        }
    }

    /**
     * Asserts that the matrix has a certain number of rows.
     * @throws SizeException
     */
    public void checkRows(int r) {
        if (rows != r) {
            throw new SizeException("Matrix does not have the necessary number of rows (" + rows + " != " + r + ").");
        }
    }

    /**
     * Asserts that the amtrix has a certain number of columns.
     * @throws SizeException
     */
    public void checkColumns(int c) {
        if (columns != c) {
            throw new SizeException("Matrix does not have the necessary number of columns (" + columns + " != " + c + ").");
        }
    }


    /**
     * Set elements in linear ordering in the specified indices.
     *
     * For example, <code>a.put(new int[]{ 1, 2, 0 }, new FloatMatrix(3, 1, 2.0f, 4.0f, 8.0f)</code>
     * does <code>a.put(1, 2.0f), a.put(2, 4.0f), a.put(0, 8.0f)</code>.
     */
    public FloatMatrix put(int[] indices, FloatMatrix x) {
        if (x.isScalar()) {
            return put(indices, x.scalar());
        }
        x.checkLength(indices.length);

        for (int i = 0; i < indices.length; i++) {
            put(indices[i], x.get(i));
        }

        return this;
    }

    /** Set multiple elements in a row. */
    public FloatMatrix put(int r, int[] indices, FloatMatrix x) {
        if (x.isScalar()) {
            return put(r, indices, x.scalar());
        }
        x.checkColumns(indices.length);

        for (int i = 0; i < indices.length; i++) {
            put(r, indices[i], x.get(i));
        }

        return this;
    }

    /** Set multiple elements in a row. */
    public FloatMatrix put(int[] indices, int c, FloatMatrix x) {
        if (x.isScalar()) {
            return put(indices, c, x.scalar());
        }
        x.checkRows(indices.length);

        for (int i = 0; i < indices.length; i++) {
            put(indices[i], c, x.get(i));
        }

        return this;
    }

    /** Put a sub-matrix as specified by the indices. */
    public FloatMatrix put(int[] rindices, int[] cindices, FloatMatrix x) {
        if (x.isScalar()) {
            return put(rindices, cindices, x.scalar());
        }
        x.checkRows(rindices.length);
        x.checkColumns(cindices.length);

        for (int i = 0; i < rindices.length; i++) {
            for (int j = 0; j < cindices.length; j++) {
                put(rindices[i], cindices[j], x.get(i, j));
            }
        }

        return this;
    }

    /** Put a matrix into specified indices. */
    public FloatMatrix put(Range rs, Range cs, FloatMatrix x) {
        rs.init(0, rows);
        cs.init(0, columns);

        x.checkRows(rs.length());
        x.checkColumns(cs.length());

        for (; rs.hasMore(); rs.next()) {
            cs.init(0, columns);
            for (; cs.hasMore(); cs.next()) {
                put(rs.value(), cs.value(), x.get(rs.index(), cs.index()));
            }
        }

        return this;
    }

    /** Put a single value into the specified indices (linear adressing). */
    public FloatMatrix put(int[] indices, float v) {
        for (int i = 0; i < indices.length; i++) {
            put(indices[i], v);
        }

        return this;
    }

    /** Put a single value into a row and the specified columns. */
    public FloatMatrix put(int r, int[] indices, float v) {
        for (int i = 0; i < indices.length; i++) {
            put(r, indices[i], v);
        }

        return this;
    }

    /** Put a single value into the specified rows of a column. */
    public FloatMatrix put(int[] indices, int c, float v) {
        for (int i = 0; i < indices.length; i++) {
            put(indices[i], c, v);
        }

        return this;
    }

    /** Put a single value into the specified rows and columns. */
    public FloatMatrix put(int[] rindices, int[] cindices, float v) {
        for (int i = 0; i < rindices.length; i++) {
            for (int j = 0; j < cindices.length; j++) {
                put(rindices[i], cindices[j], v);
            }
        }

        return this;
    }

    /**
     * Put a sub-matrix into the indices specified by the non-zero entries
     * of <tt>indices</tt> (linear adressing).
     */
    public FloatMatrix put(FloatMatrix indices, FloatMatrix v) {
        return put(indices.findIndices(), v);
    }

    /** Put a sub-vector into the specified columns (non-zero entries of <tt>indices</tt>) of a row. */
    public FloatMatrix put(int r, FloatMatrix indices, FloatMatrix v) {
        return put(r, indices.findIndices(), v);
    }

    /** Put a sub-vector into the specified rows (non-zero entries of <tt>indices</tt>) of a column. */
    public FloatMatrix put(FloatMatrix indices, int c, FloatMatrix v) {
        return put(indices.findIndices(), c, v);
    }

    /**
     * Put a sub-matrix into the specified rows and columns (non-zero entries of
     * <tt>rindices</tt> and <tt>cindices</tt>.
     */
    public FloatMatrix put(FloatMatrix rindices, FloatMatrix cindices, FloatMatrix v) {
        return put(rindices.findIndices(), cindices.findIndices(), v);
    }

    /**
     * Put a single value into the elements specified by the non-zero
     * entries of <tt>indices</tt> (linear adressing).
     */
    public FloatMatrix put(FloatMatrix indices, float v) {
        return put(indices.findIndices(), v);
    }

    /**
     * Put a single value into the specified columns (non-zero entries of
     * <tt>indices</tt>) of a row.
     */
    public FloatMatrix put(int r, FloatMatrix indices, float v) {
        return put(r, indices.findIndices(), v);
    }

    /**
     * Put a single value into the specified rows (non-zero entries of
     * <tt>indices</tt>) of a column.
     */
    public FloatMatrix put(FloatMatrix indices, int c, float v) {
        return put(indices.findIndices(), c, v);
    }

    /**
     * Put a single value in the specified rows and columns (non-zero entries
     * of <tt>rindices</tt> and <tt>cindices</tt>.
     */
    public FloatMatrix put(FloatMatrix rindices, FloatMatrix cindices, float v) {
        return put(rindices.findIndices(), cindices.findIndices(), v);
    }

    /** Find the linear indices of all non-zero elements. */
    public int[] findIndices() {
        int len = 0;
        for (int i = 0; i < length; i++) {
            if (get(i) != 0.0f) {
                len++;
            }
        }

        int[] indices = new int[len];
        int c = 0;

        for (int i = 0; i < length; i++) {
            if (get(i) != 0.0f) {
                indices[c++] = i;
            }
        }

        return indices;
    }

    /**************************************************************************
     * Basic operations (copying, resizing, element access)
     */
    /** Return transposed copy of this matrix. */
    public FloatMatrix transpose() {
        FloatMatrix result = new FloatMatrix(columns, rows);

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < columns; j++) {
                result.put(j, i, get(i, j));
            }
        }

        return result;
    }

    /**
     * Compare two matrices. Returns true if and only if other is also a
     * FloatMatrix which has the same size and the maximal absolute
     * difference in matrix elements is smaller thatn 1e-6.
     */
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof FloatMatrix)) {
            return false;
        }

        FloatMatrix other = (FloatMatrix) o;

        if (!sameSize(other)) {
            return false;
        }

        FloatMatrix diff = MatrixFunctions.absi(sub(other));

        return diff.max() / (rows * columns) < 1e-6;
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 83 * hash + this.rows;
        hash = 83 * hash + this.columns;
        hash = 83 * hash + Arrays.hashCode(this.data);
        return hash;
    }
    
    /** Resize the matrix. All elements will be set to zero. */
    public void resize(int newRows, int newColumns) {
        rows = newRows;
        columns = newColumns;
        length = newRows * newColumns;
        data = new float[rows * columns];
    }

    /** Reshape the matrix. Number of elements must not change. */
    public FloatMatrix reshape(int newRows, int newColumns) {
        if (length != newRows * newColumns) {
            throw new IllegalArgumentException(
                    "Number of elements must not change.");
        }

        rows = newRows;
        columns = newColumns;

        return this;
    }

    /** Generate a new matrix which has the given number of replications of this. */
    public FloatMatrix repmat(int rowMult, int columnMult) {
        FloatMatrix result = new FloatMatrix(rows * rowMult, columns * columnMult);

        for (int c = 0; c < columnMult; c++) {
            for (int r = 0; r < rowMult; r++) {
                for (int i = 0; i < rows; i++) {
                    for (int j = 0; j < columns; j++) {
                        result.put(r * rows + i, c * columns + j, get(i, j));
                    }
                }
            }
        }
        return result;
    }

    /** Checks whether two matrices have the same size. */
    public boolean sameSize(FloatMatrix a) {
        return rows == a.rows && columns == a.columns;
    }

    /** Throws SizeException unless two matrices have the same size. */
    public void assertSameSize(FloatMatrix a) {
        if (!sameSize(a)) {
            throw new SizeException("Matrices must have the same size.");
        }
    }

    /** Checks whether two matrices can be multiplied (that is, number of columns of
     * this must equal number of rows of a. */
    public boolean multipliesWith(FloatMatrix a) {
        return columns == a.rows;
    }

    /** Throws SizeException unless matrices can be multiplied with one another. */
    public void assertMultipliesWith(FloatMatrix a) {
        if (!multipliesWith(a)) {
            throw new SizeException("Number of columns of left matrix must be equal to number of rows of right matrix.");
        }
    }

    /** Checks whether two matrices have the same length. */
    public boolean sameLength(FloatMatrix a) {
        return length == a.length;
    }

    /** Throws SizeException unless matrices have the same length. */
    public void assertSameLength(FloatMatrix a) {
        if (!sameLength(a)) {
            throw new SizeException("Matrices must have same length (is: " + length + " and " + a.length + ")");
        }
    }

    /** Copy FloatMatrix a to this. this a is resized if necessary. */
    public FloatMatrix copy(FloatMatrix a) {
        if (!sameSize(a)) {
            resize(a.rows, a.columns);
        }

        System.arraycopy(a.data, 0, data, 0, length);
        return a;
    }

    /**
     * Returns a duplicate of this matrix. Geometry is the same (including offsets, transpose, etc.),
     * but the buffer is not shared.
     */
    public FloatMatrix dup() {
        FloatMatrix out = new FloatMatrix(rows, columns);

        JavaBlas.rcopy(length, data, 0, 1, out.data, 0, 1);

        return out;
    }

    /** Swap two columns of a matrix. */
    public FloatMatrix swapColumns(int i, int j) {
        NativeBlas.sswap(rows, data, index(0, i), 1, data, index(0, j), 1);
        return this;
    }

    /** Swap two rows of a matrix. */
    public FloatMatrix swapRows(int i, int j) {
        NativeBlas.sswap(columns, data, index(i, 0), rows, data, index(j, 0), rows);
        return this;
    }

    /** Set matrix element */
    public FloatMatrix put(int rowIndex, int columnIndex, float value) {
        data[index(rowIndex, columnIndex)] = value;
        return this;
    }

    /** Retrieve matrix element */
    public float get(int rowIndex, int columnIndex) {
        return data[index(rowIndex, columnIndex)];
    }

    /** Get index of an element */
    public int index(int rowIndex, int columnIndex) {
        return rowIndex + rows * columnIndex;
    }

    /** Compute the row index of a linear index. */
    public int indexRows(int i) {
        return i / rows;
    }

    /** Compute the column index of a linear index. */
    public int indexColumns(int i) {
        return i - indexRows(i) * rows;
    }

    /** Get a matrix element (linear indexing). */
    public float get(int i) {
        return data[i];
    }

    /** Set a matrix element (linear indexing). */
    public FloatMatrix put(int i, float v) {
        data[i] = v;
        return this;
    }

    /** Set all elements to a value. */
    public FloatMatrix fill(float value) {
        for (int i = 0; i < length; i++) {
            put(i, value);
        }
        return this;
    }

    /** Get number of rows. */
    public int getRows() {
        return rows;
    }

    /** Get number of columns. */
    public int getColumns() {
        return columns;
    }

    /** Get total number of elements. */
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

    /** Throw SizeException unless matrix is square. */
    public void assertSquare() {
        if (!isSquare()) {
            throw new SizeException("Matrix must be square!");
        }
    }

    /** Checks whether the matrix is a vector. */
    public boolean isVector() {
        return columns == 1 || rows == 1;
    }

    /** Checks whether the matrix is a row vector. */
    public boolean isRowVector() {
        return rows == 1;
    }

    /** Checks whether the matrix is a column vector. */
    public boolean isColumnVector() {
        return columns == 1;
    }

    /** Returns the diagonal of the matrix. */
    public FloatMatrix diag() {
        assertSquare();
        FloatMatrix d = new FloatMatrix(rows);
        JavaBlas.rcopy(rows, data, 0, rows + 1, d.data, 0, 1);
        return d;
    }

    /** Pretty-print this matrix to <tt>System.out</tt>. */
    public void print() {
        System.out.println(toString());
    }

    /** Generate string representation of the matrix. */
    @Override
    public String toString() {
        return toString("%f");
    }

    /**
     * Generate string representation of the matrix, with specified
     * format for the entries. For example, <code>x.toString("%.1f")</code>
     * generates a string representations having only one position after the
     * decimal point.
     */
    public String toString(String fmt) {
        return toString(fmt, "[", "]", ", ", "; ");
    }

  /**
   * Generate string representation of the matrix, with specified
   * format for the entries, and delimiters.
   *
   * @param fmt entry format (passed to String.format())
   * @param open opening parenthesis
   * @param close closing parenthesis
   * @param colSep separator between columns
   * @param rowSep separator between rows
   */
    public String toString(String fmt, String open, String close, String colSep, String rowSep) {
        StringWriter s = new StringWriter();
        PrintWriter p = new PrintWriter(s);

        p.print(open);

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                p.printf(fmt, get(r, c));
                if (c < columns - 1) {
                    p.print(colSep);
                }
            }
            if (r < rows - 1) {
                p.print(rowSep);
            }
        }

        p.print(close);

        return s.toString();
    }

    /** Converts the matrix to a one-dimensional array of floats. */
    public float[] toArray() {
        float[] array = new float[length];

        System.arraycopy(data, 0, array, 0, length);

        return array;
    }

    /** Converts the matrix to a two-dimensional array of floats. */
    public float[][] toArray2() {
        float[][] array = new float[rows][columns];

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                array[r][c] = get(r, c);
            }
        }

        return array;
    }

    /** Converts the matrix to a one-dimensional array of integers. */
    public int[] toIntArray() {
        int[] array = new int[length];

        for (int i = 0; i < length; i++) {
            array[i] = (int) Math.rint(get(i));
        }

        return array;
    }

    /** Convert the matrix to a two-dimensional array of integers. */
    public int[][] toIntArray2() {
        int[][] array = new int[rows][columns];

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                array[r][c] = (int) Math.rint(get(r, c));
            }
        }

        return array;
    }

    /** Convert the matrix to a one-dimensional array of boolean values. */
    public boolean[] toBooleanArray() {
        boolean[] array = new boolean[length];

        for (int i = 0; i < length; i++) {
            array[i] = get(i) != 0.0f ? true : false;
        }

        return array;
    }

    /** Convert the matrix to a two-dimensional array of boolean values. */
    public boolean[][] toBooleanArray2() {
        boolean[][] array = new boolean[rows][columns];

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < columns; c++) {
                array[r][c] = get(r, c) != 0.0f ? true : false;
            }
        }

        return array;
    }

    public FloatMatrix toFloat() {
         FloatMatrix result = new FloatMatrix(rows, columns);
         for (int i = 0; i < length; i++) {
            result.put(i, (float) get(i));
         }
         return result;
    }

    /**
     * A wrapper which allows to view a matrix as a List of Doubles (read-only!).
     * Also implements the {@link ConvertsToFloatMatrix} interface.
     */
    public class ElementsAsListView extends AbstractList<Float> implements ConvertsToFloatMatrix {

        private FloatMatrix me;

        public ElementsAsListView(FloatMatrix me) {
            this.me = me;
        }

        @Override
        public Float get(int index) {
            return me.get(index);
        }

        @Override
        public int size() {
            return me.length;
        }

        public FloatMatrix convertToFloatMatrix() {
            return me;
        }
    }

    public class RowsAsListView extends AbstractList<FloatMatrix> implements ConvertsToFloatMatrix {

        private FloatMatrix me;

        public RowsAsListView(FloatMatrix me) {
            this.me = me;
        }

        @Override
        public FloatMatrix get(int index) {
            return getRow(index);
        }

        @Override
        public int size() {
            return rows;
        }

        public FloatMatrix convertToFloatMatrix() {
            return me;
        }
    }

    public class ColumnsAsListView extends AbstractList<FloatMatrix> implements ConvertsToFloatMatrix {

        private FloatMatrix me;

        public ColumnsAsListView(FloatMatrix me) {
            this.me = me;
        }

        @Override
        public FloatMatrix get(int index) {
            return getColumn(index);
        }

        @Override
        public int size() {
            return columns;
        }

        public FloatMatrix convertToFloatMatrix() {
            return me;
        }
    }

    public List<Float> elementsAsList() {
        return new ElementsAsListView(this);
    }

    public List<FloatMatrix> rowsAsList() {
        return new RowsAsListView(this);
    }

    public List<FloatMatrix> columnsAsList() {
        return new ColumnsAsListView(this);
    }

    /**************************************************************************
     * Arithmetic Operations
     */
    /**
     * Ensures that the result vector has the same length as this. If not,
     * resizing result is tried, which fails if result == this or result == other.
     */
    private void ensureResultLength(FloatMatrix other, FloatMatrix result) {
        if (!sameLength(result)) {
            if (result == this || result == other) {
                throw new SizeException("Cannot resize result matrix because it is used in-place.");
            }
            result.resize(rows, columns);
        }
    }

    /** Add two matrices (in-place). */
    public FloatMatrix addi(FloatMatrix other, FloatMatrix result) {
        if (other.isScalar()) {
            return addi(other.scalar(), result);
        }
        if (isScalar()) {
            return other.addi(scalar(), result);
        }

        assertSameLength(other);
        ensureResultLength(other, result);

        if (result == this) {
            SimpleBlas.axpy(1.0f, other, result);
        } else if (result == other) {
            SimpleBlas.axpy(1.0f, this, result);
        } else {
            /*SimpleBlas.copy(this, result);
            SimpleBlas.axpy(1.0f, other, result);*/
            JavaBlas.rzgxpy(length, result.data, data, other.data);
        }

        return result;
    }

    /** Add a scalar to a matrix (in-place). */
    public FloatMatrix addi(float v, FloatMatrix result) {
        ensureResultLength(null, result);

        for (int i = 0; i < length; i++) {
            result.put(i, get(i) + v);
        }
        return result;
    }

    /** Subtract two matrices (in-place). */
    public FloatMatrix subi(FloatMatrix other, FloatMatrix result) {
        if (other.isScalar()) {
            return subi(other.scalar(), result);
        }
        if (isScalar()) {
            return other.rsubi(scalar(), result);
        }

        assertSameLength(other);
        ensureResultLength(other, result);

        if (result == this) {
            SimpleBlas.axpy(-1.0f, other, result);
        } else if (result == other) {
            SimpleBlas.scal(-1.0f, result);
            SimpleBlas.axpy(1.0f, this, result);
        } else {
            SimpleBlas.copy(this, result);
            SimpleBlas.axpy(-1.0f, other, result);
        }
        return result;
    }

    /** Subtract a scalar from a matrix (in-place). */
    public FloatMatrix subi(float v, FloatMatrix result) {
        ensureResultLength(null, result);

        for (int i = 0; i < length; i++) {
            result.put(i, get(i) - v);
        }
        return result;
    }

    /**
     * Subtract two matrices, but subtract first from second matrix, that is,
     * compute <em>result = other - this</em> (in-place).
     * */
    public FloatMatrix rsubi(FloatMatrix other, FloatMatrix result) {
        return other.subi(this, result);
    }

    /** Subtract a matrix from a scalar (in-place). */
    public FloatMatrix rsubi(float a, FloatMatrix result) {
        ensureResultLength(null, result);

        for (int i = 0; i < length; i++) {
            result.put(i, a - get(i));
        }
        return result;
    }

    /** Elementwise multiplication (in-place). */
    public FloatMatrix muli(FloatMatrix other, FloatMatrix result) {
        if (other.isScalar()) {
            return muli(other.scalar(), result);
        }
        if (isScalar()) {
            return other.muli(scalar(), result);
        }

        assertSameLength(other);
        ensureResultLength(other, result);

        for (int i = 0; i < length; i++) {
            result.put(i, get(i) * other.get(i));
        }
        return result;
    }

    /** Elementwise multiplication with a scalar (in-place). */
    public FloatMatrix muli(float v, FloatMatrix result) {
        ensureResultLength(null, result);

        for (int i = 0; i < length; i++) {
            result.put(i, get(i) * v);
        }
        return result;
    }

    /** Matrix-matrix multiplication (in-place). */
    public FloatMatrix mmuli(FloatMatrix other, FloatMatrix result) {
        if (other.isScalar()) {
            return muli(other.scalar(), result);
        }
        if (isScalar()) {
            return other.muli(scalar(), result);
        }

        /* check sizes and resize if necessary */
        assertMultipliesWith(other);
        if (result.rows != rows || result.columns != other.columns) {
            if (result != this && result != other) {
                result.resize(rows, other.columns);
            } else {
                throw new SizeException("Cannot resize result matrix because it is used in-place.");
            }
        }

        if (result == this || result == other) {
            /* actually, blas cannot do multiplications in-place. Therefore, we will fake by
             * allocating a temporary object on the side and copy the result later.
             */
            FloatMatrix temp = new FloatMatrix(result.rows, result.columns);
            if (other.columns == 1) {
                SimpleBlas.gemv(1.0f, this, other, 0.0f, temp);
            } else {
                SimpleBlas.gemm(1.0f, this, other, 0.0f, temp);
            }
            SimpleBlas.copy(temp, result);
        } else {
            if (other.columns == 1) {
                SimpleBlas.gemv(1.0f, this, other, 0.0f, result);
            } else {
                SimpleBlas.gemm(1.0f, this, other, 0.0f, result);
            }
        }
        return result;
    }

    /** Matrix-matrix multiplication with a scalar (for symmetry, does the
     * same as <code>muli(scalar)</code> (in-place).
     */
    public FloatMatrix mmuli(float v, FloatMatrix result) {
        return muli(v, result);
    }

    /** Elementwise division (in-place). */
    public FloatMatrix divi(FloatMatrix other, FloatMatrix result) {
        if (other.isScalar()) {
            return divi(other.scalar(), result);
        }
        if (isScalar()) {
            return other.rdivi(scalar(), result);
        }

        assertSameLength(other);
        ensureResultLength(other, result);

        for (int i = 0; i < length; i++) {
            result.put(i, get(i) / other.get(i));
        }
        return result;
    }

    /** Elementwise division with a scalar (in-place). */
    public FloatMatrix divi(float a, FloatMatrix result) {
        ensureResultLength(null, result);

        for (int i = 0; i < length; i++) {
            result.put(i, get(i) / a);
        }
        return result;
    }

    /**
     * Elementwise division, with operands switched. Computes
     * <code>result = other / this</code> (in-place). */
    public FloatMatrix rdivi(FloatMatrix other, FloatMatrix result) {
        return other.divi(this, result);
    }

    /** (Elementwise) division with a scalar, with operands switched. Computes
     * <code>result = a / this</code> (in-place). */
    public FloatMatrix rdivi(float a, FloatMatrix result) {
        ensureResultLength(null, result);

        for (int i = 0; i < length; i++) {
            result.put(i, a / get(i));
        }
        return result;
    }

    /** Negate each element (in-place). */
    public FloatMatrix negi() {
        for (int i = 0; i < length; i++) {
            put(i, -get(i));
        }
        return this;
    }

    /** Negate each element. */
    public FloatMatrix neg() {
        return dup().negi();
    }

    /** Maps zero to 1.0f and all non-zero values to 0.0f (in-place). */
    public FloatMatrix noti() {
        for (int i = 0; i < length; i++) {
            put(i, get(i) == 0.0f ? 1.0f : 0.0f);
        }
        return this;
    }

    /** Maps zero to 1.0f and all non-zero values to 0.0f. */
    public FloatMatrix not() {
        return dup().noti();
    }

    /** Maps zero to 0.0f and all non-zero values to 1.0f (in-place). */
    public FloatMatrix truthi() {
        for (int i = 0; i < length; i++) {
            put(i, get(i) == 0.0f ? 0.0f : 1.0f);
        }
        return this;
    }

    /** Maps zero to 0.0f and all non-zero values to 1.0f. */
    public FloatMatrix truth() {
        return dup().truthi();
    }

    public FloatMatrix isNaNi() {
        for (int i = 0; i < length; i++) {
            put(i, Float.isNaN(get(i)) ? 1.0f : 0.0f);
        }
        return this;
    }

    public FloatMatrix isNaN() {
        return dup().isNaNi();
    }

    public FloatMatrix isInfinitei() {
        for (int i = 0; i < length; i++) {
            put(i, Float.isInfinite(get(i)) ? 1.0f : 0.0f);
        }
        return this;
    }

    public FloatMatrix isInfinite() {
        return dup().isInfinitei();
    }

    /** Checks whether all entries (i, j) with i >= j are zero. */
    public boolean isLowerTriangular() {
      for (int i = 0; i < rows; i++)
        for (int j = i+1; j < columns; j++) {
          if (get(i, j) != 0.0f)
            return false;
        }

      return true;
    }

  /**
   * Checks whether all entries (i, j) with i <= j are zero.
   */
    public boolean isUpperTriangular() {
      for (int i = 0; i < rows; i++)
        for (int j = 0; j < i && j < columns; j++) {
          if (get(i, j) != 0.0f)
            return false;
        }

      return true;
    }

    public FloatMatrix selecti(FloatMatrix where) {
        checkLength(where.length);
        for (int i = 0; i < length; i++) {
            if (where.get(i) == 0.0f) {
                put(i, 0.0f);
            }
        }
        return this;
    }

    public FloatMatrix select(FloatMatrix where) {
        return dup().selecti(where);
    }

    /****************************************************************
     * Rank one-updates
     */
    /** Computes a rank-1-update A = A + alpha * x * y'. */
    public FloatMatrix rankOneUpdate(float alpha, FloatMatrix x, FloatMatrix y) {
        if (rows != x.length) {
            throw new SizeException("Vector x has wrong length (" + x.length + " != " + rows + ").");
        }
        if (columns != y.length) {
            throw new SizeException("Vector y has wrong length (" + x.length + " != " + columns + ").");
        }

        SimpleBlas.ger(alpha, x, y, this);
        return this;
    }

    /** Computes a rank-1-update A = A + alpha * x * x'. */
    public FloatMatrix rankOneUpdate(float alpha, FloatMatrix x) {
        return rankOneUpdate(alpha, x, x);
    }

    /** Computes a rank-1-update A = A + x * x'. */
    public FloatMatrix rankOneUpdate(FloatMatrix x) {
        return rankOneUpdate(1.0f, x, x);
    }

    /** Computes a rank-1-update A = A + x * y'. */
    public FloatMatrix rankOneUpdate(FloatMatrix x, FloatMatrix y) {
        return rankOneUpdate(1.0f, x, y);
    }

    /****************************************************************
     * Logical operations
     */
    /** Returns the minimal element of the matrix. */
    public float min() {
        if (isEmpty()) {
            return Float.POSITIVE_INFINITY;
        }
        float v = Float.POSITIVE_INFINITY;
        for (int i = 0; i < length; i++) {
            if (!Float.isNaN(get(i)) && get(i) < v) {
                v = get(i);
            }
        }

        return v;
    }

    /**
     * Returns the linear index of the minimal element. If there are
     * more than one elements with this value, the first one is returned.
     */
    public int argmin() {
        if (isEmpty()) {
            return -1;
        }
        float v = Float.POSITIVE_INFINITY;
        int a = -1;
        for (int i = 0; i < length; i++) {
            if (!Float.isNaN(get(i)) && get(i) < v) {
                v = get(i);
                a = i;
            }
        }

        return a;
    }

    /**
     * Computes the minimum between two matrices. Returns the smaller of the
     * corresponding elements in the matrix (in-place).
     */
    public FloatMatrix mini(FloatMatrix other, FloatMatrix result) {
        if (result == this) {
            for (int i = 0; i < length; i++) {
                if (get(i) > other.get(i)) {
                    put(i, other.get(i));
                }
            }
        } else {
            for (int i = 0; i < length; i++) {
                if (get(i) > other.get(i)) {
                    result.put(i, other.get(i));
                } else {
                    result.put(i, get(i));
                }
            }
        }
        return result;
    }

    /**
     * Computes the minimum between two matrices. Returns the smaller of the
     * corresponding elements in the matrix (in-place on this).
     */
    public FloatMatrix mini(FloatMatrix other) {
        return mini(other, this);
    }

    /**
     * Computes the minimum between two matrices. Returns the smaller of the
     * corresponding elements in the matrix (in-place on this).
     */
    public FloatMatrix min(FloatMatrix other) {
        return mini(other, new FloatMatrix(rows, columns));
    }

    public FloatMatrix mini(float v, FloatMatrix result) {
        if (result == this) {
            for (int i = 0; i < length; i++) {
                if (get(i) > v) {
                    result.put(i, v);
                }
            }
        } else {
            for (int i = 0; i < length; i++) {
                if (get(i) > v) {
                    result.put(i, v);
                } else {
                    result.put(i, get(i));
                }
            }

        }
        return result;
    }

    public FloatMatrix mini(float v) {
        return mini(v, this);
    }

    public FloatMatrix min(float v) {
        return mini(v, new FloatMatrix(rows, columns));
    }

    /** Returns the maximal element of the matrix. */
    public float max() {
        if (isEmpty()) {
            return Float.NEGATIVE_INFINITY;
        }
        float v = Float.NEGATIVE_INFINITY;
        for (int i = 0; i < length; i++) {
            if (!Float.isNaN(get(i)) && get(i) > v) {
                v = get(i);
            }
        }
        return v;
    }

    /**
     * Returns the linear index of the maximal element of the matrix. If
     * there are more than one elements with this value, the first one
     * is returned.
     */
    public int argmax() {
        if (isEmpty()) {
            return -1;
        }
        float v = Float.NEGATIVE_INFINITY;
        int a = -1;
        for (int i = 0; i < length; i++) {
            if (!Float.isNaN(get(i)) && get(i) > v) {
                v = get(i);
                a = i;
            }
        }

        return a;
    }

    /**
     * Computes the maximum between two matrices. Returns the larger of the
     * corresponding elements in the matrix (in-place).
     */
    public FloatMatrix maxi(FloatMatrix other, FloatMatrix result) {
        if (result == this) {
            for (int i = 0; i < length; i++) {
                if (get(i) < other.get(i)) {
                    put(i, other.get(i));
                }
            }
        } else {
            for (int i = 0; i < length; i++) {
                if (get(i) < other.get(i)) {
                    result.put(i, other.get(i));
                } else {
                    result.put(i, get(i));
                }
            }
        }
        return result;
    }

    /**
     * Computes the maximum between two matrices. Returns the smaller of the
     * corresponding elements in the matrix (in-place on this).
     */
    public FloatMatrix maxi(FloatMatrix other) {
        return maxi(other, this);
    }

    /**
     * Computes the maximum between two matrices. Returns the larger of the
     * corresponding elements in the matrix (in-place on this).
     */
    public FloatMatrix max(FloatMatrix other) {
        return maxi(other, new FloatMatrix(rows, columns));
    }

    public FloatMatrix maxi(float v, FloatMatrix result) {
        if (result == this) {
            for (int i = 0; i < length; i++) {
                if (get(i) < v) {
                    result.put(i, v);
                }
            }
        } else {
            for (int i = 0; i < length; i++) {
                if (get(i) < v) {
                    result.put(i, v);
                } else {
                    result.put(i, get(i));
                }
            }

        }
        return result;
    }

    public FloatMatrix maxi(float v) {
        return maxi(v, this);
    }

    public FloatMatrix max(float v) {
        return maxi(v, new FloatMatrix(rows, columns));
    }

    /** Computes the sum of all elements of the matrix. */
    public float sum() {
        float s = 0.0f;
        for (int i = 0; i < length; i++) {
            s += get(i);
        }
        return s;
    }

    /** Computes the product of all elements of the matrix */
    public float prod() {
        float p = 1.0f;
        for (int i = 0; i < length; i++) {
            p *= get(i);
        }
        return p;
    }

    /**
     * Computes the mean value of all elements in the matrix,
     * that is, <code>x.sum() / x.length</code>.
     */
    public float mean() {
        return sum() / length;
    }

    /**
     * Computes the cumulative sum, that is, the sum of all elements
     * of the matrix up to a given index in linear addressing (in-place).
     */
    public FloatMatrix cumulativeSumi() {
        float s = 0.0f;
        for (int i = 0; i < length; i++) {
            s += get(i);
            put(i, s);
        }
        return this;
    }

    /**
     * Computes the cumulative sum, that is, the sum of all elements
     * of the matrix up to a given index in linear addressing.
     */
    public FloatMatrix cumulativeSum() {
        return dup().cumulativeSumi();
    }

    /** The scalar product of this with other. */
    public float dot(FloatMatrix other) {
        return SimpleBlas.dot(this, other);
    }

    /** 
     * Computes the projection coefficient of other on this.
     *
     * The returned scalar times <tt>this</tt> is the orthogonal projection
     * of <tt>other</tt> on <tt>this</tt>.
     */
    public float project(FloatMatrix other) {
        other.checkLength(length);
        float norm = 0, dot = 0;
        for (int i = 0; i < this.length; i++) {
            float x = get(i);
            norm += x * x;
            dot += x * other.get(i);
        }
        return dot / norm;
    }

    /**
     * The Euclidean norm of the matrix as vector, also the Frobenius
     * norm of the matrix.
     */
    public float norm2() {
        float norm = 0.0f;
        for (int i = 0; i < length; i++) {
            norm += get(i) * get(i);
        }
        return (float) Math.sqrt(norm);
    }

    /**
     * The maximum norm of the matrix (maximal absolute value of the elements).
     */
    public float normmax() {
        float max = 0.0f;
        for (int i = 0; i < length; i++) {
            float a = Math.abs(get(i));
            if (a > max) {
                max = a;
            }
        }
        return max;
    }

    /**
     * The 1-norm of the matrix as vector (sum of absolute values of elements).
     */
    public float norm1() {
        float norm = 0.0f;
        for (int i = 0; i < length; i++) {
            norm += Math.abs(get(i));
        }
        return norm;
    }

    /**
     * Returns the squared (Euclidean) distance.
     */
    public float squaredDistance(FloatMatrix other) {
        other.checkLength(length);
        float sd = 0.0f;
        for (int i = 0; i < length; i++) {
            float d = get(i) - other.get(i);
            sd += d * d;
        }
        return sd;
    }

    /**
     * Returns the (euclidean) distance.
     */
    public float distance2(FloatMatrix other) {
        return (float) Math.sqrt(squaredDistance(other));
    }

    /**
     * Returns the (1-norm) distance.
     */
    public float distance1(FloatMatrix other) {
        other.checkLength(length);
        float d = 0.0f;
        for (int i = 0; i < length; i++) {
            d += Math.abs(get(i) - other.get(i));
        }
        return d;
    }

    /**
     * Return a new matrix with all elements sorted.
     */
    public FloatMatrix sort() {
        float array[] = toArray();
        java.util.Arrays.sort(array);
        return new FloatMatrix(rows, columns, array);
    }

    /**
     * Sort elements in-place.
     */
    public FloatMatrix sorti() {
        Arrays.sort(data);
        return this;
    }

    /**
     * Get the sorting permutation.
     *
     * @return an int[] array such that which indexes the elements in sorted
     * order.
     */
    public int[] sortingPermutation() {
        Integer[] indices = new Integer[length];

        for (int i = 0; i < length; i++) {
            indices[i] = i;
        }

        final float[] array = data;

        Arrays.sort(indices, new Comparator() {

            public int compare(Object o1, Object o2) {
                int i = (Integer) o1;
                int j = (Integer) o2;
                if (array[i] < array[j]) {
                    return -1;
                } else if (array[i] == array[j]) {
                    return 0;
                } else {
                    return 1;
                }
            }
        });

        int[] result = new int[length];

        for (int i = 0; i < length; i++) {
            result[i] = indices[i];
        }

        return result;
    }

    /**
     * Sort columns (in-place).
     */
    public FloatMatrix sortColumnsi() {
        for (int i = 0; i < length; i += rows) {
            Arrays.sort(data, i, i + rows);
        }
        return this;
    }

    /** Sort columns. */
    public FloatMatrix sortColumns() {
        return dup().sortColumnsi();
    }

    /** Return matrix of indices which sort all columns. */
    public int[][] columnSortingPermutations() {
        int[][] result = new int[columns][];

        FloatMatrix temp = new FloatMatrix(rows);
        for (int c = 0; c < columns; c++) {
            result[c] = getColumn(c, temp).sortingPermutation();
        }

        return result;
    }

    /** Sort rows (in-place). */
    public FloatMatrix sortRowsi() {
        // actually, this is much harder because the data is not consecutive
        // in memory...
        FloatMatrix temp = new FloatMatrix(columns);
        for (int r = 0; r < rows; r++) {
            putRow(r, getRow(r, temp).sorti());
        }
        return this;
    }

    /** Sort rows. */
    public FloatMatrix sortRows() {
        return dup().sortRowsi();
    }

    /** Return matrix of indices which sort all columns. */
    public int[][] rowSortingPermutations() {
        int[][] result = new int[rows][];

        FloatMatrix temp = new FloatMatrix(columns);
        for (int r = 0; r < rows; r++) {
            result[r] = getRow(r, temp).sortingPermutation();
        }

        return result;
    }

    /** Return a vector containing the sums of the columns (having number of columns many entries) */
    public FloatMatrix columnSums() {
        if (rows == 1) {
            return dup();
        } else {
            FloatMatrix v = new FloatMatrix(1, columns);

            for (int c = 0; c < columns; c++) {
                for (int r = 0; r < rows; r++) {
                    v.put(c, v.get(c) + get(r, c));
                }
            }

            return v;
        }
    }

    /** Return a vector containing the means of all columns. */
    public FloatMatrix columnMeans() {
        return columnSums().divi(rows);
    }

    /** Return a vector containing the sum of the rows. */
    public FloatMatrix rowSums() {
        if (columns == 1) {
            return dup();
        } else {
            FloatMatrix v = new FloatMatrix(rows);

            for (int c = 0; c < columns; c++) {
                for (int r = 0; r < rows; r++) {
                    v.put(r, v.get(r) + get(r, c));
                }
            }

            return v;
        }
    }

    /** Return a vector containing the means of the rows. */
    public FloatMatrix rowMeans() {
        return rowSums().divi(columns);
    }

    /************************************************************************
     * Column and rows access.
     */

    /** Get a copy of a column. */
    public FloatMatrix getColumn(int c) {
        return getColumn(c, new FloatMatrix(rows, 1));
    }

    /** Copy a column to the given vector. */
    public FloatMatrix getColumn(int c, FloatMatrix result) {
        result.checkLength(rows);
        JavaBlas.rcopy(rows, data, index(0, c), 1, result.data, 0, 1);
        return result;
    }

    /** Copy a column back into the matrix. */
    public void putColumn(int c, FloatMatrix v) {
        JavaBlas.rcopy(rows, v.data, 0, 1, data, index(0, c), 1);
    }

    /** Get a copy of a row. */
    public FloatMatrix getRow(int r) {
        return getRow(r, new FloatMatrix(1, columns));
    }

    /** Copy a row to a given vector. */
    public FloatMatrix getRow(int r, FloatMatrix result) {
        result.checkLength(columns);
        JavaBlas.rcopy(columns, data, index(r, 0), rows, result.data, 0, 1);
        return result;
    }

    /** Copy a row back into the matrix. */
    public void putRow(int r, FloatMatrix v) {
        JavaBlas.rcopy(columns, v.data, 0, 1, data, index(r, 0), rows);
    }

    /** Return column-wise minimums. */
    public FloatMatrix columnMins() {
        FloatMatrix mins = new FloatMatrix(1, columns);
        for (int c = 0; c < columns; c++) {
            mins.put(c, getColumn(c).min());
        }
        return mins;
    }

    /** Return index of minimal element per column. */
    public int[] columnArgmins() {
        int[] argmins = new int[columns];
        for (int c = 0; c < columns; c++) {
            argmins[c] = getColumn(c).argmin();
        }
        return argmins;
    }

    /** Return column-wise maximums. */
    public FloatMatrix columnMaxs() {
        FloatMatrix maxs = new FloatMatrix(1, columns);
        for (int c = 0; c < columns; c++) {
            maxs.put(c, getColumn(c).max());
        }
        return maxs;
    }

    /** Return index of minimal element per column. */
    public int[] columnArgmaxs() {
        int[] argmaxs = new int[columns];
        for (int c = 0; c < columns; c++) {
            argmaxs[c] = getColumn(c).argmax();
        }
        return argmaxs;
    }

    /** Return row-wise minimums. */
    public FloatMatrix rowMins() {
        FloatMatrix mins = new FloatMatrix(rows);
        for (int c = 0; c < rows; c++) {
            mins.put(c, getRow(c).min());
        }
        return mins;
    }

    /** Return index of minimal element per row. */
    public int[] rowArgmins() {
        int[] argmins = new int[rows];
        for (int c = 0; c < rows; c++) {
            argmins[c] = getRow(c).argmin();
        }
        return argmins;
    }

    /** Return row-wise maximums. */
    public FloatMatrix rowMaxs() {
        FloatMatrix maxs = new FloatMatrix(rows);
        for (int c = 0; c < rows; c++) {
            maxs.put(c, getRow(c).max());
        }
        return maxs;
    }

    /** Return index of minimal element per row. */
    public int[] rowArgmaxs() {
        int[] argmaxs = new int[rows];
        for (int c = 0; c < rows; c++) {
            argmaxs[c] = getRow(c).argmax();
        }
        return argmaxs;
    }

    /**************************************************************************
     * Elementwise Functions
     */
    /** Add a row vector to all rows of the matrix (in place). */
    public FloatMatrix addiRowVector(FloatMatrix x) {
        x.checkLength(columns);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) + x.get(c));
            }
        }
        return this;
    }

    /** Add a row to all rows of the matrix. */
    public FloatMatrix addRowVector(FloatMatrix x) {
        return dup().addiRowVector(x);
    }

    /** Add a vector to all columns of the matrix (in-place). */
    public FloatMatrix addiColumnVector(FloatMatrix x) {
        x.checkLength(rows);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) + x.get(r));
            }
        }
        return this;
    }

    /** Add a vector to all columns of the matrix. */
    public FloatMatrix addColumnVector(FloatMatrix x) {
        return dup().addiColumnVector(x);
    }

    /** Subtract a row vector from all rows of the matrix (in-place). */
    public FloatMatrix subiRowVector(FloatMatrix x) {
        // This is a bit crazy, but a row vector must have as length as the columns of the matrix.
        x.checkLength(columns);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) - x.get(c));
            }
        }
        return this;
    }

    /** Subtract a row vector from all rows of the matrix. */
    public FloatMatrix subRowVector(FloatMatrix x) {
        return dup().subiRowVector(x);
    }

    /** Subtract a column vector from all columns of the matrix (in-place). */
    public FloatMatrix subiColumnVector(FloatMatrix x) {
        x.checkLength(rows);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) - x.get(r));
            }
        }
        return this;
    }

    /** Subtract a vector from all columns of the matrix. */
    public FloatMatrix subColumnVector(FloatMatrix x) {
        return dup().subiColumnVector(x);
    }

    /** Multiply a row by a scalar. */
    public FloatMatrix mulRow(int r, float scale) {
        NativeBlas.sscal(columns, scale, data, index(r, 0), rows);
        return this;
    }

    /** Multiply a column by a scalar. */
    public FloatMatrix mulColumn(int c, float scale) {
        NativeBlas.sscal(rows, scale, data, index(0, c), 1);
        return this;
    }

    /** Multiply all columns with a column vector (in-place). */
    public FloatMatrix muliColumnVector(FloatMatrix x) {
        x.checkLength(rows);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) * x.get(r));
            }
        }
        return this;
    }

    /** Multiply all columns with a column vector. */
    public FloatMatrix mulColumnVector(FloatMatrix x) {
        return dup().muliColumnVector(x);
    }

    /** Multiply all rows with a row vector (in-place). */
    public FloatMatrix muliRowVector(FloatMatrix x) {
        x.checkLength(columns);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) * x.get(c));
            }
        }
        return this;
    }

    /** Multiply all rows with a row vector. */
    public FloatMatrix mulRowVector(FloatMatrix x) {
        return dup().muliRowVector(x);
    }

    public FloatMatrix diviRowVector(FloatMatrix x) {
        x.checkLength(columns);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) / x.get(c));
            }
        }
        return this;
    }

    public FloatMatrix divRowVector(FloatMatrix x) {
        return dup().diviRowVector(x);
    }

    public FloatMatrix diviColumnVector(FloatMatrix x) {
        x.checkLength(rows);
        for (int c = 0; c < columns; c++) {
            for (int r = 0; r < rows; r++) {
                put(r, c, get(r, c) / x.get(r));
            }
        }
        return this;
    }

    public FloatMatrix divColumnVector(FloatMatrix x) {
        return dup().diviColumnVector(x);
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
        for (int i = 0; i < data.length; i++) {
            dos.writeDouble(data[i]);
        }
    }

    /**
     * Reads in a matrix from the given data stream. Note
     * that the old data of this matrix will be discarded.
     * @param dis the data input stream to read from.
     * @throws IOException
     */
    public void in(DataInputStream dis) throws IOException {
        if (!dis.readUTF().equals("float")) {
            throw new IllegalStateException("The matrix in the specified file is not of the correct type!");
        }

        this.columns = dis.readInt();
        this.rows = dis.readInt();

        final int MAX = dis.readInt();
        data = new float[MAX];
        for (int i = 0; i < MAX; i++) {
            data[i] = dis.readFloat();
        }
    }

    /**
     * Saves this matrix to the specified file.
     * @param filename the file to write the matrix in.
     * @throws IOException thrown on errors while writing the matrix to the file
     */
    public void save(String filename) throws IOException {
        DataOutputStream dos = new DataOutputStream(new FileOutputStream(filename, false));
        this.out(dos);
        dos.close();
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
        dis.close();
    }

    public static FloatMatrix loadAsciiFile(String filename) throws IOException {
        BufferedReader is = new BufferedReader(new InputStreamReader(new FileInputStream(filename)));

        // Go through file and count columns and rows. What makes this endeavour a bit difficult is
        // that files can have leading or trailing spaces leading to spurious fields
        // after String.split().
        String line;
        int rows = 0;
        int columns = -1;
        while ((line = is.readLine()) != null) {
            String[] elements = line.split("\\s+");
            int numElements = elements.length;
            if (elements[0].length() == 0) {
                numElements--;
            }
            if (elements[elements.length - 1].length() == 0) {
                numElements--;
            }

            if (columns == -1) {
                columns = numElements;
            } else {
                if (columns != numElements) {
                    throw new IOException("Number of elements changes in line " + line + ".");
                }
            }

            rows++;
        }
        is.close();

        // Go through file a second time process the actual data.
        is = new BufferedReader(new InputStreamReader(new FileInputStream(filename)));
        FloatMatrix result = new FloatMatrix(rows, columns);
        int r = 0;
        while ((line = is.readLine()) != null) {
            String[] elements = line.split("\\s+");
            int firstElement = (elements[0].length() == 0) ? 1 : 0;
            for (int c = 0, cc = firstElement; c < columns; c++, cc++) {
                result.put(r, c, Float.valueOf(elements[cc]));
            }
            r++;
        }
        return result;
    }

    public static FloatMatrix loadCSVFile(String filename) throws IOException {
        BufferedReader is = new BufferedReader(new InputStreamReader(new FileInputStream(filename)));

        List<FloatMatrix> rows = new LinkedList<FloatMatrix>();
        String line;
        int columns = -1;
        while ((line = is.readLine()) != null) {
            String[] elements = line.split(",");
            int numElements = elements.length;
            if (elements[0].length() == 0) {
                numElements--;
            }
            if (elements[elements.length - 1].length() == 0) {
                numElements--;
            }

            if (columns == -1) {
                columns = numElements;
            } else {
                if (columns != numElements) {
                    throw new IOException("Number of elements changes in line " + line + ".");
                }
            }

            FloatMatrix row = new FloatMatrix(columns);
            for (int c = 0; c < columns; c++) {
                row.put(c, Float.valueOf(elements[c]));
            }
            rows.add(row);
        }
        is.close();

        System.out.println("Done reading file");

        FloatMatrix result = new FloatMatrix(rows.size(), columns);
        int r = 0;
        Iterator<FloatMatrix> ri = rows.iterator();
        while (ri.hasNext()) {
            result.putRow(r, ri.next());
            r++;
        }
        return result;
    }

    /****************************************************************
     * Autogenerated code
     */
    /***** Code for operators ***************************************/

    /* Overloads for the usual arithmetic operations */
    /*#
    def gen_overloads(base, result_rows, result_cols, verb=''); <<-EOS
    #{doc verb.capitalize + " a matrix (in place)."}
    public FloatMatrix #{base}i(FloatMatrix other) {
    return #{base}i(other, this);
    }

    #{doc verb.capitalize + " a matrix (in place)."}
    public FloatMatrix #{base}(FloatMatrix other) {
    return #{base}i(other, new FloatMatrix(#{result_rows}, #{result_cols}));
    }

    #{doc verb.capitalize + " a scalar (in place)."}
    public FloatMatrix #{base}i(float v) {
    return #{base}i(v, this);
    }

    #{doc verb.capitalize + " a scalar."}
    public FloatMatrix #{base}(float v) {
    return #{base}i(v, new FloatMatrix(rows, columns));
    }
    EOS
    end
    #*/

    /* Generating code for logical operators. This not only generates the stubs
     * but really all of the code.
     */
    /*#
    def gen_compare(name, op, cmp); <<-EOS
    #{doc 'Test for ' + cmp + ' (in-place).'}
    public FloatMatrix #{name}i(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return #{name}i(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) #{op} other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    #{doc 'Test for ' + cmp + ' (in-place).'}
    public FloatMatrix #{name}i(FloatMatrix other) {
    return #{name}i(other, this);
    }

    #{doc 'Test for ' + cmp + '.'}
    public FloatMatrix #{name}(FloatMatrix other) {
    return #{name}i(other, new FloatMatrix(rows, columns));
    }

    #{doc 'Test for ' + cmp + ' against a scalar (in-place).'}
    public FloatMatrix #{name}i(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) #{op} value ? 1.0f : 0.0f);
    return result;
    }

    #{doc 'Test for ' + cmp + ' against a scalar (in-place).'}
    public FloatMatrix #{name}i(float value) {
    return #{name}i(value, this);
    }

    #{doc 'test for ' + cmp + ' against a scalar.'}
    public FloatMatrix #{name}(float value) {
    return #{name}i(value, new FloatMatrix(rows, columns));
    }
    EOS
    end
    #*/
    /*#
    def gen_logical(name, op, cmp); <<-EOS
    #{doc 'Compute elementwise ' + cmp + ' (in-place).'}
    public FloatMatrix #{name}i(FloatMatrix other, FloatMatrix result) {
    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) #{op} (other.get(i) != 0.0f) ? 1.0f : 0.0f);
    return result;
    }

    #{doc 'Compute elementwise ' + cmp + ' (in-place).'}
    public FloatMatrix #{name}i(FloatMatrix other) {
    return #{name}i(other, this);
    }

    #{doc 'Compute elementwise ' + cmp + '.'}
    public FloatMatrix #{name}(FloatMatrix other) {
    return #{name}i(other, new FloatMatrix(rows, columns));
    }

    #{doc 'Compute elementwise ' + cmp + ' against a scalar (in-place).'}
    public FloatMatrix #{name}i(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    boolean val = (value != 0.0f);
    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) #{op} val ? 1.0f : 0.0f);
    return result;
    }

    #{doc 'Compute elementwise ' + cmp + ' against a scalar (in-place).'}
    public FloatMatrix #{name}i(float value) {
    return #{name}i(value, this);
    }

    #{doc 'Compute elementwise ' + cmp + ' against a scalar.'}
    public FloatMatrix #{name}(float value) {
    return #{name}i(value, new FloatMatrix(rows, columns));
    }
    EOS
    end
    #*/

    /*# collect(gen_overloads('add', 'rows', 'columns', 'add'),
    gen_overloads('sub', 'rows', 'columns', 'subtract'),
    gen_overloads('rsub', 'rows', 'columns', '(right-)subtract'),
    gen_overloads('div', 'rows', 'columns', 'elementwise divide by'),
    gen_overloads('rdiv', 'rows', 'columns', '(right-)elementwise divide by'),
    gen_overloads('mul', 'rows', 'columns', 'elementwise multiply by'),
    gen_overloads('mmul', 'rows', 'other.columns', 'matrix-multiply by'),
    gen_compare('lt', '<', '"less than"'),
    gen_compare('gt', '>', '"greater than"'),
    gen_compare('le', '<=', '"less than or equal"'),
    gen_compare('ge', '>=', '"greater than or equal"'),
    gen_compare('eq', '==', 'equality'),
    gen_compare('ne', '!=', 'inequality'),
    gen_logical('and', '&', 'logical and'),
    gen_logical('or', '|', 'logical or'),
    gen_logical('xor', '^', 'logical xor'))
    #*/
//RJPP-BEGIN------------------------------------------------------------
    /** Add a matrix (in place). */
    public FloatMatrix addi(FloatMatrix other) {
    return addi(other, this);
    }

    /** Add a matrix (in place). */
    public FloatMatrix add(FloatMatrix other) {
    return addi(other, new FloatMatrix(rows, columns));
    }

    /** Add a scalar (in place). */
    public FloatMatrix addi(float v) {
    return addi(v, this);
    }

    /** Add a scalar. */
    public FloatMatrix add(float v) {
    return addi(v, new FloatMatrix(rows, columns));
    }

    /** Subtract a matrix (in place). */
    public FloatMatrix subi(FloatMatrix other) {
    return subi(other, this);
    }

    /** Subtract a matrix (in place). */
    public FloatMatrix sub(FloatMatrix other) {
    return subi(other, new FloatMatrix(rows, columns));
    }

    /** Subtract a scalar (in place). */
    public FloatMatrix subi(float v) {
    return subi(v, this);
    }

    /** Subtract a scalar. */
    public FloatMatrix sub(float v) {
    return subi(v, new FloatMatrix(rows, columns));
    }

    /** (right-)subtract a matrix (in place). */
    public FloatMatrix rsubi(FloatMatrix other) {
    return rsubi(other, this);
    }

    /** (right-)subtract a matrix (in place). */
    public FloatMatrix rsub(FloatMatrix other) {
    return rsubi(other, new FloatMatrix(rows, columns));
    }

    /** (right-)subtract a scalar (in place). */
    public FloatMatrix rsubi(float v) {
    return rsubi(v, this);
    }

    /** (right-)subtract a scalar. */
    public FloatMatrix rsub(float v) {
    return rsubi(v, new FloatMatrix(rows, columns));
    }

    /** Elementwise divide by a matrix (in place). */
    public FloatMatrix divi(FloatMatrix other) {
    return divi(other, this);
    }

    /** Elementwise divide by a matrix (in place). */
    public FloatMatrix div(FloatMatrix other) {
    return divi(other, new FloatMatrix(rows, columns));
    }

    /** Elementwise divide by a scalar (in place). */
    public FloatMatrix divi(float v) {
    return divi(v, this);
    }

    /** Elementwise divide by a scalar. */
    public FloatMatrix div(float v) {
    return divi(v, new FloatMatrix(rows, columns));
    }

    /** (right-)elementwise divide by a matrix (in place). */
    public FloatMatrix rdivi(FloatMatrix other) {
    return rdivi(other, this);
    }

    /** (right-)elementwise divide by a matrix (in place). */
    public FloatMatrix rdiv(FloatMatrix other) {
    return rdivi(other, new FloatMatrix(rows, columns));
    }

    /** (right-)elementwise divide by a scalar (in place). */
    public FloatMatrix rdivi(float v) {
    return rdivi(v, this);
    }

    /** (right-)elementwise divide by a scalar. */
    public FloatMatrix rdiv(float v) {
    return rdivi(v, new FloatMatrix(rows, columns));
    }

    /** Elementwise multiply by a matrix (in place). */
    public FloatMatrix muli(FloatMatrix other) {
    return muli(other, this);
    }

    /** Elementwise multiply by a matrix (in place). */
    public FloatMatrix mul(FloatMatrix other) {
    return muli(other, new FloatMatrix(rows, columns));
    }

    /** Elementwise multiply by a scalar (in place). */
    public FloatMatrix muli(float v) {
    return muli(v, this);
    }

    /** Elementwise multiply by a scalar. */
    public FloatMatrix mul(float v) {
    return muli(v, new FloatMatrix(rows, columns));
    }

    /** Matrix-multiply by a matrix (in place). */
    public FloatMatrix mmuli(FloatMatrix other) {
    return mmuli(other, this);
    }

    /** Matrix-multiply by a matrix (in place). */
    public FloatMatrix mmul(FloatMatrix other) {
    return mmuli(other, new FloatMatrix(rows, other.columns));
    }

    /** Matrix-multiply by a scalar (in place). */
    public FloatMatrix mmuli(float v) {
    return mmuli(v, this);
    }

    /** Matrix-multiply by a scalar. */
    public FloatMatrix mmul(float v) {
    return mmuli(v, new FloatMatrix(rows, columns));
    }

    /** Test for "less than" (in-place). */
    public FloatMatrix lti(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return lti(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) < other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "less than" (in-place). */
    public FloatMatrix lti(FloatMatrix other) {
    return lti(other, this);
    }

    /** Test for "less than". */
    public FloatMatrix lt(FloatMatrix other) {
    return lti(other, new FloatMatrix(rows, columns));
    }

    /** Test for "less than" against a scalar (in-place). */
    public FloatMatrix lti(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) < value ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "less than" against a scalar (in-place). */
    public FloatMatrix lti(float value) {
    return lti(value, this);
    }

    /** test for "less than" against a scalar. */
    public FloatMatrix lt(float value) {
    return lti(value, new FloatMatrix(rows, columns));
    }

    /** Test for "greater than" (in-place). */
    public FloatMatrix gti(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return gti(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) > other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "greater than" (in-place). */
    public FloatMatrix gti(FloatMatrix other) {
    return gti(other, this);
    }

    /** Test for "greater than". */
    public FloatMatrix gt(FloatMatrix other) {
    return gti(other, new FloatMatrix(rows, columns));
    }

    /** Test for "greater than" against a scalar (in-place). */
    public FloatMatrix gti(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) > value ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "greater than" against a scalar (in-place). */
    public FloatMatrix gti(float value) {
    return gti(value, this);
    }

    /** test for "greater than" against a scalar. */
    public FloatMatrix gt(float value) {
    return gti(value, new FloatMatrix(rows, columns));
    }

    /** Test for "less than or equal" (in-place). */
    public FloatMatrix lei(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return lei(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) <= other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "less than or equal" (in-place). */
    public FloatMatrix lei(FloatMatrix other) {
    return lei(other, this);
    }

    /** Test for "less than or equal". */
    public FloatMatrix le(FloatMatrix other) {
    return lei(other, new FloatMatrix(rows, columns));
    }

    /** Test for "less than or equal" against a scalar (in-place). */
    public FloatMatrix lei(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) <= value ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "less than or equal" against a scalar (in-place). */
    public FloatMatrix lei(float value) {
    return lei(value, this);
    }

    /** test for "less than or equal" against a scalar. */
    public FloatMatrix le(float value) {
    return lei(value, new FloatMatrix(rows, columns));
    }

    /** Test for "greater than or equal" (in-place). */
    public FloatMatrix gei(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return gei(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) >= other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "greater than or equal" (in-place). */
    public FloatMatrix gei(FloatMatrix other) {
    return gei(other, this);
    }

    /** Test for "greater than or equal". */
    public FloatMatrix ge(FloatMatrix other) {
    return gei(other, new FloatMatrix(rows, columns));
    }

    /** Test for "greater than or equal" against a scalar (in-place). */
    public FloatMatrix gei(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) >= value ? 1.0f : 0.0f);
    return result;
    }

    /** Test for "greater than or equal" against a scalar (in-place). */
    public FloatMatrix gei(float value) {
    return gei(value, this);
    }

    /** test for "greater than or equal" against a scalar. */
    public FloatMatrix ge(float value) {
    return gei(value, new FloatMatrix(rows, columns));
    }

    /** Test for equality (in-place). */
    public FloatMatrix eqi(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return eqi(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) == other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    /** Test for equality (in-place). */
    public FloatMatrix eqi(FloatMatrix other) {
    return eqi(other, this);
    }

    /** Test for equality. */
    public FloatMatrix eq(FloatMatrix other) {
    return eqi(other, new FloatMatrix(rows, columns));
    }

    /** Test for equality against a scalar (in-place). */
    public FloatMatrix eqi(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) == value ? 1.0f : 0.0f);
    return result;
    }

    /** Test for equality against a scalar (in-place). */
    public FloatMatrix eqi(float value) {
    return eqi(value, this);
    }

    /** test for equality against a scalar. */
    public FloatMatrix eq(float value) {
    return eqi(value, new FloatMatrix(rows, columns));
    }

    /** Test for inequality (in-place). */
    public FloatMatrix nei(FloatMatrix other, FloatMatrix result) {
    if (other.isScalar())
    return nei(other.scalar(), result);

    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, get(i) != other.get(i) ? 1.0f : 0.0f);
    return result;
    }

    /** Test for inequality (in-place). */
    public FloatMatrix nei(FloatMatrix other) {
    return nei(other, this);
    }

    /** Test for inequality. */
    public FloatMatrix ne(FloatMatrix other) {
    return nei(other, new FloatMatrix(rows, columns));
    }

    /** Test for inequality against a scalar (in-place). */
    public FloatMatrix nei(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    for (int i = 0; i < length; i++)
    result.put(i, get(i) != value ? 1.0f : 0.0f);
    return result;
    }

    /** Test for inequality against a scalar (in-place). */
    public FloatMatrix nei(float value) {
    return nei(value, this);
    }

    /** test for inequality against a scalar. */
    public FloatMatrix ne(float value) {
    return nei(value, new FloatMatrix(rows, columns));
    }

    /** Compute elementwise logical and (in-place). */
    public FloatMatrix andi(FloatMatrix other, FloatMatrix result) {
    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) & (other.get(i) != 0.0f) ? 1.0f : 0.0f);
    return result;
    }

    /** Compute elementwise logical and (in-place). */
    public FloatMatrix andi(FloatMatrix other) {
    return andi(other, this);
    }

    /** Compute elementwise logical and. */
    public FloatMatrix and(FloatMatrix other) {
    return andi(other, new FloatMatrix(rows, columns));
    }

    /** Compute elementwise logical and against a scalar (in-place). */
    public FloatMatrix andi(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    boolean val = (value != 0.0f);
    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) & val ? 1.0f : 0.0f);
    return result;
    }

    /** Compute elementwise logical and against a scalar (in-place). */
    public FloatMatrix andi(float value) {
    return andi(value, this);
    }

    /** Compute elementwise logical and against a scalar. */
    public FloatMatrix and(float value) {
    return andi(value, new FloatMatrix(rows, columns));
    }

    /** Compute elementwise logical or (in-place). */
    public FloatMatrix ori(FloatMatrix other, FloatMatrix result) {
    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) | (other.get(i) != 0.0f) ? 1.0f : 0.0f);
    return result;
    }

    /** Compute elementwise logical or (in-place). */
    public FloatMatrix ori(FloatMatrix other) {
    return ori(other, this);
    }

    /** Compute elementwise logical or. */
    public FloatMatrix or(FloatMatrix other) {
    return ori(other, new FloatMatrix(rows, columns));
    }

    /** Compute elementwise logical or against a scalar (in-place). */
    public FloatMatrix ori(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    boolean val = (value != 0.0f);
    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) | val ? 1.0f : 0.0f);
    return result;
    }

    /** Compute elementwise logical or against a scalar (in-place). */
    public FloatMatrix ori(float value) {
    return ori(value, this);
    }

    /** Compute elementwise logical or against a scalar. */
    public FloatMatrix or(float value) {
    return ori(value, new FloatMatrix(rows, columns));
    }

    /** Compute elementwise logical xor (in-place). */
    public FloatMatrix xori(FloatMatrix other, FloatMatrix result) {
    assertSameLength(other);
    ensureResultLength(other, result);

    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) ^ (other.get(i) != 0.0f) ? 1.0f : 0.0f);
    return result;
    }

    /** Compute elementwise logical xor (in-place). */
    public FloatMatrix xori(FloatMatrix other) {
    return xori(other, this);
    }

    /** Compute elementwise logical xor. */
    public FloatMatrix xor(FloatMatrix other) {
    return xori(other, new FloatMatrix(rows, columns));
    }

    /** Compute elementwise logical xor against a scalar (in-place). */
    public FloatMatrix xori(float value, FloatMatrix result) {
    ensureResultLength(null, result);
    boolean val = (value != 0.0f);
    for (int i = 0; i < length; i++)
    result.put(i, (get(i) != 0.0f) ^ val ? 1.0f : 0.0f);
    return result;
    }

    /** Compute elementwise logical xor against a scalar (in-place). */
    public FloatMatrix xori(float value) {
    return xori(value, this);
    }

    /** Compute elementwise logical xor against a scalar. */
    public FloatMatrix xor(float value) {
    return xori(value, new FloatMatrix(rows, columns));
    }
//RJPP-END--------------------------------------------------------------

    public ComplexFloatMatrix toComplex() {
      return new ComplexFloatMatrix(this);
    }
}