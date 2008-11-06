/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

/**
 *
 * @author mikio
 */
public class IndicesRange implements Range {
    private int[] indices;
    private int counter;
        
    public IndicesRange(int[] is) {
        indices = is;
    }
    
    public void init(int l, int u) {
        counter = 0;
    }
    
    public IndicesRange(DoubleMatrix is) {
        this(is.toIntArray());
    }
    
    public int length() {
        return indices.length;
    }
    
    public void next() {
        counter++;
    }
    
    public int index() {
        return counter;
    }

    public int value() {
        return indices[counter];
    }
    
    public boolean hasMore() {
        return counter < indices.length;
    }
}
