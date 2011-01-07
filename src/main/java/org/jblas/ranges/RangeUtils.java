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

import org.jblas.*;

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
