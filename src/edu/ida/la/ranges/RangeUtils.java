/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la.ranges;

import edu.ida.la.*;
import edu.ida.la.ranges.IndicesRange;
import edu.ida.la.ranges.AllRange;

/**
 * A bunch of static functions for making construction of ranges more
 * uniform. Basically, we have
 * <ul>
 * <li>point(3) - a PointRange.
 * <li>indices(new int[] {1,2,3,...}) - a Indices Range.
 * <li>interval(1, 2) - an interval range.
 * <li>all() - an AllRange.
 * <li>indices(x) - with a DoubleMatrix.
 * <li>find(x) - an index constructed from the non-zero elements of x.
 * </ul>
 * 
 */
public class RangeUtils {
    /** Construct point range (constant range) with given index. */
    public static Range point(int i) {
        return new PointRange(i);
    }
    
    public static Range indices(int[] is) {
        return new IndicesRange(is);
    }
    
    public static Range interval(int a, int b) {
        return new IntervalRange(a, b);
    }
    
    public static Range all() {
        return new AllRange();
    }
    
    public static Range indices(DoubleMatrix is) {
        return new IndicesRange(is);
    }
    
    public static Range find(DoubleMatrix is) {
        return new IndicesRange(is.findIndices());
    }
}
