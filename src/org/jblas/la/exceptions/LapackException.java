package org.jblas.la.exceptions;

/**
 * Base class for all exceptions within LAPACK. Also reports the function where the
 * error is.
 */
public class LapackException extends RuntimeException {
        /** Construct new LapackException for the given function. */
	public LapackException(String function) {
		super("LAPACK " + function);
	}
	
        /** Construct new Lapack Exception for the given function, with message. */
	public LapackException(String function, String message) {
		super("LAPACK " + function + ": " + message);
	}
}
