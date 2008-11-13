/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

/**
 * Range which varies over pre-specified indices.
 * 
 * For example,
 * <pre>
 *     int[] indices = new int[] { 1, 1, 2, 3, 5, 8, 13 };
 *     Range r = new IndicesRange(indices);</pre>
 * ranges over the first few Fibonacci numbers.
 */
public class IndicesRange implements Range {
    private int[] indices;
    private int counter;
        
    /** Initialize from integer array. */
    public IndicesRange(int[] is) {
        indices = is;
    }
    
    public void init(int l, int u) {
        counter = 0;
    }
    
    /** 
     * Initialize from DoubleMatrix. Entries are converted to integers
     * by truncation.
     */
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
