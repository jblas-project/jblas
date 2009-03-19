package org.jblas.la;

/**
 * <p>Eigenvalue and Eigenvector related functions.</p>
 * 
 * <p>Methods exist for working with symmetric matrices or general eigenvalues.
 * The symmetric versions are usually much faster on symmetric matrices.</p>
 */
public class Eigen {
        private static final DoubleMatrix dummyDouble = new DoubleMatrix(1);
    
        /** Compute the eigenvalues for a symmetric matrix. */
	public static DoubleMatrix symmetricEigenvalues(DoubleMatrix A) {
		A.assertSquare();
		DoubleMatrix eigenvalues = new DoubleMatrix(A.rows);
        int isuppz[] = new int[2*A.rows];
		SimpleBlas.syevr('N', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, dummyDouble, isuppz);
		return eigenvalues;
	}

        /** 
         * Computes the eigenvalues and eigenvectors for a symmetric matrix. 
         *
         * @return an array of DoubleMatrix objects containing the eigenvectors 
         * stored as the columns of the first matrix, and the eigenvalues as
         * diagonal elements of the second matrix.
         */
	public static DoubleMatrix[] symmetricEigenvectors(DoubleMatrix A) {
		A.assertSquare();
		DoubleMatrix eigenvalues = new DoubleMatrix(A.rows);
		DoubleMatrix eigenvectors = A.dup();
        int isuppz[] = new int[2*A.rows];
        SimpleBlas.syevr('V', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, eigenvectors, isuppz);
		return new DoubleMatrix[] { eigenvectors, DoubleMatrix.diag(eigenvalues) };
	}
        
        /** Computes the eigenvalues of a general matrix. */
        public static ComplexDoubleMatrix eigenvalues(DoubleMatrix A) {
            A.assertSquare();
            DoubleMatrix WR = new DoubleMatrix(A.rows);
            DoubleMatrix WI = WR.dup();
            SimpleBlas.geev('N', 'N', A.dup(), WR, WI, dummyDouble, dummyDouble);
                    
            return new ComplexDoubleMatrix(WR, WI);
        }
        
        /** 
         * Computes the eigenvalues and eigenvectors of a general matrix.
         * 
         * @return an array of ComplexDoubleMatrix objects containing the eigenvectors
         * stored as the columns of the first matrix, and the eigenvalues as the
         * diagonal elements of the second matrix.
         */
        public static ComplexDoubleMatrix[] eigenvectors(DoubleMatrix A) {
            A.assertSquare();
            // setting up result arrays
            DoubleMatrix WR = new DoubleMatrix(A.rows);
            DoubleMatrix WI = WR.dup();
            DoubleMatrix VR = new DoubleMatrix(A.rows, A.rows);

            SimpleBlas.geev('N', 'V', A.dup(), WR, WI, dummyDouble, VR);
            
            // transferring the result
            ComplexDoubleMatrix E = new ComplexDoubleMatrix(WR, WI);
            ComplexDoubleMatrix V = new ComplexDoubleMatrix(A.rows, A.rows);
            System.err.printf("VR = %s\n", VR.toString());
            for (int i = 0; i < A.rows; i++) {
                if (E.get(i).isReal()) {
                    V.putColumn(i, new ComplexDoubleMatrix(VR.getColumn(i)));
                } else {
                    ComplexDoubleMatrix v = new ComplexDoubleMatrix(VR.getColumn(i), VR.getColumn(i+1));
                    V.putColumn(i, v);
                    V.putColumn(i+1, v.conji());
                    i += 1;
                }
            }
            return new ComplexDoubleMatrix[] { V, ComplexDoubleMatrix.diag(E) };
        }
//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
        private static final FloatMatrix dummyFloat = new FloatMatrix(1);
    
        /** Compute the eigenvalues for a symmetric matrix. */
	public static FloatMatrix symmetricEigenvalues(FloatMatrix A) {
		A.assertSquare();
		FloatMatrix eigenvalues = new FloatMatrix(A.rows);
        int isuppz[] = new int[2*A.rows];
		SimpleBlas.syevr('N', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, dummyFloat, isuppz);
		return eigenvalues;
	}

        /** 
         * Computes the eigenvalues and eigenvectors for a symmetric matrix. 
         *
         * @return an array of FloatMatrix objects containing the eigenvectors 
         * stored as the columns of the first matrix, and the eigenvalues as
         * diagonal elements of the second matrix.
         */
	public static FloatMatrix[] symmetricEigenvectors(FloatMatrix A) {
		A.assertSquare();
		FloatMatrix eigenvalues = new FloatMatrix(A.rows);
		FloatMatrix eigenvectors = A.dup();
        int isuppz[] = new int[2*A.rows];
        SimpleBlas.syevr('V', 'A', 'U', A.dup(), 0, 0, 0, 0, 0, eigenvalues, eigenvectors, isuppz);
		return new FloatMatrix[] { eigenvectors, FloatMatrix.diag(eigenvalues) };
	}
        
        /** Computes the eigenvalues of a general matrix. */
        public static ComplexFloatMatrix eigenvalues(FloatMatrix A) {
            A.assertSquare();
            FloatMatrix WR = new FloatMatrix(A.rows);
            FloatMatrix WI = WR.dup();
            SimpleBlas.geev('N', 'N', A.dup(), WR, WI, dummyFloat, dummyFloat);
                    
            return new ComplexFloatMatrix(WR, WI);
        }
        
        /** 
         * Computes the eigenvalues and eigenvectors of a general matrix.
         * 
         * @return an array of ComplexFloatMatrix objects containing the eigenvectors
         * stored as the columns of the first matrix, and the eigenvalues as the
         * diagonal elements of the second matrix.
         */
        public static ComplexFloatMatrix[] eigenvectors(FloatMatrix A) {
            A.assertSquare();
            // setting up result arrays
            FloatMatrix WR = new FloatMatrix(A.rows);
            FloatMatrix WI = WR.dup();
            FloatMatrix VR = new FloatMatrix(A.rows, A.rows);

            SimpleBlas.geev('N', 'V', A.dup(), WR, WI, dummyFloat, VR);
            
            // transferring the result
            ComplexFloatMatrix E = new ComplexFloatMatrix(WR, WI);
            ComplexFloatMatrix V = new ComplexFloatMatrix(A.rows, A.rows);
            System.err.printf("VR = %s\n", VR.toString());
            for (int i = 0; i < A.rows; i++) {
                if (E.get(i).isReal()) {
                    V.putColumn(i, new ComplexFloatMatrix(VR.getColumn(i)));
                } else {
                    ComplexFloatMatrix v = new ComplexFloatMatrix(VR.getColumn(i), VR.getColumn(i+1));
                    V.putColumn(i, v);
                    V.putColumn(i+1, v.conji());
                    i += 1;
                }
            }
            return new ComplexFloatMatrix[] { V, ComplexFloatMatrix.diag(E) };
        }
//END
}
