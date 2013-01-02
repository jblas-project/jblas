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
  public void lu() {
    DoubleMatrix A = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

    Decompose.LUDecomposition<DoubleMatrix> lu = Decompose.lu(A);

    assertEquals(0.0, (lu.p.mmul(lu.l).mmul(lu.u).sub(A).normmax()), 1e-10);

    assertTrue(lu.l.isLowerTriangular());
    assertTrue(lu.u.isUpperTriangular());
  }
}
