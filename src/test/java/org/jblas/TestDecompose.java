package org.jblas;

import org.junit.*;
import static org.junit.Assert.*;

/**
 * Test class for Decompose
 *
 * @author Mikio Braun
 */
public class TestDecompose {
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

  @Test
  public void qrDouble() {
    DoubleMatrix A = new DoubleMatrix(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);

    Decompose.QRDecomposition<DoubleMatrix> qr = Decompose.qr(A);

    assertEquals(0.0, DoubleMatrix.eye(3).sub(qr.q.transpose().mmul(qr.q)).normmax(), 1e-10);
    assertTrue(qr.r.isUpperTriangular());
    assertEquals(0.0, A.sub(qr.q.mmul(qr.r)).normmax(), 1e-10);
  }

  @Test
  public void qrRectangularDouble() {
    DoubleMatrix A = new DoubleMatrix(2, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

    Decompose.QRDecomposition<DoubleMatrix> qr = Decompose.qr(A);

    assertEquals(0.0, DoubleMatrix.eye(2).sub(qr.q.transpose().mmul(qr.q)).normmax(), 1e-10);
    assertTrue(qr.r.isUpperTriangular());
    assertEquals(0.0, A.sub(qr.q.mmul(qr.r)).normmax(), 1e-10);
  }

  @Test
  public void qrRectangular2Double() {
    DoubleMatrix A = new DoubleMatrix(4, 2, 1.0, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 6.5);

    Decompose.QRDecomposition<DoubleMatrix> qr = Decompose.qr(A);

    DoubleMatrix qtq = qr.q.transpose().mmul(qr.q);

    assertEquals(0.0, DoubleMatrix.eye(4).sub(qtq).normmax(), 1e-10);
    assertTrue(qr.r.isUpperTriangular());
    assertEquals(0.0, A.sub(qr.q.mmul(qr.r)).normmax(), 1e-10);
  }


  @Test
  public void qrFloat() {
    FloatMatrix A = new FloatMatrix(3, 3, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);

    Decompose.QRDecomposition<FloatMatrix> qr = Decompose.qr(A);

    assertEquals(0.0f, FloatMatrix.eye(3).sub(qr.q.transpose().mmul(qr.q)).normmax(), 1e-5f);
    assertTrue(qr.r.isUpperTriangular());
    assertEquals(0.0f, A.sub(qr.q.mmul(qr.r)).normmax(), 1e-5f);
  }

  @Test
  public void qrRectangularFloat() {
    //FloatMatrix A = new FloatMatrix(2, 3, 1.0f, 2.0f, 3.0f, 4.0f, 7.0f, 6.0f);
    FloatMatrix A = FloatMatrix.rand(2, 3);

    Decompose.QRDecomposition<FloatMatrix> qr = Decompose.qr(A);

    assertEquals(0.0f, FloatMatrix.eye(2).sub(qr.q.transpose().mmul(qr.q)).normmax(), 1e-5f);
    assertTrue(qr.r.isUpperTriangular());
    assertEquals(0.0f, A.sub(qr.q.mmul(qr.r)).normmax(), 1e-5f);
  }
}
