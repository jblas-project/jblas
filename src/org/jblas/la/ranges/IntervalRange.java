/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.jblas.la.ranges;

/**
 * Range which varies from a given interval. Endpoints are both inclusive!
 */
public class IntervalRange implements Range {
    private int start;
    private int end;
    private int value;
    private int counter;

    /** Construct new interval range. Endpoints are inclusive. */
    public IntervalRange(int a, int b) {
        start = a;
        end = b;
    }

    public void init(int lower, int upper) {
        value = start;
        counter = 0;
        if (start < lower || end > upper) {
            throw new IllegalArgumentException("Bounds " + lower + " to " + upper + " are beyond range interval " + start + " to " + end + ".");
        }
    }

    public int length() {
        return end - start + 1;
    }

    public void next() {
        counter++;
        value++;
    }
    
    public int index() {
        return counter;
    }
    
    public int value() {
        return value;
    }

    public boolean hasMore() {
        return counter < end;
    }
}
