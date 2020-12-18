package org.jblas;

import org.jblas.exceptions.SizeException;

/**
 * This class provides some common function interfaces of m*n matrices.
 * This class is distinguished from the <class>SimpleBlas</class> which
 * provides an interface for the computation of one row or column matrix dot product.
 * For instance,it provides an interface of the m*n and n*p matrix inner product.
 * <p/>
 * For example, you can do any legitimate m*n matrix operation
 * <p/>
 * Currently, only implements inner,scalar matrix
 */
public class CommonBlas {

    /**
     * inner product of x with y <- x.y
     *
     * @return result of compution
     * @throws SizeException x colums is inconsistent with y rows
     */
    public static FloatMatrix inner(FloatMatrix x,FloatMatrix y) throws SizeException {
        FloatMatrix x_t = x.transpose();
        if (x_t.rows == y.rows) {
            FloatMatrix result = new FloatMatrix(x_t.columns,y.columns);
            for (int i = 0; i < x_t.columns; i++) {
                for (int j = 0; j < y.columns; j++) {
                    float value_ij = SimpleBlas.dot(x_t.getColumn(i), y.getColumn(j));
                    result.put(i, j, value_ij);
                }
            }
            return result;
        }else
            throw new SizeException("x colums is inconsistent with y rows");
    }

    /**
     * inner product of x with y <- x.y
     *
     * @return result of compution
     * @throws SizeException x colums is inconsistent with y rows
     */
    public static DoubleMatrix inner(DoubleMatrix x,DoubleMatrix y) throws SizeException {
        DoubleMatrix x_t = x.transpose();
        if (x_t.rows == y.rows) {
            DoubleMatrix result = new DoubleMatrix(x_t.columns,y.columns);
            for (int i = 0; i < x_t.columns; i++) {
                for (int j = 0; j < y.columns; j++) {
                    double value_ij = SimpleBlas.dot(x_t.getColumn(i), y.getColumn(j));
                    result.put(i, j, value_ij);
                }
            }
            return result;
        }else
            throw new SizeException("x colums is inconsistent with y rows");
    }

    /**
     * provide a instance of n*n float scalar matrix which value is n
     * @param rows rows=column
     * @param n value
     * @return result
     */
    public static FloatMatrix floatScalarMatrixInstance(int rows,float n){
        FloatMatrix result = new FloatMatrix(rows,rows);
        for(int i=0;i<rows;i++){
            result.put(i,i,n);
        }
        return result;
    }

    /**
     * provide a instance of n*n double scalar matrix which value is n
     * @param rows rows=column
     * @param n value
     * @return result
     */
    public static DoubleMatrix doubleScalarMatrixInstance(int rows,double n){
        DoubleMatrix result = new DoubleMatrix(rows,rows);
        for(int i=0;i<rows;i++){
            result.put(i,i,n);
        }
        return result;
    }
}
