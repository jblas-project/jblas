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

import java.io.File;
import java.io.PrintStream;

import org.jblas.ranges.IntervalRange;
import org.jblas.util.Random;

import java.util.Arrays;

import static org.jblas.ranges.RangeUtils.*;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

public class TestFloatMatrix {

 private final float eps = 1e-6f;

  FloatMatrix A, B, C, D, E, F;

  @Before
  public void setUp() {
    A = new FloatMatrix(4, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f);
    B = new FloatMatrix(3, 1, 2.0f, 4.0f, 8.0f);
    C = new FloatMatrix(3, 1, -1.0f, 2.0f, -3.0f);
    D = new FloatMatrix(3, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    E = new FloatMatrix(3, 3, 1.0f, -2.0f, 3.0f, -4.0f, 5.0f, -6.0f, 7.0f, -8.0f, 9.0f);
    F = new FloatMatrix(3, 1, 3.0f, 4.0f, 7.0f);
  }

  @Test
  public void testConstructionAndSetGet() {
    float[][] dataA = {{1.0f, 5.0f, 9.0f}, {2.0f, 6.0f, 10.0f}, {3.0f, 7.0f, 11.0f}, {4.0f, 8.0f, 12.0f}};

    assertEquals(A.rows, 4);
    assertEquals(A.columns, 3);

    for (int r = 0; r < 4; r++) {
      for (int c = 0; c < 3; c++) {
        assertEquals(dataA[r][c], A.get(r, c), eps);
      }
    }
  }

  @Test
  public void testSetAndGet() {
    FloatMatrix M = new FloatMatrix(3, 3);

    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        M.put(i, j, i + j);
      }
    }
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        assertEquals((float) i + j, M.get(i, j), eps);
      }
    }
  }

  @Test
  public void testGetWithRowIndicesAndSingleColumn() {
    FloatMatrix M = new FloatMatrix(new float[][] {{1, 2}, {3, 4}});

    assertEquals(new FloatMatrix(2, 1, 1, 3), M.get(new int[]{0, 1}, 0));
    assertEquals(new FloatMatrix(2, 1, 2, 4), M.get(new int[]{0, 1}, 1));
  }

  @Test
  public void testCopy() {
    FloatMatrix M = new FloatMatrix();

    assertFalse(M.equals(A));

    M.copy(A);
    assertEquals(M, A);
  }

  @Test
  public void testDup() {
    FloatMatrix M = A.dup();
    assertEquals(M, A);

    M.put(0, 0, 2.0f);
    assertFalse(M.equals(A));
  }

  @Test
  public void testResize() {
    FloatMatrix M = A.dup();

    assertEquals(4, M.rows);
    assertEquals(3, M.columns);

    M.resize(4, 5);
    assertEquals(4, M.rows);
    assertEquals(5, M.columns);

    assertEquals(0.0f, M.get(3, 4), eps);
  }

  @Test
  public void testReshape() {
    FloatMatrix M = new FloatMatrix(2, 2, 1.0f, 2.0f, 3.0f, 4.0f);

    M.reshape(1, 4);
    assertEquals(1.0f, M.get(0, 0), eps);
    assertEquals(4.0f, M.get(0, 3), eps);

    M.reshape(4, 1);
    assertEquals(1.0f, M.get(0, 0), eps);
    assertEquals(4.0f, M.get(3, 0), eps);
  }

  @Test
  public void testMmul() {
    FloatMatrix R = A.dup();
    FloatMatrix result = new FloatMatrix(4, 1, 94.0f, 108.0f, 122.0f, 136.0f);

    A.mmuli(B, R);
    assertEquals(result, R);

    assertEquals(result, A.mmul(B));

    FloatMatrix resultDE = new FloatMatrix(3, 3, 14.0f, 16.0f, 18.0f, -26.0f, -31.0f, -36.0f, 38.0f, 46.0f, 54.0f);

    // In-place with independent operands
    assertEquals(resultDE, D.mmuli(E, R));

    // In-place on this
    R = D.dup();
    assertEquals(resultDE, R.mmuli(E, R));

    // In-place on this
    R = E.dup();
    assertEquals(resultDE, D.mmuli(R, R));

    // Fully dynamic
    assertEquals(resultDE, D.mmul(E));
  }

  @Test
  public void testAdd() {
    FloatMatrix result = new FloatMatrix(3, 1, 1.0f, 6.0f, 5.0f);

    FloatMatrix R = new FloatMatrix();

    // In-place, but independent operands
    B.addi(C, R);
    assertEquals(result, R);

    // In-place on this
    R = B.dup();
    assertEquals(result, R.addi(C, R));

    // In-place on other
    R = C.dup();
    assertEquals(result, B.addi(R, R));

    // fully dynamic
    assertEquals(result, B.add(C));

    result = new FloatMatrix(3, 1, 3.0f, 5.0f, 9.0f);

    // In-place, but independent operands
    assertEquals(result, B.addi(1.0f, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.addi(1.0f, R));

    // fully dynamic
    assertEquals(result, B.add(1.0f));
  }

  @Test
  public void testSub() {
    FloatMatrix result = new FloatMatrix(3, 1, 3.0f, 2.0f, 11.0f);

    FloatMatrix R = new FloatMatrix();

    // In-place, but independent operands
    assertEquals(result, B.subi(C, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.subi(C, R));

    // In-place on other
    R = C.dup();
    assertEquals(result, B.subi(R, R));

    // fully dynamic
    assertEquals(result, B.sub(C));

    result = new FloatMatrix(3, 1, 1.0f, 3.0f, 7.0f);

    // In-place, but independent operands
    assertEquals(result, B.subi(1.0f, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.subi(1.0f, R));

    // fully dynamic
    assertEquals(result, B.sub(1.0f));
  }

  @Test
  public void testRsub() {
    FloatMatrix result = new FloatMatrix(3, 1, 3.0f, 2.0f, 11.0f);

    FloatMatrix R = new FloatMatrix();

    // In-place, but independent operands
    assertEquals(result, C.rsubi(B, R));

    // In-place on this
    R = C.dup();
    assertEquals(result, R.rsubi(B, R));

    // In-place on other
    R = B.dup();
    assertEquals(result, C.rsubi(R, R));

    // fully dynamic
    assertEquals(result, C.rsub(B));

    result = new FloatMatrix(3, 1, -1.0f, -3.0f, -7.0f);

    // In-place, but independent operands
    assertEquals(result, B.rsubi(1.0f, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.rsubi(1.0f, R));

    // fully dynamic
    assertEquals(result, B.rsub(1.0f));
  }

  @Test
  public void testMul() {
    FloatMatrix result = new FloatMatrix(3, 1, -2.0f, 8.0f, -24.0f);

    FloatMatrix R = new FloatMatrix();

    // In-place, but independent operands
    assertEquals(result, B.muli(C, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.muli(C, R));

    // In-place on other
    R = C.dup();
    assertEquals(result, B.muli(R, R));

    // fully dynamic
    assertEquals(result, B.mul(C));

    result = new FloatMatrix(3, 1, 1.0f, 2.0f, 4.0f);

    // In-place, but independent operands
    assertEquals(result, B.muli(0.5f, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.muli(0.5f, R));

    // fully dynamic
    assertEquals(result, B.mul(0.5f));
  }

  @Test
  public void testDiv() {
    FloatMatrix result = new FloatMatrix(3, 1, -2.0f, 2.0f, -2.666666666f);

    FloatMatrix R = new FloatMatrix();

    // In-place, but independent operands
    assertEquals(result, B.divi(C, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.divi(C, R));

    // In-place on other
    R = C.dup();
    assertEquals(result, B.divi(R, R));

    // fully dynamic
    assertEquals(result, B.div(C));

    result = new FloatMatrix(3, 1, 1.0f, 2.0f, 4.0f);

    // In-place, but independent operands
    assertEquals(result, B.divi(2.0f, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.divi(2.0f, R));

    // fully dynamic
    assertEquals(result, B.div(2.0f));
  }

  @Test
  public void testRdiv() {
    FloatMatrix result = new FloatMatrix(3, 1, -2.0f, 2.0f, -2.666666666f);

    FloatMatrix R = new FloatMatrix();

    // In-place, but independent operands
    assertEquals(result, C.rdivi(B, R));

    // In-place on this
    R = C.dup();
    assertEquals(result, R.rdivi(B, R));

    // In-place on other
    R = B.dup();
    assertEquals(result, C.rdivi(R, R));

    // fully dynamic
    assertEquals(result, C.rdiv(B));

    result = new FloatMatrix(3, 1, 0.5f, 0.25f, 0.125f);

    // In-place, but independent operands
    assertEquals(result, B.rdivi(1.0f, R));

    // In-place on this
    R = B.dup();
    assertEquals(result, R.rdivi(1.0f, R));

    // fully dynamic
    assertEquals(result, B.rdiv(1.0f));
  }

  /*# def test_logical(op, result, scalar, result2); <<-EOS
  @Test
  public void test#{op.upcase}() {
  FloatMatrix result = new FloatMatrix(3, 1, #{result});
  FloatMatrix result2 = new FloatMatrix(3, 1, #{result2});
  FloatMatrix R = new FloatMatrix();

  // in-place but independent operands
  assertEquals(result, B.#{op}i(F, R));

  // in-place but in other
  R = F.dup();
  assertEquals(result, B.#{op}i(R, R));

  // in-place in this
  R = B.dup();
  assertEquals(result, R.#{op}i(F, R));

  // fully dynamic
  assertEquals(result, B.#{op}(F));

  // in-place in this
  R = B.dup();
  assertEquals(result2, R.#{op}i(#{scalar}, R));

  // fully dynamic
  assertEquals(result2, B.#{op}(#{scalar}));
  }
  EOS
  end
  #*/
  /*# test_logical('lt', '1.0f, 0.0f, 0.0f', 4.0f, '1.0f, 0.0f, 0.0f') #*/
//RJPP-BEGIN------------------------------------------------------------
  @Test
  public void testLT() {
  FloatMatrix result = new FloatMatrix(3, 1, 1.0f, 0.0f, 0.0f);
  FloatMatrix result2 = new FloatMatrix(3, 1, 1.0f, 0.0f, 0.0f);
  FloatMatrix R = new FloatMatrix();

  // in-place but independent operands
  assertEquals(result, B.lti(F, R));

  // in-place but in other
  R = F.dup();
  assertEquals(result, B.lti(R, R));

  // in-place in this
  R = B.dup();
  assertEquals(result, R.lti(F, R));

  // fully dynamic
  assertEquals(result, B.lt(F));

  // in-place in this
  R = B.dup();
  assertEquals(result2, R.lti(4.0f, R));

  // fully dynamic
  assertEquals(result2, B.lt(4.0f));
  }
//RJPP-END--------------------------------------------------------------
  /*# test_logical('le', '1.0f, 1.0f, 0.0f', 4.0f, '1.0f, 1.0f, 0.0f') #*/
//RJPP-BEGIN------------------------------------------------------------
  @Test
  public void testLE() {
  FloatMatrix result = new FloatMatrix(3, 1, 1.0f, 1.0f, 0.0f);
  FloatMatrix result2 = new FloatMatrix(3, 1, 1.0f, 1.0f, 0.0f);
  FloatMatrix R = new FloatMatrix();

  // in-place but independent operands
  assertEquals(result, B.lei(F, R));

  // in-place but in other
  R = F.dup();
  assertEquals(result, B.lei(R, R));

  // in-place in this
  R = B.dup();
  assertEquals(result, R.lei(F, R));

  // fully dynamic
  assertEquals(result, B.le(F));

  // in-place in this
  R = B.dup();
  assertEquals(result2, R.lei(4.0f, R));

  // fully dynamic
  assertEquals(result2, B.le(4.0f));
  }
//RJPP-END--------------------------------------------------------------
  /*# test_logical('gt', '0.0f, 0.0f, 1.0f', 4.0f, '0.0f, 0.0f, 1.0f') #*/
//RJPP-BEGIN------------------------------------------------------------
  @Test
  public void testGT() {
  FloatMatrix result = new FloatMatrix(3, 1, 0.0f, 0.0f, 1.0f);
  FloatMatrix result2 = new FloatMatrix(3, 1, 0.0f, 0.0f, 1.0f);
  FloatMatrix R = new FloatMatrix();

  // in-place but independent operands
  assertEquals(result, B.gti(F, R));

  // in-place but in other
  R = F.dup();
  assertEquals(result, B.gti(R, R));

  // in-place in this
  R = B.dup();
  assertEquals(result, R.gti(F, R));

  // fully dynamic
  assertEquals(result, B.gt(F));

  // in-place in this
  R = B.dup();
  assertEquals(result2, R.gti(4.0f, R));

  // fully dynamic
  assertEquals(result2, B.gt(4.0f));
  }
//RJPP-END--------------------------------------------------------------
  /*# test_logical('ge', '0.0f, 1.0f, 1.0f', 4.0f, '0.0f, 1.0f, 1.0f') #*/
//RJPP-BEGIN------------------------------------------------------------
  @Test
  public void testGE() {
  FloatMatrix result = new FloatMatrix(3, 1, 0.0f, 1.0f, 1.0f);
  FloatMatrix result2 = new FloatMatrix(3, 1, 0.0f, 1.0f, 1.0f);
  FloatMatrix R = new FloatMatrix();

  // in-place but independent operands
  assertEquals(result, B.gei(F, R));

  // in-place but in other
  R = F.dup();
  assertEquals(result, B.gei(R, R));

  // in-place in this
  R = B.dup();
  assertEquals(result, R.gei(F, R));

  // fully dynamic
  assertEquals(result, B.ge(F));

  // in-place in this
  R = B.dup();
  assertEquals(result2, R.gei(4.0f, R));

  // fully dynamic
  assertEquals(result2, B.ge(4.0f));
  }
//RJPP-END--------------------------------------------------------------
  @Test
  public void testMinMax() {
    assertEquals(1.0f, A.min(), eps);
    assertEquals(12.0f, A.max(), eps);
  }

  @Test
  public void testArgMinMax() {
    assertEquals(0, A.argmin(), eps);
    assertEquals(11, A.argmax(), eps);
  }

  @Test
  public void testTranspose() {
    FloatMatrix At = A.transpose();
    assertEquals(1.0f, At.get(0, 0), eps);
    assertEquals(2.0f, At.get(0, 1), eps);
    assertEquals(5.0f, At.get(1, 0), eps);
  }

  @Test
  public void testGetRowVector() {
    for (int r = 0; r < A.rows; r++) {
      A.getRow(r);
    }

    for (int c = 0; c < A.columns; c++) {
      A.getColumn(c);
    }

    A.addiRowVector(new FloatMatrix(3, 1, 10.0f, 100.0f, 1000.0f));
    A.addiColumnVector(new FloatMatrix(1, 4, 10.0f, 100.0f, 1000.0f, 10000.0f));
  }

  @Test
  public void testPairwiseDistance() {
    FloatMatrix D = Geometry.pairwiseSquaredDistances(A, A);

    FloatMatrix X = new FloatMatrix(1, 3, 1.0f, 0.0f, -1.0f);

    Geometry.pairwiseSquaredDistances(X, X);

    FloatMatrix A1 = new FloatMatrix(1, 2, 1.0f, 2.0f);
    FloatMatrix A2 = new FloatMatrix(1, 3, 1.0f, 2.0f, 3.0f);

    Geometry.pairwiseSquaredDistances(A1, A2);
  }

  @Test
  public void testSwapColumns() {
    FloatMatrix AA = A.dup();

    AA.swapColumns(1, 2);
    assertEquals(new FloatMatrix(4, 3, 1.0f, 2.0f, 3.0f, 4.0f, 9.0f, 10.0f, 11.0f, 12.0f, 5.0f, 6.0f, 7.0f, 8.0f), AA);
  }

  @Test
  public void testSwapRows() {
    FloatMatrix AA = A.dup();

    AA.swapRows(1, 2);
    assertEquals(new FloatMatrix(4, 3, 1.0f, 3.0f, 2.0f, 4.0f, 5.0f, 7.0f, 6.0f, 8.0f, 9.0f, 11.0f, 10.0f, 12.0f), AA);
  }

  @Test
  public void testSolve() {
    FloatMatrix AA = new FloatMatrix(3, 3, 3.0f, 5.0f, 6.0f, 1.0f, 0.0f, 0.0f, 2.0f, 4.0f, 0.0f);
    FloatMatrix BB = new FloatMatrix(3, 1, 1.0f, 2.0f, 3.0f);

    FloatMatrix Adup = AA.dup();
    FloatMatrix Bdup = BB.dup();

    FloatMatrix X = Solve.solve(AA, BB);

    assertEquals(Adup, AA);
    assertEquals(Bdup, BB);
  }

  @Test
  public void testConstructFromArray() {
    float[][] data = {
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    };

    FloatMatrix A = new FloatMatrix(data);

    for (int r = 0; r < 3; r++) {
      for (int c = 0; c < 3; c++) {
        assertEquals(data[r][c], A.get(r, c), eps);
      }
    }
  }

  @Test
  public void testDiag() {
    FloatMatrix A = new FloatMatrix(new float[][]{
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
        {7.0f, 8.0f, 9.0f}
    });

    assertEquals(new FloatMatrix(3, 1, 1.0f, 5.0f, 9.0f), A.diag());

    assertEquals(new FloatMatrix(new float[][]{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 2.0f, 0.0f},
        {0.0f, 0.0f, 3.0f}
    }), FloatMatrix.diag(new FloatMatrix(3, 1, 1.0f, 2.0f, 3.0f)));
  }

  @Test
  public void testColumnAndRowMinMax() {
    assertEquals(new FloatMatrix(1, 3, 1.0f, 5.0f, 9.0f), A.columnMins());
    assertEquals(new FloatMatrix(4, 1, 1.0f, 2.0f, 3.0f, 4.0f), A.rowMins());
    assertEquals(new FloatMatrix(1, 3, 4.0f, 8.0f, 12.0f), A.columnMaxs());
    assertEquals(new FloatMatrix(4, 1, 9.0f, 10.0f, 11.0f, 12.0f), A.rowMaxs());
    int[] i = A.columnArgmins();
    assertEquals(0, i[0]);
    assertEquals(0, i[1]);
    assertEquals(0, i[2]);
    i = A.columnArgmaxs();
    assertEquals(3, i[0]);
    assertEquals(3, i[1]);
    assertEquals(3, i[2]);
    i = A.rowArgmins();
    assertEquals(0, i[0]);
    assertEquals(0, i[1]);
    assertEquals(0, i[2]);
    assertEquals(0, i[3]);
    i = A.rowArgmaxs();
    assertEquals(2, i[0]);
    assertEquals(2, i[1]);
    assertEquals(2, i[2]);
    assertEquals(2, i[3]);
  }

  @Test
  public void testToArray() {
    assertTrue(Arrays.equals(new float[]{2.0f, 4.0f, 8.0f}, B.toArray()));
    assertTrue(Arrays.equals(new int[]{2, 4, 8}, B.toIntArray()));
    assertTrue(Arrays.equals(new boolean[]{true, true, true}, B.toBooleanArray()));
  }

  @Test
  public void testLoadAsciiFile() {
    try {
      File f = File.createTempFile("jblas-test", "txt");
      f.deleteOnExit();
      PrintStream out = new PrintStream(f);
      out.println("1.0f 2.0f 3.0f");
      out.println("4.0f 5.0f 6.0f");
      out.close();

      FloatMatrix result = FloatMatrix.loadAsciiFile(f.getAbsolutePath());
      assertEquals(new FloatMatrix(2, 3, 1.0f, 4.0f, 2.0f, 5.0f, 3.0f, 6.0f), result);
    } catch (Exception e) {
      fail("Caught exception " + e);
    }
  }

  @Test
  public void testRanges() {
    FloatMatrix A = new FloatMatrix(3, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    FloatMatrix B = new FloatMatrix(2, 3, -1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f);

    A.put(interval(0, 2), interval(0, 3), B);

    /*assertEquals(-1.0f, A.get(0, 0));
  assertEquals(-2.0f, A.get(0, 1));
  assertEquals(-3.0f, A.get(0, 2));
  assertEquals(-4.0f, A.get(1, 0));
  assertEquals(-5.0f, A.get(1, 1));
  assertEquals(-6.0f, A.get(1, 2));*/
  }

  @Test
  public void testRandWithSeed() {
    Random.seed(1);
    FloatMatrix A = FloatMatrix.rand(3, 3);
    Random.seed(1);
    FloatMatrix B = FloatMatrix.rand(3, 3);
    assertEquals(0.0f, A.sub(B).normmax(), 1e-9);
  }

  @Test
  public void testToString() {
    // We have to be a bit cautious here because my Float => Float converter scripts will
    // add a "f" to every floating point number, even in the strings. Therefore, I
    // explicitly remove all "f"s
    assertEquals("[1.000000f, 5.000000f, 9.000000f; 2.000000f, 6.000000f, 10.000000f; 3.000000f, 7.000000f, 11.000000f; 4.000000f, 8.000000f, 12.000000f]".replaceAll("f", ""), A.toString());

    assertEquals("[1.0f, 5.0f, 9.0f; 2.0f, 6.0f, 10.0f; 3.0f, 7.0f, 11.0f; 4.0f, 8.0f, 12.0f]".replaceAll("f", ""), A.toString("%.1f"));

    assertEquals("{1.0f 5.0f 9.0f; 2.0f 6.0f 10.0f; 3.0f 7.0f 11.0f; 4.0f 8.0f 12.0f}".replaceAll("f", ""), A.toString("%.1f", "{", "}", " ", "; "));
  }

  @Test
  public void testGetRowsWithRanges() {
    FloatMatrix m = new FloatMatrix(new float[][]{{1,2,3},{4,5,6},{7,8,9},{10,11,12}});
    FloatMatrix m1 = m.getRows(new org.jblas.ranges.IntervalRange(0, 2));
    assertEquals("[1, 2, 3; 4, 5, 6]", m1.toString("%.0f"));

    FloatMatrix m2 = m.getRows(new org.jblas.ranges.IntervalRange(2, m.rows));
    assertEquals("[7, 8, 9; 10, 11, 12]", m2.toString("%.0f"));

    FloatMatrix m3 = m.getRows(new org.jblas.ranges.IndicesRange(new int[] {1, 3, 0}));
    assertEquals("[4, 5, 6; 10, 11, 12; 1, 2, 3]", m3.toString("%.0f"));

  }

  @Test
  public void testGetColumsWithRanges() {
    FloatMatrix m = new FloatMatrix(new float[][]{{ 1, 2, 3},
                                                     { 4, 5, 6},
                                                     { 7, 8, 9},
                                                     {10,11,12}});
    FloatMatrix m1 = m.getColumns(new org.jblas.ranges.IntervalRange(0, 2));
    assertEquals("[1, 2; 4, 5; 7, 8; 10, 11]", m1.toString("%.0f"));

    FloatMatrix m2 = m.getColumns(new org.jblas.ranges.IntervalRange(2, m.columns));
    assertEquals("[3; 6; 9; 12]", m2.toString("%.0f"));

    FloatMatrix m3 = m.getColumns(new org.jblas.ranges.IndicesRange(new int[] {0, 1, 0}));
    assertEquals("[1, 2, 1; 4, 5, 4; 7, 8, 7; 10, 11, 10]", m3.toString("%.0f"));
  }
}