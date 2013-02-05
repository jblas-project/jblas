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

package org.jblas;

import java.nio.FloatBuffer;

/**
 * A complex value with float precision.
 * 
 * @author Mikio L. Braun
 * 
 */
public class ComplexFloat {

    private float r,  i;
    public static final ComplexFloat UNIT = new ComplexFloat(1.0f, 0.0f);
    public static final ComplexFloat I = new ComplexFloat(0.0f, 1.0f);
    public static final ComplexFloat NEG_UNIT = new ComplexFloat(-1.0f, 0.0f);
    public static final ComplexFloat NEG_I = new ComplexFloat(0.0f, -1.0f);
    public static final ComplexFloat ZERO = new ComplexFloat(0.0f);

    public ComplexFloat(float real, float imag) {
        r = real;
        i = imag;
    }

    public ComplexFloat(float real) {
        this(real, 0.0f);
    }

    public String toString() {
        if (i >= 0) {
            return r + " + " + i + "i";
        } else {
            return r + " - " + (-i) + "i";
        }
    }

    public ComplexFloat set(float real, float imag) {
        r = real;
        i = imag;
        return this;
    }

    public float real() {
        return r;
    }

    public float imag() {
        return i;
    }

    public ComplexFloat dup() {
        return new ComplexFloat(r, i);
    }

    public ComplexFloat copy(ComplexFloat other) {
        r = other.r;
        i = other.i;
        return this;
    }

    /** Add two complex numbers in-place */
    public ComplexFloat addi(ComplexFloat c, ComplexFloat result) {
        if (this == result) {
            r += c.r;
            i += c.i;
        } else {
            result.r = r + c.r;
            result.i = i + c.i;
        }
        return result;
    }

    /** Add two complex numbers in-place storing the result in this. */
    public ComplexFloat addi(ComplexFloat c) {
        return addi(c, this);
    }

    /** Add two complex numbers. */
    public ComplexFloat add(ComplexFloat c) {
        return dup().addi(c);
    }

    /** Add a real number to a complex number in-place. */
    public ComplexFloat addi(float a, ComplexFloat result) {
        if (this == result) {
            r += a;
        } else {
            result.r = r + a;
            result.i = i;
        }
        return result;
    }

    /** Add a real number to complex number in-place, storing the result in this. */
    public ComplexFloat addi(float c) {
        return addi(c, this);
    }

    /** Add a real number to a complex number. */
    public ComplexFloat add(float c) {
        return dup().addi(c);
    }

    /** Subtract two complex numbers, in-place */
    public ComplexFloat subi(ComplexFloat c, ComplexFloat result) {
        if (this == result) {
            r -= c.r;
            i -= c.i;
        } else {
            result.r = r - c.r;
            result.i = i - c.i;
        }
        return this;
    }
    
    public ComplexFloat subi(ComplexFloat c) {
        return subi(c, this);
    }

    /** Subtract two complex numbers */
    public ComplexFloat sub(ComplexFloat c) {
        return dup().subi(c);
    }

    public ComplexFloat subi(float a, ComplexFloat result) {
        if (this == result) {
            r -= a;
        } else {
            result.r = r - a;
            result.i = i;
        }
        return result;
    }
    
    public ComplexFloat subi(float a) {
        return subi(a, this);
    }

    public ComplexFloat sub(float r) {
        return dup().subi(r);
    }

    /** Multiply two complex numbers, inplace */
    public ComplexFloat muli(ComplexFloat c, ComplexFloat result) {
        float newR = r * c.r - i * c.i;
        float newI = r * c.i + i * c.r;
        result.r = newR;
        result.i = newI;
        return result;
    }
    
    public ComplexFloat muli(ComplexFloat c) {
        return muli(c, this);
    }

    /** Multiply two complex numbers */
    public ComplexFloat mul(ComplexFloat c) {
        return dup().muli(c);
    }

    public ComplexFloat mul(float v) {
        return dup().muli(v);
    }

    public ComplexFloat muli(float v, ComplexFloat result) {
        if (this == result) {
            r *= v;
            i *= v;    
        } else {
            result.r = r * v;
            result.i = i * v;
        }
        return this;
    }
    
    public ComplexFloat muli(float v) {
        return muli(v, this);
    }

    /** Divide two complex numbers */
    public ComplexFloat div(ComplexFloat c) {
        return dup().divi(c);
    }

    /** Divide two complex numbers, in-place */
    public ComplexFloat divi(ComplexFloat c, ComplexFloat result) {
        float d = c.r * c.r + c.i * c.i;
        float newR = (r * c.r + i * c.i) / d;
        float newI = (i * c.r - r * c.i) / d;
        result.r = newR;
        result.i = newI;
        return result;
    }

    public ComplexFloat divi(ComplexFloat c) {
        return divi(c, this);
    }
    
    public ComplexFloat divi(float v, ComplexFloat result) {
        if (this == result) {
            r /= v;
            i /= v;
        } else {
            result.r = r / v;
            result.i = i / v;
        }
        return this;
    }
    
    public ComplexFloat divi(float v) {
        return divi(v, this);
    }

    public ComplexFloat div(float v) {
        return dup().divi(v);
    }

    /** Return the absolute value */
    public float abs() {
        return (float) Math.sqrt(r * r + i * i);
    }

    /** Returns the argument of a complex number. */
    public float arg() {
        return (float) Math.acos(r/abs());
    }

    public ComplexFloat invi() {
        float d = r * r + i * i;
        r = r / d;
        i = -i / d;
        return this;
    }
    
    public ComplexFloat inv() {
        return dup().invi();
    }

    public ComplexFloat neg() {
        return dup().negi();
    }

    public ComplexFloat negi() {
        r = -r;
        i = -i;
        return this;
    }

    public ComplexFloat conji() {
        i = -i;
        return this;
    }
    
    public ComplexFloat conj() {
        return dup().conji();
    }

    public ComplexFloat sqrt() {
        float a = abs();
        float s2 = (float)Math.sqrt(2);
        float p = (float)Math.sqrt(a + r)/s2;
        float q = (float)Math.sqrt(a - r)/s2 * Math.signum(i);
        return new ComplexFloat(p, q);
    }
    
    /**
     * Comparing two DoubleComplex values.
     */
    public boolean equals(Object o) {
        if (!(o instanceof ComplexFloat)) {
            return false;
        }
        ComplexFloat c = (ComplexFloat) o;

        return eq(c);
    }

    public boolean eq(ComplexFloat c) {
        return Math.abs(r - c.r) + Math.abs(i - c.i) < (float) 1e-6;
    }

    public boolean ne(ComplexFloat c) {
        return !eq(c);
    }

    public boolean isZero() {
        return r == 0.0f && i == 0.0f;
    }
    
    public boolean isReal() {
        return i == 0.0f;
    }
    
    public boolean isImag() {
        return r == 0.0f;
    }
}