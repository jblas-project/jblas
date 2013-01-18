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
package <%= package %>;

import org.jblas.util.Logger;

/**
 * Native BLAS and LAPACK functions.
 *
 * <p>The <%= classname %> class contains the native BLAS and LAPACK functions. Each
 * Fortran function is mapped to a static method of this class. For each array argument,
 * an additional parameter is introduced which gives the offset from the beginning of
 * the passed array. In C, you would be able to pass a different pointer, but
 * in Java, you can only pass the whole array.</p>
 *
 * <p>Note that due to the way the JNI is usually implemented, the arrays are first
 * copied outside of the JVM before the function is called. This means that
 * functions whose runtime is linear in the amount of memory do usually not
 * run faster just because you are using a native implementation. This holds true
 * for most Level 1 BLAS routines (like vector addition), and unfortunately also
 * for most Level 2 BLAS routines (like matrix-vector multiplication). For these,
 * there exists a class JavaBlas which contains Java implementations.</p>
 *
 * <p>In LAPACK, there exist routines which require workspace to be allocated together
 * with a standard procedure for computing the size of these workspaces. jblas
 * automatically also generates wrappers for these routines with automatic
 * workspace allocation. These routines have the same name, but the workspace
 * arguments are removed.</p>
 *
 * <p>Finally, an example: The fortran routine<pre>
 * SUBROUTINE DAXPY(N,DA,DX,INCX,DY,INCY)
 *     DOUBLE PRECISION DA
 *     INTEGER INCX,INCY,N
 *     DOUBLE PRECISION DX(*),DY(*)
 * </pre>
 * becomes <pre>
 * public static native void daxpy(int n, double da, double[] dx, int dxIdx,
 *    int incx, double[] dy, int dyIdx, int incy);
 * </pre>
 */
public class <%= classname %> {

  static {
    NativeBlasLibraryLoader.loadLibraryAndCheckErrors();
  }

  private static int[] intDummy = new int[1];
  private static double[] doubleDummy = new double[1];
  private static float[] floatDummy = new float[1];

<% for r in routines -%>
<%= generate_native_declaration r %>
<% end %>
}
