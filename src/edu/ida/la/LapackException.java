package edu.ida.la;

public class LapackException extends RuntimeException {
	LapackException(String function) {
		super("LAPACK " + function);
	}
	
	LapackException(String function, String message) {
		super("LAPACK " + function + ": " + message);
	}
}
