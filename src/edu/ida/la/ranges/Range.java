/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package edu.ida.la.ranges;

/**
 * <p>The Range interface represents basically a set of indices. Before using a range
 * you have to call init() with the actually available lower and upper bounds, such 
 * that you can also have an "AllRange" which contains all possible indices.</p>
 * 
 * <p>Further operations include:</p>
 * 
 * <ul>
 * <li> length() - returns total number of elements.
 * <li> next() - increase counter (use value()) to retrieve the value.
 * <li> index() - get the index of the current value.
 * <li> value() - get the current value.
 * <li> hasMore() - more indices available.
 * </ul>
 * 
 * <p>Typical uses look like this:</p>
 * <pre>    for (r.init(lower, upper); r.hasMore(); r.next()) {
 *       System.out.printf("Value number %d is %d%n", index(), value());
 *    }</pre>
 */
public interface Range {
    /** Initialize Range to available indices */
    public void init(int lower, int upper);
    /** Total number of indices. */
    public int length();
    /** Increase counter. */
    public void next();
    /** Consecutive numbering of current index. */
    public int index();
    /** Get current index. */
    public int value();
    /** More indices available? */
    public boolean hasMore();
}
