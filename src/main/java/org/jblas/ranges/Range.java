// --- BEGIN LICENSE BLOCK ---
/* 
 * Copyright (c) 2009, Mikio L. Braun
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the Technische Universität Berlin nor the
 *       names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior
 *       written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// --- END LICENSE BLOCK ---

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.jblas.ranges;

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
