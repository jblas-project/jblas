package edu.ida.la;

/**
 * General functions which are geometric in nature.
 * 
 * For example, computing all pairwise squared distances between all columns of a matrix.
 */
public class Geometry {
	
	/**
	 * Compute the pairwise squared distances between all columns of the two
	 * matrices.
	 * 
	 * An efficient way to do this is to observe that (x-y)^2 = x^2 - 2xy - y^2
	 * and to then properly carry out the computation with matrices.
	 */
	public static DoubleMatrix pairwiseSquaredDistances(DoubleMatrix X, DoubleMatrix Y) {
		if (X.rows != Y.rows)
			throw new IllegalArgumentException(
					"Matrices must have same number of rows");
	
		DoubleMatrix XX = X.mul(X).columnSums();
		DoubleMatrix YY = Y.mul(Y).columnSums();
	
		DoubleMatrix Z = X.transpose().mmul(Y);
		Z.muli(-2.0); //Z.print();
		Z.addiColumnVector(XX);
		Z.addiRowVector(YY);
	
		return Z;
	}

	public static DoubleMatrix center(DoubleMatrix x) {
		return x.subi(x.mean());
	}
	
	public static DoubleMatrix centerRows(DoubleMatrix x) {
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, center(x.getRow(r)));
		return x;
	}
	
	public static DoubleMatrix centerColumns(DoubleMatrix x) {
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, center(x.getColumn(c)));
		return x;
	}
	
	public static DoubleMatrix normalize(DoubleMatrix x) {
		return x.divi(x.norm2());
	}

	public static DoubleMatrix normalizeRows(DoubleMatrix x) {
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, normalize(x.getRow(r)));
		return x;
	}
	
	public static DoubleMatrix normalizeColumns(DoubleMatrix x) {
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, normalize(x.getColumn(c)));
		return x;
	}

//BEGIN
  // The code below has been automatically generated.
  // DO NOT EDIT!
	
	/**
	 * Compute the pairwise squared distances between all columns of the two
	 * matrices.
	 * 
	 * An efficient way to do this is to observe that (x-y)^2 = x^2 - 2xy - y^2
	 * and to then properly carry out the computation with matrices.
	 */
	public static FloatMatrix pairwiseSquaredDistances(FloatMatrix X, FloatMatrix Y) {
		if (X.rows != Y.rows)
			throw new IllegalArgumentException(
					"Matrices must have same number of rows");
	
		FloatMatrix XX = X.mul(X).columnSums();
		FloatMatrix YY = Y.mul(Y).columnSums();
	
		FloatMatrix Z = X.transpose().mmul(Y);
		Z.muli(-2.0f); //Z.print();
		Z.addiColumnVector(XX);
		Z.addiRowVector(YY);
	
		return Z;
	}

	public static FloatMatrix center(FloatMatrix x) {
		return x.subi(x.mean());
	}
	
	public static FloatMatrix centerRows(FloatMatrix x) {
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, center(x.getRow(r)));
		return x;
	}
	
	public static FloatMatrix centerColumns(FloatMatrix x) {
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, center(x.getColumn(c)));
		return x;
	}
	
	public static FloatMatrix normalize(FloatMatrix x) {
		return x.divi(x.norm2());
	}

	public static FloatMatrix normalizeRows(FloatMatrix x) {
		for (int r = 0; r < x.rows; r++)
			x.putRow(r, normalize(x.getRow(r)));
		return x;
	}
	
	public static FloatMatrix normalizeColumns(FloatMatrix x) {
		for (int c = 0; c < x.columns; c++)
			x.putColumn(c, normalize(x.getColumn(c)));
		return x;
	}

//END
}
