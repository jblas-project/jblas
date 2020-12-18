package org.jblas;

import org.junit.Test;

import static org.junit.Assert.assertTrue;

/**
 * Some test for class CommonBlas
 *
 * @author Jason Chen
 */
public class CommonBlasTest {
    @Test
    public void testInner() {
        DoubleMatrix A = new DoubleMatrix(2, 2, 3.0, -3.0, 1.0, 1.0);
        DoubleMatrix B = new DoubleMatrix(2,2,1.0,-1.0,2.0,4.0);
        DoubleMatrix C = new DoubleMatrix(2,2,2.0,-4.0,10.0,-2.0);

        FloatMatrix D = new FloatMatrix(2, 2, 3.0f, -3.0f, 1.0f, 1.0f);
        FloatMatrix E = new FloatMatrix(2,2,1.0f,-1.0f,2.0f,4.0f);
        FloatMatrix F = new FloatMatrix(2,2,2.0f,-4.0f,10.0f,-2.0f);

        assertTrue(CommonBlas.inner(A,B).equals(C));
        assertTrue(CommonBlas.inner(D,E).equals(F));
    }

    @Test
    public void testScalar(){
        DoubleMatrix scalarDoubleMatrix = CommonBlas.doubleScalarMatrixInstance(2,-9.0);
        FloatMatrix scalarFloatMatrix = CommonBlas.floatScalarMatrixInstance(2,-8.0f);

        assertTrue(new DoubleMatrix(2,2,-9.0,0.0,0.0,-9.0).equals(scalarDoubleMatrix));
        assertTrue(new FloatMatrix(2,2,-8.0f,0,0,-8.0f).equals(scalarFloatMatrix));
    }
}
