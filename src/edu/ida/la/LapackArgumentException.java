package edu.ida.la;

/**
 * LapackException for a specific argument. LAPACK routines routinely check
 * whether some arguments contain illegal arguments. This exception class
 * automatically constructs a message for a given argument index.
 */
public class LapackArgumentException extends LapackException {
    /** 
     * Construct exception for given function and info. Message
     * will read "Argument <info> had an illegal value.");
     */
	public LapackArgumentException(String function, int info) {
		super(function, "Argument " + info + " had an illegal value.");
	}
}
