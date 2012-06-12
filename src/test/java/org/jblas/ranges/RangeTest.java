package org.jblas.ranges;

import org.jblas.DoubleMatrix;
import org.junit.Test;
import static org.jblas.ranges.RangeUtils.*;
import static org.jblas.JblasAssert.*;

/**
 * Testing the ranges facility.
 *
 * @author Mikio L. Braun
 * File created June 12, 2012, 16:04
 */

public class RangeTest {

  private DoubleMatrix A = new DoubleMatrix(3, 4, 1.0, 2.0, 3.0,
                                                  4.0, 5.0, 6.0,
                                                  7.0, 8.0, 9.0,
                                                 10.0, 11.0, 12.0);

  @Test
  public void allRange() {
    assertEquals(A, A.get(all(), all()));
    assertEquals(new DoubleMatrix(3, 1, 1.0, 2.0, 3.0), A.get(all(), 0));
    assertEquals(new DoubleMatrix(3, 1, 7.0, 8.0, 9.0), A.get(all(), 2));
  }

  @Test
  public void pointRange() {
    assertEquals(DoubleMatrix.scalar(8.0), A.get(point(1), point(2)));
  }

  @Test
  public void indicesRange() {
    assertEquals(new DoubleMatrix(1, 5, 1.0, 7.0, 4.0, 10.0, 1.0), A.get(0, indices(new int[] {0, 2, 1, 3, 0})));
  }

  @Test
  public void mixedRanges() {
    assertEquals(new DoubleMatrix(3, 2, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0), A.get(all(), interval(1, 3)));
    assertEquals(new DoubleMatrix(2, 1, 11.0, 12.0), A.get(interval(1, 3), point(3)));
  }
}
