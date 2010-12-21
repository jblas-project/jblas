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
