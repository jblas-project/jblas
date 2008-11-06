/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

/**
 *
 * @author mikio
 */
public interface Range {
    public void init(int lower, int upper);
    public int length();
    public void next();
    public int index();
    public int value();
    public boolean hasMore();
}
