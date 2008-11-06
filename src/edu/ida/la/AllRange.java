/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la;

/**
 *
 * @author mikio
 */
public class AllRange implements Range {
    private int lower;
    private int upper;
    private int value;
    private int counter;
    
    public AllRange() {}
    
    public void init(int l, int u) {
        lower = l;
        upper = u;
        value = l;
        counter = 0;
    }
    
    public int length() {
        return upper - lower + 1;
    }
    
    public int value() {
        return value;
    }
    
    public int index() {
        return counter;
    }
    
    public void next() {
        counter++;
        value++;
    }
    
    public boolean hasMore() {
        return value < upper;
    }
}
