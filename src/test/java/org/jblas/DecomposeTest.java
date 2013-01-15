package org.jblas;

import org.junit.*;
import static org.junit.Assert.*;

/**
 * Test class for Decompose
 *
 * @author Mikio Braun
 */
public class DecomposeTest {
  @Test
  public void luDouble() {
    DoubleMatrix A = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

    Decompose.LUDecomposition<DoubleMatrix> lu = Decompose.lu(A);

    assertEquals(0.0, (lu.p.mmul(lu.l).mmul(lu.u).sub(A).normmax()), 1e-10);

    assertTrue(lu.l.isLowerTriangular());
    assertTrue(lu.u.isUpperTriangular());
  }

  @Test
  public void luFloat() {
    FloatMatrix A = new FloatMatrix(3, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);

    Decompose.LUDecomposition<FloatMatrix> lu = Decompose.lu(A);

    assertEquals(0.0f, (lu.p.mmul(lu.l).mmul(lu.u).sub(A).normmax()), 1e-6f);

    assertTrue(lu.l.isLowerTriangular());
    assertTrue(lu.u.isUpperTriangular());
  }
}
