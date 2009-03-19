/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.la.exceptions;

/**
 *
 * @author mikio
 */
public class LapackConvergenceException extends LapackException {
  	public LapackConvergenceException(String function, String msg) {
		super(function, msg);
	}

}
