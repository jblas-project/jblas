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

import org.junit.*;
import static org.junit.Assert.*;

public class TestBlasDoubleComplex {

    @Test
    public void testZCOPY() {
        double[] a = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
        double[] b = new double[6];
        NativeBlas.zcopy(3, a, 0, 1, b, 0, 1);

        for (int i = 0; i < 6; i++) {
            assertEquals((double)(i+1), b[i], 1e-6);
        }
    }

    @Test
    public void testZDOTU() {
        double[] a = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};

        ComplexDouble c = NativeBlas.zdotu(3, a, 0, 1, a, 0, 1);
        assertEquals(new ComplexDouble(-21.0, 88.0), c);
    }

    @Test
    public void testAxpy() {
        double[] x = {0.0, -1.0};
        double[] y = {0.0, 1.0};
        ComplexDouble a = new ComplexDouble(0.0, 1.0);

        // compute x = 2 * x
        NativeBlas.zdscal(1, 2.0, x, 0, 1);
        assertEquals(new ComplexDouble(0.0, -2.0), new ComplexDouble(x[0], x[1]));

        // compute -I * x + y
        NativeBlas.zaxpy(1, a, x, 0, 1, y, 0, 1);
        assertEquals(new ComplexDouble(2.0, 1.0), new ComplexDouble(y[0], y[1]));
    }
}
