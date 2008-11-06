package edu.ida.la;

public class LapackArgumentException extends LapackException {
	LapackArgumentException(String function, int info) {
		super(function, "Argument " + info + " had an illegal value.");
	}
}
