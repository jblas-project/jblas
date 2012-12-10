package org.jblas;

import org.junit.*;
import static org.junit.Assert.*;

/**
 * Test cases for class Singular
 *
 * Singular value decompositions
 *
 * @author Mikio L. Braun
 */
public class TestSingular {
  @Test
  public void testComplexDoubleSVD() {
    ComplexDoubleMatrix A = new ComplexDoubleMatrix(3, 4);

    for (int i = 0; i < A.rows; i++)
      for (int j = 0; j < A.columns; j++)
        A.put(i, j, (double) i, (double) j);

    ComplexDoubleMatrix[] USV = Singular.sparseSVD(A);
    ComplexDoubleMatrix U = USV[0];
    ComplexDoubleMatrix S = ComplexDoubleMatrix.diag(USV[1]);
    ComplexDoubleMatrix V = USV[2];

    assertEquals(3, U.rows);
    assertEquals(3, U.columns);
    assertEquals(4, V.rows);
    assertEquals(3, V.columns);
    assertEquals(0.0, U.mmul(S).mmul(V.hermitian()).sub(A).normmax(), 1e-10);

    USV = Singular.fullSVD(A);
    U = USV[0];
    S = ComplexDoubleMatrix.diag(USV[1], 3, 4);
    V = USV[2];

    assertEquals(3, U.rows);
    assertEquals(3, U.columns);
    assertEquals(3, S.rows);
    assertEquals(4, S.columns);
    assertEquals(4, V.rows);
    assertEquals(4, V.columns);

    assertEquals(0.0, U.mmul(S).mmul(V.hermitian()).sub(A).normmax(), 1e-10);
  }

  @Test
  public void testComplexFloatSVD() {
    ComplexFloatMatrix A = new ComplexFloatMatrix(3, 4);

    for (int i = 0; i < A.rows; i++)
      for (int j = 0; j < A.columns; j++)
        A.put(i, j, (float) i, (float) j);

    ComplexFloatMatrix[] USV = Singular.sparseSVD(A);
    ComplexFloatMatrix U = USV[0];
    ComplexFloatMatrix S = ComplexFloatMatrix.diag(USV[1]);
    ComplexFloatMatrix V = USV[2];

    assertEquals(3, U.rows);
    assertEquals(3, U.columns);
    assertEquals(4, V.rows);
    assertEquals(3, V.columns);
    assertEquals(0.0, U.mmul(S).mmul(V.hermitian()).sub(A).normmax(), 1e-4);

    USV = Singular.fullSVD(A);
    U = USV[0];
    S = ComplexFloatMatrix.diag(USV[1], 3, 4);
    V = USV[2];

    assertEquals(3, U.rows);
    assertEquals(3, U.columns);
    assertEquals(3, S.rows);
    assertEquals(4, S.columns);
    assertEquals(4, V.rows);
    assertEquals(4, V.columns);

    assertEquals(0.0, U.mmul(S).mmul(V.hermitian()).sub(A).normmax(), 1e-4);
  }
}
