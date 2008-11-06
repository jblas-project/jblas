package edu.ida.la;

/**
 * Eigenvalue and Eigenvector related functions.
 * 
 * Comment on the used routines: On my computer, I found that for matrices up to 2000 points, 
 * syevd is faster than syev. I couldn't get syev[xr] to run :(
 */
public class Eigen {
        private static final DoubleMatrix dummyDouble = new DoubleMatrix(1);
    
	public static DoubleMatrix symmetricEigenvalues(DoubleMatrix A) {
		A.assertSquare();
		DoubleMatrix eigenvalues = new DoubleMatrix(A.rows);
		SimpleBlas.syevd('N', 'U', A.dup(), eigenvalues);
		return eigenvalues;
	}
	
	public static DoubleMatrix[] symmetricEigenvectors(DoubleMatrix A) {
		A.assertSquare();
		DoubleMatrix eigenvalues = new DoubleMatrix(A.rows);
		DoubleMatrix eigenvectors = A.dup();
		SimpleBlas.syevd('V', 'U', eigenvectors, eigenvalues);
		return new DoubleMatrix[] { eigenvectors, DoubleMatrix.diag(eigenvalues) };
	}
        
        public static ComplexDoubleMatrix eigenvalues(DoubleMatrix A) {
            A.assertSquare();
            DoubleMatrix WR = new DoubleMatrix(A.rows);
            DoubleMatrix WI = WR.dup();
            SimpleBlas.geev('N', 'N', A.dup(), WR, WI, dummyDouble, dummyDouble);
                    
            return new ComplexDoubleMatrix(WR, WI);
        }
        
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
    
	public static FloatMatrix symmetricEigenvalues(FloatMatrix A) {
		A.assertSquare();
		FloatMatrix eigenvalues = new FloatMatrix(A.rows);
		SimpleBlas.syevd('N', 'U', A.dup(), eigenvalues);
		return eigenvalues;
	}
	
	public static FloatMatrix[] symmetricEigenvectors(FloatMatrix A) {
		A.assertSquare();
		FloatMatrix eigenvalues = new FloatMatrix(A.rows);
		FloatMatrix eigenvectors = A.dup();
		SimpleBlas.syevd('V', 'U', eigenvectors, eigenvalues);
		return new FloatMatrix[] { eigenvectors, FloatMatrix.diag(eigenvalues) };
	}
        
        public static ComplexFloatMatrix eigenvalues(FloatMatrix A) {
            A.assertSquare();
            FloatMatrix WR = new FloatMatrix(A.rows);
            FloatMatrix WI = WR.dup();
            SimpleBlas.geev('N', 'N', A.dup(), WR, WI, dummyFloat, dummyFloat);
                    
            return new ComplexFloatMatrix(WR, WI);
        }
        
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
