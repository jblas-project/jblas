package org.jblas.la.exceptions;

/**
 * Exception thrown when matrices are singular.
 */
public class LapackSingularityException extends LapackException {
    public LapackSingularityException(String fct, String msg) {
        super(fct, msg);
    }
}
