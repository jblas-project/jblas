/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

/**
 *
 * @author mikio
 */
public class PointRange implements Range {
    private int value;
    private boolean consumed; 
    
    public PointRange(int v) {
        value = v;
    }
    
    public void init(int l, int u) {
        consumed = false;
    }

    public int length() {
        return 1;
    }
    
    public int value() {
        return value;
    }
    
    public int index() {
        return 0;
    }
    
    public void next() {
        consumed = true;
    }
    
    public boolean hasMore() {
        return !consumed;
    }
}
