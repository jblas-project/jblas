/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.exceptions;

/**
 *
 */
public class LapackPositivityException extends LapackException {
    public LapackPositivityException(String fct, String msg) {
        super(fct, msg);
    }
}
